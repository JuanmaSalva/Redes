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

    hintsInfo.ai_family = AF_UNSPEC; //cualquier tipo de ip (Ipv4 o IPv6)
    hintsInfo.ai_socktype = 0; //todos los protocolos

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

        std::cout << host << "  " << service << " " << i->ai_family << " " << i->ai_socktype << "\n";
    }

    freeaddrinfo(resInfo);

    return 0; 
}