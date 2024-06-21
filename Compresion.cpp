#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm> // Para std::max

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
std::vector<ParLZ> CompresionLZ(const std::string& texto) {
    std::vector<ParLZ> comprimido;
    int n = texto.size();
    for (int i = 0; i < n;) {
        int maxLon = 0;
        int maxPos = 0;
        for (int j = std::max(0, i - 255); j < i; ++j) {
            int largo = 0;
            while (i + largo < n && texto[j + largo] == texto[i + largo]) {
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
            if (pos < 0) pos = 0; // Asegurarse de que la posición no sea negativa
            descomprimido += descomprimido.substr(pos, par.lon);
        }
        if (par.sigchar != '\0') {
            descomprimido += par.sigchar;
        }
    }
    return descomprimido;
}

int main() {
    SetConsoleOutputCP(CP_UTF8); // Carácteres en español

    // Leer el archivo entrada.txt
    std::ifstream file("entradaprueba.txt");
    if (!file) {
        std::cerr << "No se pudo abrir el archivo entrada.txt\n";
        return 1;
    }

    std::string dataset((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Compresión LZ
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<ParLZ> ComprimidoLZ = CompresionLZ(dataset);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> TiempoCompresionLZ = end - start;

    start = std::chrono::high_resolution_clock::now();
    std::string DescomprimidoLZ = DescompresionLZ(ComprimidoLZ);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> TiempoDescompresionLZ = end - start;

    std::cout << "Tiempo de Compresión LZ: " << TiempoCompresionLZ.count() << " s\n";
    std::cout << "Tiempo de Descompresión LZ: " << TiempoDescompresionLZ.count() << " s\n";
    std::cout << "Tamaño de Compresión LZ: " << ComprimidoLZ.size() * sizeof(ParLZ) * 8 << " bits\n";

    return 0;
}
