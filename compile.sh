if [ "$1" == "buddy" ]; then
    MM="buddy"
else
    MM="naive"
fi

docker run -d -v "${PWD}:/root" --privileged -ti --name TPECOMPILER agodio/itba-so-multi-platform:3.0
docker start TPECOMPILER
docker exec -it TPECOMPILER make clean -C/root/Toolchain
docker exec -it TPECOMPILER make clean -C/root/
docker exec -it TPECOMPILER make all -C /root/Toolchain MEM_IMPL="$MM"
docker exec -it TPECOMPILER make all -C /root/ MEM_IMPL="$MM"
docker stop TPECOMPILER 
docker rm TPECOMPILER 

