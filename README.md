# PingPongOS

## Sobre o projeto
Desenvolvimento do projeto PingPongOS, feito pelo professor Maziero da UFPR.

Link do projeto: https://wiki.inf.ufpr.br/maziero/doku.php?id=so:pingpongos

Além disso, acompanhei e me baseei no desenvolvimento desse projeto na série do Waine Barbosa. 
Link da playlist: https://www.youtube.com/playlist?list=PL2WQTg3Tx5wPOXFxgGCgl34IbXvlYImVg

## Conteúdo
Os seguintes conteúdos já foram desenvolvidos para o projeto:
- [X] Biblioteca de filas (warm-up)
- [X] Trocas de contexto
- [X] Gestão de tarefas
- [X] Dispatcher
- [X] Escalonador por prioridades
- [ ] Preempção por tempo
- [ ] Contabilização
- [ ] Tarefa main
- [ ] Tarefas suspensas
- [ ] Tarefas dormindo
- [ ] Semáforos
- [ ] Uso de semáforos
- [ ] Operador Barreira
- [ ] Filas de mensagens
- [ ] Gerente de disco
- [ ] Escalonamento de disco
- [ ] Sistema de arquivos
- [ ] Alocação de memória
- [ ] Gestão de terminal
- [ ] Interface de rede
- [ ] Execução em um hardware real

## Estrutura técnica do projeto
Os arquivos seguem a seguinte estrutura:
![img.png](https://wiki.inf.ufpr.br/maziero/lib/exe/fetch.php?media=so:ppos.png)

### Estrutura de pastas do projeto
- `./include`: contém arquivos .h fornecendo as interfaces das bibliotecas;
- `./src`: contém a implementação das interfaces do projeto e o arquivo `main.c`;
- `./tests`: contém testes básicos para as interfaces;
- `./docs`: documentações em geral (em desuso);

### Como rodar o projeto
O projeto deve ser compilado via `cmake --build`. O projeto deve rodar em ambiente que suporte POSIX*. 

\* Futuramente o projeto contará com sua própria interface de syscalls, mas por enquanto estou utilizando POSIX.

### Como rodar testes
1. Ajuste o CMakeLists para apontar para o arquivo de teste desejado;
2. No arquivo .c de teste que você deseja rodar, retire o prefixo `test_` da função main do arquivo;
3. Compile com `cmake --build` e execute.

### Depuração de memória utilizando valgrind
*Work in progress.*
