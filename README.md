# SD_Proj02

# Autores
- Jiarong Li       fc58177
- Xiaoji Wu        fc59800
- Jiaxin Huang     fc60808

# Project Overview
Este projeto implementa um sistema cliente-servidor em C para gerenciamento de uma hash table, que armazena pares chave-valor e é acessível através de sockets TCP. O servidor mantém a estrutura de dados hash table em memória, enquanto o cliente se comunica com ele para realizar operações. Para a serialização e de-serialização dos dados trocados entre cliente e servidor, foi utilizado o Protocol Buffers, substituindo a abordagem de serialização manual adotada na primeira fase do projeto.

O objetivo principal é permitir que o cliente realize operações na tabela hash armazenada no servidor, como inserção, remoção, consulta e listagem de chaves.

# Estrutura do Projeto

- include/: Contém ficheiros de cabeçalho (.h)
- source/: Contém ficheiros de código fonte (.c)
- object/: Diretório para ficheiros de objetos
- binary/: Diretório para ficheiros executáveis
- dependencies/: Diretório para ficheiros de dependências necessários para a compilação e execução do projeto.
- proto/: Contém os ficheiros de definição do Protocol Buffers (.proto) usados para serialização dos dados.


# Detalhes sobre a execução do programa
Neste projeto 2, conseguimos compilar o código sem nenhum problema. 
 No entanto, 
 em algumas funcionalidades e respostas do sistema, 
 ainda restaram alguns problemas que não foram totalmente resolvidos. 
 A seguir, 
 explicaremos em detalhes como o nosso projeto funciona, 
 descrevendo o processo de execução, as operações implementadas,
 e as áreas onde identificamos falhas ou comportamentos inesperados.


Projeto 2:
0º passo: change directory
    cd ----/sd_projeto2

1º passo: Makefile
    (comando no terminal)
    make clean
    make all
    (nota: 
          No nosso projeto, utilizamos as seguintes pastas organizadas no Makefile:
            BIN_DIR = binary
            INC_DIR = include
            OBJ_DIR = object
            SRC_DIR = source
            LIB_DIR = library
            DEP_DIR = dependencies
          Se algum desses diretórios estiver faltando, 
          a compilação poderá ser interrompida. 
          Neste caso, por favor, 
          crie os diretórios manualmente antes de compilar o projeto.)

2º passo: terminal, executar
    abre 2 (mais uma) janelas de terminal , janela 1 para servidor, 2 para cliente
    Janela 1: (exemplo)
        ./binary/server_hashtable 12345 111
    Janela 2: (exemplo)
        ./binary/client_hashtable 10.0.2.15:12345
        OR
        ./binary/client_hashtable 127.0.0.1:12345

3º passo: hashtable content intodução:   
    No server_hashtable.exe, 
        No início, Output "Server ready, waiting for connections"
        Quando cliente ligar o socket, Output "Client connection established"
        Quando client quit (desligar o socket), Output "Client connection closed"
        E espera a outra ligação com Output "Server ready, waiting for connections"
        Para sair o server_hashtable, usa "Ctrl+c"
    No client_hashtable.exe,
        Há 7 operações:
        p: p[ut] <key> <value> : está tudo bem
        g: g[et] <key> : vai dar um erro e occore hashtable deixa de funcionar 
        d: d[el] <key> : está tudo bem
        s: s[ize] : está tudo bem
        k: k[eys] : está tudo bem
        t: t[able] : vai dar um erro e occore hashtable deixa de funcionar
        q: q[uit] : está tudo bem
        Logo, temos 2 falhas, e vou apresentar 
                    um pedaço de Output do terminal 
                    para as operações que estão tudo bem

4º passo: Output das operações no terminal
------------------------------------------------
------------------------------------------------
    (Janela do client_hashtable)
    ./binary/client_hashtable 127.0.0.1:12345
    Command: a
    Invalid command.
    Usage: p[ut] <key> <value> | g[et] <key> | d[el] <key> | s[ize] | k[eys] | t[able] | q[uit]
    Command: s
    Table size: 0
    Command: p 123 234
    123 :: 234
    Command: s
    Table size: 1
    Command: p 56 789
    56 :: 789
    Command: k
    123
    56
    Command: d 123
    Entry removed
    Command: s   
    Table size: 1
    Command: k
    56
    Command: q
    Bye, bye!

## Notas de Implementação
- A comunicação cliente-servidor é feita utilizando sockets TCP.
- Utilizamos Protocol Buffers para serializar e desserializar mensagens trocadas entre o cliente e o servidor.
- O gerenciamento de memória é cuidadosamente tratado para evitar vazamentos.
- O sistema atual permite apenas uma conexão de cliente por vez. Suporte para múltiplos clientes será adicionado em fases futuras do projeto.

## Limitações Conhecidas
- O sistema não possui suporte para concorrência, limitando a utilização a um cliente por vez.


