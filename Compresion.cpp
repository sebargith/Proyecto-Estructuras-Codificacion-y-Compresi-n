#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>
#include <algorithm>

// Estructura para representar un par de LZ
struct ParLZ {
    uint16_t pos;
    uint16_t lon;
    char sigchar;

    ParLZ(uint16_t p, uint16_t l, char c) : pos(p), lon(l), sigchar(c) {}
};

// Función para comprimir el texto usando LZ
std::vector<ParLZ> CompresionLZ(const std::string& texto) {
    std::vector<ParLZ> comprimido;
    int n = texto.size();
    for (int i = 0; i < n;) {
        int maxLon = 0;
        int maxPos = 0;
        for (int j = std::max(0, i - 8192); j < i; ++j) {
            int largo = 0;
            while (i + largo < n && texto[j + largo] == texto[i + largo] && largo < std::numeric_limits<uint16_t>::max()) {
                largo++;
            }
            if (largo > maxLon) {
                maxLon = largo;
                maxPos = j;
            }
        }
        char sigchar = (i + maxLon < n) ? texto[i + maxLon] : '\0';
        comprimido.emplace_back(i - maxPos, maxLon, sigchar);
        i += maxLon + 1;
    }
    return comprimido;
}

// Función para descomprimir el texto usando LZ
std::string DescompresionLZ(const std::vector<ParLZ>& comprimido) {
    std::string descomprimido;
    for (const auto& par : comprimido) {
        if (par.lon > 0) {
            int pos = descomprimido.size() - par.pos;
            for (int i = 0; i < par.lon; ++i) {
                descomprimido += descomprimido[pos + i];
            }
        }
        if (par.sigchar != '\0') {
            descomprimido += par.sigchar;
        }
    }
    return descomprimido;
}

// Función para leer el archivo de entrada
std::string LeerArchivo(const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        exit(1);
    }
    std::string contenido((std::istreambuf_iterator<char>(archivo)), std::istreambuf_iterator<char>());
    archivo.close();
    return contenido;
}

// Función para escribir el archivo comprimido en formato binario
void EscribirArchivoComprimido(const std::string& nombreArchivo, const std::vector<ParLZ>& comprimido) {
    std::ofstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo escribir el archivo: " << nombreArchivo << std::endl;
        exit(1);
    }
    for (const auto& par : comprimido) {
        archivo.write(reinterpret_cast<const char*>(&par.pos), sizeof(par.pos));
        archivo.write(reinterpret_cast<const char*>(&par.lon), sizeof(par.lon));
        archivo.write(&par.sigchar, sizeof(par.sigchar));
    }
    archivo.close();
}

// Función para leer el archivo comprimido en formato binario
std::vector<ParLZ> LeerArchivoComprimido(const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo comprimido: " << nombreArchivo << std::endl;
        exit(1);
    }
    std::vector<ParLZ> comprimido;
    while (archivo.peek() != EOF) {
        uint16_t pos, lon;
        char sigchar;
        archivo.read(reinterpret_cast<char*>(&pos), sizeof(pos));
        archivo.read(reinterpret_cast<char*>(&lon), sizeof(lon));
        archivo.read(&sigchar, sizeof(sigchar));
        if (archivo) {
            comprimido.emplace_back(pos, lon, sigchar);
        }
    }
    archivo.close();
    return comprimido;
}

// Función para escribir el archivo de texto descomprimido
void EscribirArchivoTexto(const std::string& nombreArchivo, const std::string& contenido) {
    std::ofstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo escribir el archivo: " << nombreArchivo << std::endl;
        exit(1);
    }
    archivo.write(contenido.data(), contenido.size());
    archivo.close();
}

// Función para escribir los pares LZ en un archivo de texto
void EscribirParesLZ(const std::string& nombreArchivo, const std::vector<ParLZ>& comprimido) {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo escribir el archivo: " << nombreArchivo << std::endl;
        exit(1);
    }
    for (const auto& par : comprimido) {
        archivo << "Pos: " << par.pos << ", Lon: " << par.lon << ", SigChar: " << par.sigchar << "\n";
    }
    archivo.close();
}

int main() {
    std::string nombreArchivoEntrada = "english1MB.txt";
    std::string nombreArchivoSalida = "textoComprimido.bin";
    std::string nombreArchivoDescomprimido = "textoDescomprimido.txt";
    std::string nombreArchivoParesLZ = "paresLZ.txt";
    
    std::string texto = LeerArchivo(nombreArchivoEntrada);

    // Compresión LZ
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<ParLZ> ComprimidoLZ = CompresionLZ(texto);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> TiempoCompresionLZ = end - start;

    std::cout << "Tiempo de Compresión LZ: " << TiempoCompresionLZ.count() << " s\n";
    std::cout << "Tamaño de Archivo Original: " << texto.size() << " bytes\n";

    // Escribir el archivo comprimido
    EscribirArchivoComprimido(nombreArchivoSalida, ComprimidoLZ);
    std::ifstream archivo_comprimido(nombreArchivoSalida, std::ios::binary | std::ios::ate);
    std::cout << "Tamaño de Archivo Comprimido: " << archivo_comprimido.tellg() << " bytes\n";
    archivo_comprimido.close();

    // Escribir los pares LZ en un archivo de texto
    EscribirParesLZ(nombreArchivoParesLZ, ComprimidoLZ);

    // Leer archivo comprimido y descomprimir
    std::vector<ParLZ> paresLZ = LeerArchivoComprimido(nombreArchivoSalida);

    start = std::chrono::high_resolution_clock::now();
    std::string DescomprimidoLZ = DescompresionLZ(paresLZ);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> TiempoDescompresionLZ = end - start;

    std::cout << "Tiempo de Descompresión LZ: " << TiempoDescompresionLZ.count() << " s\n";

    // Escribir archivo descomprimido
    EscribirArchivoTexto(nombreArchivoDescomprimido, DescomprimidoLZ);
    return 0;
}
