class Mensaje{
public:
    Mensaje(int socket_){
        std::cout << "Thread: " << std::this_thread::get_id() << " escuchando\n";
        char buffer[50];

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        struct sockaddr client;
        socklen_t client_len = sizeof(struct sockaddr);

        getnameinfo(&client, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        //std::cout << "Conexión desde: " << host << ":" << service << "\n";

        while(true){
            //cargamos el mensaje guardando la cantidad de bytes que se han mandado
            //std::cout << "Esperando\n";
            int bytes = recvfrom(socket_, buffer, 50, 0, &client, &client_len);
            std::cout << "El thread: " << std::this_thread::get_id() << " está en uso\n";

            if(bytes == -1){
                std::cout << "Se ha producido un error al recibir el mensaje\n";
            }

            //std::cout << bytes << " bytes de " << client << "\n";
            //std::cout << "Comando: " << buffer[0] << "\n";
            ;

            int bytesSend = 0;

            switch(buffer[0]){
                    case 't':
                    bytesSend = resp("%T", socket_, client, client_len);                
                break;
                    case 'd':
                    bytesSend = resp("%D", socket_, client, client_len); 
                break;
                    case 'q':
                    //serverEncendido = false;
                    bytesSend = conexionTerminada(socket_, client, client_len);
                break;
                    default:
                    std::cout << "Comando incorrecto\n";
                break;
            }

            if(bytesSend == -1){
                std::cout << "Se ha producido un error al enviar la respuesta\n";
            }
            sleep(5);
            std::cout << "El thread: " << std::this_thread::get_id() << " está libre\n";
        }
    }

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
};