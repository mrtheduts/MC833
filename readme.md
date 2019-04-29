# MC833
Aqui se encontram os projetos desenvolvidos na disciplina [MC833 - Programação em Redes de Computadores](http://www.ic.unicamp.br/~edmundo/MC833/), ministrada pelo [Prof. Dr. Edmundo Roberto Mauro
Madeira](http://www.ic.unicamp.br/~edmundo/)

## Pré-requisitos
Para executar os projetos que aqui se encontram, são necessários os seguintes programas:
- [CMake](https://cmake.org/)
- [Docker Compose](https://docs.docker.com/compose/install/)
- [MongoDB C Driver](http://mongoc.org/)

## Execução
Há, na pasta raiz, o arquivo `setup_project.sh`, usado para automatizar a execução do projeto desejado. Eis os passos necessários para sua execução:
```
$ chmod a+x setup_project.sh
$ ./setup_project.sh <numero-do-projeto>
```
Após a execução, os binários do servidor e do cliente (`servidor` e `cliente`, respectivamente) se encontrarão na pasta raiz do projeto e podem ser executados localmente da seguinte maneira:

`$ ./servidor`

`$ ./cliente localhost`

## Parando os Containers
Para interromper a execução dos containers e apagar as imagens gerada pelo `docker-compose`, basta executar:
```
$ docker container stop lab_redes_mongodb_1
$ docker container rm lab_redes_mongodb_1
```

## Todo
- [X] Terminar o Readme
- [ ] Fazer o Projeto 1
- [ ] Fazer o Projeto 2
- [ ] Fazer o Projeto 3
