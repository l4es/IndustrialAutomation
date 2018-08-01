#!/bin/bash

# this script is given as an exemple
# I use it to prepare a ready-to-build container
# and then use that container with enter_docker.sh

docker rm current
docker rmi beremiz_builder
docker build --build-arg UID=$(id -u) --build-arg GID=$(id -g) -t beremiz_builder .
docker create \
       --name current \
       -v ~/src:/home/devel/src \
       -v ~/build/:/home/devel/build \
       -v ~/.bash_history:/home/devel/.bash_history \
       -w /home/devel/build \
       -i -t beremiz_builder /bin/bash
