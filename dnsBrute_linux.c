// dnsBrute_linux.c
// Multithreading, mutex, prevenção de Buffer Overflow.
// Compilação: gcc dnsBrute_linux.c -o dnsBrute_linux -lpthread
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// Estrutura para passar dados para as threads
typedef struct
{
    char *subdomain;
    char *domain;
    FILE *output_file;
} thread_args;

// Mutex global para sincronizar a escrita no arquivo
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

// Função que será executada por cada thread
void *resolve_dns(void *args)
{
    thread_args *t_args = (thread_args *)args;
    char fulldomain[1024];
    char ip_str[INET6_ADDRSTRLEN];

    // Monta o nome de domínio completo de forma segura
    snprintf(fulldomain, sizeof(fulldomain), "%s.%s", t_args->subdomain, t_args->domain);

    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Aceita IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;

    // Tenta resolver o nome de domínio
    if (getaddrinfo(fulldomain, NULL, &hints, &res) != 0)
    {
        // Falha na resolução, subdomínio provavelmente não existe.
        free(t_args->subdomain);
        free(t_args);
        return NULL;
    }

    // Itera sobre os resultados (pode haver mais de um IP)
    for (p = res; p != NULL; p = p->ai_next)
    {
        void *addr;

        if (p->ai_family == AF_INET)
        { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else
        { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Converte o endereço IP de binário para texto
        inet_ntop(p->ai_family, addr, ip_str, sizeof(ip_str));

        // Exibe o resultado encontrado diretamente no console
        printf("Encontrado: %s -> %s\n", fulldomain, ip_str);

        // Bloqueia o mutex antes de escrever no arquivo para evitar conflitos
        pthread_mutex_lock(&file_mutex);

        fprintf(t_args->output_file, "%s -> %s\n", fulldomain, ip_str);
        fflush(t_args->output_file); // Garante a escrita imediata dos dados

        // Libera o mutex
        pthread_mutex_unlock(&file_mutex);
    }

    freeaddrinfo(res);       // Libera a memória alocada por getaddrinfo
    free(t_args->subdomain); // Libera a cópia do subdomínio
    free(t_args);            // Libera a estrutura de argumentos

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Modo de uso: %s <DOMINIO> <WORDLIST> <ARQUIVO_SAIDA>\n", argv[0]);
        return 1;
    }

    const char *domain = argv[1];
    const char *wordlist_path = argv[2];
    const char *output_path = argv[3];

    FILE *wordlist_file = fopen(wordlist_path, "r");
    if (!wordlist_file)
    {
        perror("Erro ao abrir a wordlist");
        return 1;
    }

    FILE *output_file = fopen(output_path, "w");
    if (!output_file)
    {
        perror("Erro ao criar o arquivo de saída");
        fclose(wordlist_file);
        return 1;
    }

    printf("Iniciando busca por subdomínios... Resultados em '%s'\n", output_path);

    char line[1024];
    int thread_count = 0;
    pthread_t threads[50]; // Pool de 50 threads

    while (fgets(line, sizeof(line), wordlist_file))
    {
        line[strcspn(line, "\r\n")] = 0; // Remove quebras de linha

        thread_args *args = malloc(sizeof(thread_args));
        args->subdomain = strdup(line); // strdup aloca memória e copia a string
        args->domain = (char *)domain;
        args->output_file = output_file;

        // Cria uma nova thread para resolver o subdomínio atual
        pthread_create(&threads[thread_count++], NULL, resolve_dns, args);

        // Se o pool de threads estiver cheio, aguarda todas terminarem
        if (thread_count == 50)
        {
            for (int i = 0; i < thread_count; i++)
            {
                pthread_join(threads[i], NULL);
            }
            thread_count = 0;
        }
    }

    // Aguarda as threads restantes que não completaram o pool de 50
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("[+] Busca finalizada. [+]\n");

    // Limpeza
    fclose(wordlist_file);
    fclose(output_file);
    pthread_mutex_destroy(&file_mutex);

    return 0;
}