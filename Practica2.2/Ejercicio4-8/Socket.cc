#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{ 
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo   
    struct addrinfo hintsInfo;
    struct addrinfo * resInfo;

    memset((void*) &hintsInfo, 0, sizeof(struct addrinfo));

    hintsInfo.ai_family = AF_INET; //le decimos que tiene que ser ipv4
    hintsInfo.ai_socktype = SOCK_DGRAM; //udp
    
    //hacemos la llamada por red
    int info = getaddrinfo(address, port, &hintsInfo, &resInfo);

    if(info != 0)
    { //error
        std::cout << "Se ha producido un error en el getaddrinfo\n";
        return;
    }

    //Con el resultado inicializar sd
    sd = socket(resInfo->ai_family, resInfo->ai_socktype, 0); //creamos un socket
    if(sd == -1){
        std::cout << "Se ha producido un error al crear el socket\n";
        return;
    }

    //inicializamos sa y sa_len 
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    sa_len = sizeof(struct sockaddr);
    getnameinfo(&sa, sa_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    //Enviar el objeto binario a sock usando el socket sd
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

