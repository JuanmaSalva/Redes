#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <iostream>


int main(int argc, char** argv) //argv[1] direccion, argv[2] protocolo
{
    struct addrinfo hintsInfo;
    struct addrinfo * resInfo;

    memset((void*) &hintsInfo, 0, sizeof(struct addrinfo));

    hintsInfo.ai_family = AF_UNSPEC; //le decimos que tiene que ser ipv4
    hintsInfo.ai_socktype = SOCK_STREAM; //tcp

    //hacemos la llamada por red
    int info = getaddrinfo(argv[1], argv[2], &hintsInfo, &resInfo);

    if(info != 0)
    { //error
        std::cout << "Se ha producido un error\n";
        return -1;
    }

    //recorremos todas las conexiones que hemos detectado
    for(struct addrinfo* i = resInfo; i != NULL; i = i->ai_next){
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen,
                       host, NI_MAXHOST,
                       service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Host: " << host << " Port: " << service << "\n";
    }

    freeaddrinfo(resInfo);

    return 0; 
}