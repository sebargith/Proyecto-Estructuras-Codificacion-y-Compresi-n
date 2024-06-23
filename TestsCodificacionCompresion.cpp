#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// Estructura para representar un nodo en el árbol Huffman
struct NodoHuffman {
    char caracter;
    int frecuencia;
    NodoHuffman *izquierda, *derecha;

    NodoHuffman(char c, int freq) : caracter(c), frecuencia(freq), izquierda(nullptr), derecha(nullptr) {}
};

// Comparador para la cola de prioridad del árbol Huffman
struct Comparador {
    bool operator()(NodoHuffman* a, NodoHuffman* b) {
        return a->frecuencia > b->frecuencia;
    }
};

// Función para generar el árbol Huffman y retornar la raíz
NodoHuffman* construirArbolHuffman(unordered_map<char, int>& frecuencias) {
    priority_queue<NodoHuffman*, vector<NodoHuffman*>, Comparador> cola;

    // Crear un nodo para cada carácter con su frecuencia y agregarlo a la cola de prioridad
    for (auto par : frecuencias) {
        cola.push(new NodoHuffman(par.first, par.second));
    }

    // Construir el árbol Huffman
    while (cola.size() > 1) {
        NodoHuffman* izquierda = cola.top();
        cola.pop();
        NodoHuffman* derecha = cola.top();
        cola.pop();

        NodoHuffman* nuevoNodo = new NodoHuffman('$', izquierda->frecuencia + derecha->frecuencia);
        nuevoNodo->izquierda = izquierda;
        nuevoNodo->derecha = derecha;

        cola.push(nuevoNodo);
    }

    return cola.top();
}

// Función auxiliar para realizar el recorrido del árbol Huffman y obtener los códigos de cada carácter
void generarCodigosHuffman(NodoHuffman* raiz, string codigoActual, unordered_map<char, string>& codigos) {
    if (!raiz) return;

    if (raiz->caracter != '$') {
        codigos[raiz->caracter] = codigoActual;
    }

    generarCodigosHuffman(raiz->izquierda, codigoActual + "0", codigos);
    generarCodigosHuffman(raiz->derecha, codigoActual + "1", codigos);
}

// Función para codificar un texto usando los códigos de Huffman generados
string codificarHuffman(string texto, unordered_map<char, string>& codigos) {
    string textoCodificado;
    for (char caracter : texto) {
        textoCodificado += codigos[caracter];
    }
    return textoCodificado;
}

// Función para decodificar un string codificado usando el árbol de Huffman
string decodificarHuffman(const string& stringCodificado, NodoHuffman* raiz) {
    string stringDecodificado;
    NodoHuffman* nodoActual = raiz;
    
    for (char bit : stringCodificado) {
        if (bit == '0') {
            nodoActual = nodoActual->izquierda;
        } else {
            nodoActual = nodoActual->derecha;
        }
        
        // Si llegamos a un nodo hoja
        if (nodoActual->izquierda == nullptr && nodoActual->derecha == nullptr) {
            stringDecodificado += nodoActual->caracter;
            nodoActual = raiz;
        }
    }
    
    return stringDecodificado;
}

// Función para liberar la memoria del árbol Huffman
void liberarArbolHuffman(NodoHuffman* raiz) {
    if (!raiz) return;
    liberarArbolHuffman(raiz->izquierda);
    liberarArbolHuffman(raiz->derecha);
    delete raiz;
}

// Estructura para representar un par de LZ
struct ParLZ {
    uint16_t pos;
    uint16_t lon;
    char sigchar;

    ParLZ(uint16_t p, uint16_t l, char c) : pos(p), lon(l), sigchar(c) {}
};

// Función para comprimir el texto usando LZ
vector<ParLZ> CompresionLZ(const string& texto) {
    vector<ParLZ> comprimido;
    int n = texto.size();
    for (int i = 0; i < n;) {
        int maxLon = 0;
        int maxPos = 0;
        for (int j = max(0, i - 8192); j < i; ++j) {
            int largo = 0;
            while (i + largo < n && texto[j + largo] == texto[i + largo] && largo < numeric_limits<uint16_t>::max()) {
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
string DescompresionLZ(const vector<ParLZ>& comprimido) {
    string descomprimido;
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

void realizarPruebas(const string& archivoEntrada, ofstream& archivoResultados) {
    ifstream archivo(archivoEntrada);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo " << archivoEntrada << endl;
        return;
    }

    string texto((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    archivo.close();

    // Codificación Huffman
    unordered_map<char, int> frecuencias;
    for (char caracter : texto) {
        frecuencias[caracter]++;
    }

    NodoHuffman* raiz = construirArbolHuffman(frecuencias);

    unordered_map<char, string> codigos;
    generarCodigosHuffman(raiz, "", codigos);

    auto inicio = high_resolution_clock::now();
    string textoCodificado = codificarHuffman(texto, codigos);
    auto fin = high_resolution_clock::now();
    auto duracionCodificacion = duration_cast<milliseconds>(fin - inicio);

    inicio = high_resolution_clock::now();
    string textoDecodificado = decodificarHuffman(textoCodificado, raiz);
    fin = high_resolution_clock::now();
    auto duracionDecodificacion = duration_cast<milliseconds>(fin - inicio);

    liberarArbolHuffman(raiz);

    // Compresión LZ
    auto start = high_resolution_clock::now();
    vector<ParLZ> ComprimidoLZ = CompresionLZ(texto);
    auto end = high_resolution_clock::now();
    auto TiempoCompresionLZ = duration_cast<milliseconds>(end - start);

    start = high_resolution_clock::now();
    string DescomprimidoLZ = DescompresionLZ(ComprimidoLZ);
    end = high_resolution_clock::now();
    auto TiempoDescompresionLZ = duration_cast<milliseconds>(end - start);

    // Escribir resultados en archivo CSV
    archivoResultados << archivoEntrada << ";" << duracionCodificacion.count() << ";" << duracionDecodificacion.count() << ";"
                      << TiempoCompresionLZ.count() << ";" << TiempoDescompresionLZ.count() << "\n";
}

int main() {
    vector<string> archivosEntrada = {"english10kb.txt", "english1MB.txt", "english5MB.txt", "english10MB.txt", "english20MB.txt"};
    ofstream archivoResultados("resultados.csv");

    if (!archivoResultados) {
        cerr << "No se pudo crear el archivo de resultados." << endl;
        return 1;
    }

    archivoResultados << "Archivo de Entrada;Tiempo Codificación;Tiempo Decodificación;Tiempo Compresión;Tiempo Descompresión\n";

    for (int i = 0; i < 10; ++i) {
        for (const auto& archivo : archivosEntrada) {
            realizarPruebas(archivo, archivoResultados);
        }
    }

    archivoResultados.close();

    return 0;
}
