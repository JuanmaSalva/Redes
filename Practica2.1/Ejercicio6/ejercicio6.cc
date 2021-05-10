#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread> 

#include "mensaje.cc"

#define MAX_THREADS 5

int main(int argc, char** argv) //argv[1] direccion donde escucha, argv[2] puerto
{
    struct addrinfo hintsInfo;
    struct addrinfo * resInfo;

    memset((void*) &hintsInfo, 0, sizeof(struct addrinfo));

    hintsInfo.ai_family = AF_INET; //le decimos que tiene que ser ipv4
    hintsInfo.ai_socktype = SOCK_DGRAM; //udp

    //hacemos la llamada por red
    int info = getaddrinfo(argv[1], argv[2], &hintsInfo, &resInfo);

    if(info != 0)
    { //error
        std::cout << "Se ha producido un error\n";
        return -1;
    }
    
    //en res[0] tenemos la direccion traducida a binario y el puerto    
    int socket_ = socket(resInfo->ai_family, resInfo->ai_socktype, 0); //creamos un socket
    if(socket_ == -1){
        std::cout << "Se ha producido un error al crear el socket\n";
        return -1;
    }

    //hacemos el bind para asignarle una direccion al socket que acabamos de crear
    bind(socket_, resInfo->ai_addr, resInfo->ai_addrlen);

    freeaddrinfo(resInfo);

    //tratamiento de hilos
    std::thread pool[MAX_THREADS];

    for(int i=0; i< MAX_THREADS ;i++){        
        pool[i] = std::thread([&](){
            Mensaje msg = Mensaje(socket_);
        }); //despues de la , tienen que ir los parámetros
    }

    for(int i=0;i<5;i++){
        pool[i].join();
    }

    close(socket_);

    return 0; 
}