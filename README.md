# Exercício Programa 1 - MAC0336

##### Hélio Hideki Assakura Moreira - 8941064

___

A pasta do EP contém os arquivos:

* **structs.h**
  Arquivo header contendo as estruturas usadas no programa

* **utils.c**
  Contém as funções "utilitárias" usadas no programa, como rotação de bits, conversão de tipos e leitura e escrita de arquivos

* **K128.c**
  Programa principal do Exercício Programa. Contém as funções necessárias para criptografar arquivos usando a criptografia K128 e métodos usados no cálculo da entropia

* **utils.h / K128.h**
  Arquivos header de utils e K128

* **sbox1..4**
  Sboxes listadas no enunciado do programa, usadas no algoritmo K128

* **arquivos teste**
  Foram usados 2 arquivos para testes, uma imagem e um texto

* **main.c**
  Arquivo usado para organizar as opções do usuário, conforme descrito no enunciado

* **main.exe**
  Arquivo executável do projeto

* **Makefile**

### Uso do programa

Para usar o programa:
1. digite **make**
2. use um dos comandos listados abaixo:
  * **Para criptografar arquivos:** 
    ./main -c -i <arquivo de entrada> -o <arquivo de saída> -p <senha> -a
  * **Para decriptografar arquivos:**
    ./main -d -i <arquivo de entrada> -o <arquivo de saída> -p <senha>
  * **Para calcular aleatoriedade pelo método 1 (conforme descrito no enunciado):**
    ./main -1 -i <arquivo de entrada> -p <senha>
  * **Para calcular aleatoriedade pelo método 2  (conforme descrito no enunciado):**
    ./main -2 -i <arquivo de entrada> -p <senha>

