#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

 #include <time.h>
 

int resp(const char *comando, int socket_, sockaddr client, socklen_t client_len){
    char bufferOut[50] = {};
    time_t t;
    struct tm * timeInfo;

    t = time(NULL);
    timeInfo = localtime(&t);

    //const char *aux = "%T";
    strftime(bufferOut, sizeof(bufferOut), comando, timeInfo);


    std::cout << "Respuesta mandada\n";
    return sendto(socket_, bufferOut, sizeof(bufferOut), 0, &client, client_len);
}

int conexionTerminada(int socket_, sockaddr client, socklen_t client_len){
    const char msg[50] = {"Server cerrado"};

    return sendto(socket_, msg, sizeof(msg), 0, &client, client_len);
}

int main(int argc, char** argv) //argv[1] direccion donde escucha, argv[2] puerto
{
    struct addrinfo hintsInfo;
    struct addrinfo * resInfo;

    memset((void*) &hintsInfo, 0, sizeof(struct addrinfo));

    hintsInfo.ai_family = AF_INET; //le decimos que tiene que ser ipv4
    hintsInfo.ai_socktype = SOCK_STREAM; //tcp

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



    listen(socket_, 16); //16 concexiones posibles

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];


    struct sockaddr client;
    socklen_t client_len = sizeof(struct sockaddr);
    int client_socket = accept(socket_, &client, &client_len); //este es el socket q específico a esa conexión

    getnameinfo(&client, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    std::cout << "Conexión desde: " << host << ":" << service << "\n";

    
    bool serverEncendido = true;
    while(serverEncendido){
        char buffer[50];

        //cargamos el mensaje guardando la cantidad de bytes que se han mandado
        //std::cout << "Esperando\n";
        int bytes = recv(client_socket, buffer, 50, 0);

        if(bytes == 0){ //el cliente ha terminado la conexión
            serverEncendido = false; //en verdad esto no hace falta
            std::cout << "Servidor cerrado\n";
            break;
        }

        if(bytes == -1){
            std::cout << "Se ha producido un error al recibir el mensaje\n";
            return -1;
        }
        std::cout << "Mensaje recibido\n";
        
        send(client_socket, buffer, bytes, 0);
    }

    close(socket_);

    return 0; 
}