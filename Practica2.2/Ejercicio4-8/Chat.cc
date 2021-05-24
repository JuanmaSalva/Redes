#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* aux = _data;

    //guardamos el tipo
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    //guardamos el nombre
    memcpy(aux, nick.c_str(), sizeof(char) * 8);
    aux += sizeof(char) * 8;

    //guardamos el mensaje
    memcpy(aux, message.c_str(), sizeof(char) * 80);

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    char* nickChar = (char*)malloc(sizeof(char) * 8);
    memcpy(nickChar, _data, sizeof(char) * 8);
    nick = std::string(nickChar);
    aux += sizeof(char) * 8;

    char* msgChar = (char*)malloc(sizeof(char) * 80);
    memcpy(msgChar, _data, sizeof(char) * 80);
    message = std::string(msgChar);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje   
        std::cout << "Abrimos server\n";
        ChatMessage msg;
        socket.recv(msg);

        switch(msg.type){
            case 0:
            std::cout << "LOGIN\n";
            break;
            case 1:
            std::cout << "LOGOUT\n";
            break;
            case 2:
            std::cout << "MESSAGE\n";            
            break;
        }

        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::cout << "LOGIN cliente\n";
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    //std::cout << "se va a enviar los datos\n";
    socket.send(em, socket);
    //std::cout << "se han enviado los datos\n";
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string msg;
        std::getline (std::cin,msg);

        // Enviar al servidor usando socket
        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        ChatMessage msg;
        socket.recv(msg);
        
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << msg.nick << ": " << msg.message << "\n";
    }
}

