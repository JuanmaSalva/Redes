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

    sa = *resInfo->ai_addr;
    sa_len = resInfo->ai_addrlen;
}

Socket::Socket(struct sockaddr * _sa, socklen_t _sa_len) : sd(-1), sa(*_sa),sa_len(_sa_len){
	//Creamos el constructor para poder enviar mensajes a los clientes
	sd = socket(AF_INET,SOCK_DGRAM,0);
	bind();
};

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
    obj.to_bin();

    //Enviar el objeto binario a sock usando el socket sd
    sendto(sock.sd, obj.data(), obj.size(), 0, &sa, sa_len);
}

bool operator== (const Socket &s1, const Socket &s2)
{    
    //en memoria un sockaddr_in es lo mismo que un sockaddr
    const struct sockaddr_in* a = (const struct sockaddr_in*)&s1.sa;
    const struct sockaddr_in* b = (const struct sockaddr_in*)&s2.sa;

    //Complos campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar falarar se si alguno difiere
    if(a->sin_family == b->sin_family && a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port)
     return true;
    else return false;
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

