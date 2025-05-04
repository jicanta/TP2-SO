#!/bin/bash

if [ -z "$CONTAINER_NAME" ]; then
  echo "Error: CONTAINER_NAME environment variable is not set."
  echo "Please set the CONTAINER_NAME variable and try again."
  exit 1
fi

docker start $CONTAINER_NAME
docker exec -it $CONTAINER_NAME make clean -C /root/Toolchain
docker exec -it $CONTAINER_NAME make clean -C /root/
docker exec -it $CONTAINER_NAME make -C /root/Toolchain
docker exec -it $CONTAINER_NAME make -C /root/