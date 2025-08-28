# DNS Brute Force Tool para Linux
Uma ferramenta de linha de comando multithread, desenvolvida em C, para descobrir subdomínios válidos através da busca por brute force, utilizando palavras de uma wordlist.

### Sobre a Ferranenta
Este script foi criado para automatizar a busca por subdomínios a partir de um domínio alvo e uma lista de palavras (wordlist). Ele é otimizado para desempenho e segurança, utilizando recursos nativos do ambiente Linux.

### Características Principais
- **Alto Desempenho com Multithreading**: Utiliza um pool de até 50 threads (pthreads) para realizar múltiplas consultas DNS simultaneamente, tornando a busca bastante rápida.

- **Segurança na Escrita**: Emprega um mutex para controlar o acesso ao arquivo de saída, garantindo que não haja conflitos ou dados corrompidos quando múltiplas threads encontram resultados ao mesmo tempo.

- **Prevenção de Buffer Overflow**: Utiliza a função snprintf para a concatenação de strings, evitando riscos de buffer overflow.

- **Resolução de DNS Moderna**: Faz uso da função getaddrinfo, que oferece suporte tanto para IPv4 quanto para IPv6.

### Pré-requisitos
Para compilar e executar esta ferramenta, você precisará de:

- Sistema operacional Linux (Kali, Parrot, Ubuntu, etc.)
- O compilador GCC ou similar.

#### Compilação
- O script deve ser compilado com a flag `-lpthread` para linkar a biblioteca POSIX Threads. No terminal, execute o seguinte comando:

```yaml
gcc dnsBrute_linux.c -o dnsBrute_linux -lpthread
```
Ou com o nome do script que preferir nomear, após o comando `-o`

Isso irá gerar um arquivo executável, por exemplo chamado `dnsBrute_linux`.

### Modo de Uso:
A ferramenta requer 3 argumentos para funcionar:
```yaml
./dnsBrute_linux <DOMINIO> <WORDLIST> <ARQUIVO_SAIDA>
```

### Argumentos:

`<DOMINIO>`: O domínio alvo que será investigado (ex: exemplo.com).

`<WORDLIST>`: O caminho para o arquivo de texto contendo os subdomínios a serem testados, um por linha.

`<ARQUIVO_SAIDA>`: O nome do arquivo onde os resultados (subdomínios e IPs encontrados) serão salvos.

#### Exemplo prático:

Suponha que você tenha uma lista de subdomínios em um arquivo *wordlist.txt*. Para testá-los no domínio github.com e salvar os resultados em *resultados.txt*, o comando seria:

```yaml
./dnsBrute_linux github.com wordlist.txt resultados.txt
```
O terminal exibirá o progresso em tempo real:

```yaml
Iniciando busca por subdomínios... Resultados em 'resultados.txt'
Encontrado: www.github.com -> 140.82.121.4
Encontrado: api.github.com -> 140.82.121.6
...
[+] Busca finalizada. [+]
```
O arquivo resultados.txt conterá a lista final dos subdomínios válidos e seus respectivos IPs.
<hr>
