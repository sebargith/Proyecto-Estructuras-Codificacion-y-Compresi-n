#include <iostream>
#include <string>
#include <vector>

// Estructura para representar un par de LZ
struct ParLZ {
    int pos;
    int lon;
    char sigchar;

    ParLZ(int p, int l, char c) : pos(p), lon(l), sigchar(c) {}
};

// Declaración de funciones iniciales
std::vector<ParLZ> CompresionLZ(const std::string& texto);
std::string DescompresionLZ(const std::vector<ParLZ>& comprimido);





// Función para comprimir el texto usando LZ
std::vector<ParLZ> CompresionLZ() {

    return comprimido;
}




// Función para descomprimir el texto usando LZ
std::string DescompresionLZ() {

    return descomprimido;
}







int main() {


    return 0;
}
