#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {        
        strncpy(name, _n, 80);
        _size = (80 * sizeof(char)) +  (2 * sizeof(int16_t));
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //primero reservamos espacio suficiente para todos los datos
        allocData();

        std::cout << _size << "\n";
        //se crea una copia para poderlo avanzar
        char* aux = _data;

        //en _data tenemos que meter todo los datos de la clase
        memcpy(aux, name, 80 * sizeof(char));

        //para no sobre-escribir lo que ya tenemos, avanzamos 
        aux += (80 * sizeof(char));

        //copiamos la x y la y
        memcpy(aux,&x, sizeof(int16_t));
        aux += sizeof(int16_t);

        memcpy(aux, &y, sizeof(int16_t));
        aux += sizeof(int16_t);
    }

    int from_bin(char * data)
    {
        //en data tenemos todo lo necesario para cargar la informacion
        memcpy(name, data, 80 * sizeof(char));
        data += 80 * sizeof(char);

        memcpy(&x, data, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(&y, data, sizeof(int16_t));
        data+= sizeof(int16_t);

        return 0;
    }

    char* getData(){
        return _data;
    }

    int32_t getMaxSize(){
        return _size;
    }

    void printVal(){
        std::cout << "Name: " << name << "\n";
        std::cout << "X: " << x << "\n";
        std::cout << "Y: " << y << "\n";
    }

public:
    char name[80];

    int16_t x;
    int16_t y;

protected:
    int32_t _size; //el tamaño total de datos que vamos a guardar en binario
    char* _data; //lo datos que vamos a guardar en binario

    //reserverá suficiente espacio en _data pera guardar toda la info de la clase
    void allocData(){
        
        if(_data != 0) //primero miramos si ya hay algo guardado
            free(_data); //si teniamos algo guardado lo eliminamos

        //reservamos el espacio necesario
        _data = (char *)malloc(_size);
    }
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    std::cout << "Hemos nada\n";
    one_w.to_bin();
    std::cout << "Hemos otro\n";

    // 2. Escribir la serialización en un fichero

    //abrimos el archivo
    int file = open("./jugador", O_CREAT | O_TRUNC | O_RDWR);
    //escribimos los datos
    char* data = one_w.getData();
    std::cout << sizeof(data) << "\n";
    write(file, data, sizeof(data));

    //cerramos el archivo
    if(close(file) != 0)
        std::cout << "Se ha producido un error al cerrar el fichero\n";

    std::cout << "Hemos escrito\n";
        
    // 3. Leer el fichero
    int inFile = open("./jugador", O_TRUNC | O_RDONLY);

    char* inData;
    read(inFile, inData, one_r.getMaxSize());
    //cerramos el archivo
    if(close(inFile) != 0)
            std::cout << "Se ha producido un error al cerrar el fichero\n";

    
    std::cout << "Hemos leido\n";
    // 4. "Deserializar" en one_r
    one_r.from_bin(inData);

    // 5. Mostrar el contenido de one_r
    one_r.printVal();

    return 0;
}

