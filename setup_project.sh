#!/usr/bin/bash

if [ $# -ne 1 ]; then
    echo "Uso: ./setup_project.sh <numero-do-projeto>"
    exit 1
fi

if echo $1 | egrep -q '^[1-1]+$'; then
    echo "Acessando o projeto $1..."
else
    echo "Parametro incorreto. Use como argumento apenas o numero do projeto que deseja executar."
    exit 2
fi


ROOT_PROJECT_FOLDER=$(pwd)

cd ./projeto$1/

echo "Executando o CMake..."

if [ -d "build/" ]; then
    rm -rf build
fi

mkdir build
cd ./build
cmake ..
cmake --build .

echo ""
echo "Copiando os executaveis na raiz do projeto para maior conveniencia..."
cp ./bin/* $ROOT_PROJECT_FOLDER

echo "Verificando os containers docker..."
cd $ROOT_PROJECT_FOLDER/docker

if [ "$(docker inspect -f {{.State.Running}} lab_redes_mongodb_1)" == "true" ]; then
    echo "O container docker do mongodb ja esta executando."
else
    echo "Preparando para iniciar os containers do mongodb..."
    $(docker-compose -p lab_redes up -d)
fi

echo ""
echo "Finalizado! Nao se esqueca de encerrar os containers apos o uso!"
exit 0
