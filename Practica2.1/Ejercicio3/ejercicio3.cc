#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>


int main(int argc, char** argv) //argv[1] direccion, argv[2] puerti, argv[3] comando
{
    struct addrinfo hintsInfo;
    struct addrinfo * resInfo, *rp;

    memset((void*) &hintsInfo, 0, sizeof(struct addrinfo));

    hintsInfo.ai_family = AF_INET; //le decimos que tiene que ser ipv4
    hintsInfo.ai_socktype = SOCK_DGRAM; //todos los protocolos

    //hacemos la llamada por red
    int info = getaddrinfo(argv[1], argv[2], &hintsInfo, &resInfo);

    if(info != 0)
    { //error
        std::cout << "Se ha producido un error\n";
        return -1;
    }

    int socket_;
    //de toda la indormacion que nos da getaddrinfo, buscamos cual es la que queremos
    for(rp = resInfo; rp != NULL; rp = rp->ai_next){
        socket_ = socket(rp->ai_family, rp->ai_socktype, 0); //creamos un socket
        
        if(socket_ == -1) 
            continue;
        if(connect(socket_, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(socket_); //no deberiamos llegar aquí
    }
    
    if(rp == NULL){
        std::cout << "No se ha podido conectar al socket\n";
        return -1;
    }

    freeaddrinfo(resInfo);

    bind(socket_, resInfo->ai_addr, resInfo->ai_addrlen);

    char buffer[50];
    struct sockaddr client;
    socklen_t client_len = sizeof(struct sockaddr);

    //sendto
    sendto(socket_, argv[3], sizeof(argv[3]),0, NULL, 0);
    //write(socket_, argv[3], sizeof(argv[3]));

    //recivefrom
    int bytes = recvfrom(socket_, buffer, sizeof(buffer), 0, &client, &client_len);
    //int bytes = read(socket_, buffer, 50);

    if(bytes == -1){
        std::cout << "Se ha producido un error al recibir los datos\n";
        return -1;
    }

    std::cout << buffer << "\n";

    return 0; 
}