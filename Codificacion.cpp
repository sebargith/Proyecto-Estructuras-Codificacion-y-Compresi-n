#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <fstream>
#include <chrono>
#include <bitset>

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

// Función para escribir los bits como bytes en un archivo binario
void escribirBitsComoBytes(const string& bits, const string& nombreArchivo) {
    ofstream archivoBinario(nombreArchivo, ios::binary);
    if (!archivoBinario) {
        cerr << "No se pudo crear el archivo binario." << endl;
        return;
    }

    // Añadir ceros adicionales si no es múltiplo de 8
    string bitsModificados = bits;
    while (bitsModificados.size() % 8 != 0) {
        bitsModificados += '0';
    }

    // Convertir los bits a bytes y escribir en el archivo
    for (size_t i = 0; i < bitsModificados.size(); i += 8) {
        bitset<8> byte(bitsModificados.substr(i, 8));
        archivoBinario.put(static_cast<unsigned char>(byte.to_ulong()));
    }

    archivoBinario.close();
    cout << "Texto codificado guardado en '" << nombreArchivo << "'" << endl;
}

int main() {
    ifstream archivo("english1MB.txt");
    if (!archivo) {
        cerr << "No se pudo abrir el archivo." << endl;
        return 1;
    }

    string texto((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    archivo.close();

    // Calcular frecuencia de cada carácter en el texto
    unordered_map<char, int> frecuencias;
    for (char caracter : texto) {
        frecuencias[caracter]++;
    }

    // Construir el árbol Huffman
    NodoHuffman* raiz = construirArbolHuffman(frecuencias);

    // Generar los códigos de Huffman
    unordered_map<char, string> codigos;
    generarCodigosHuffman(raiz, "", codigos);

    // Imprimir los códigos de Huffman
    cout << "Códigos de Huffman:\n";
    for (auto par : codigos) {
        cout << par.first << ": " << par.second << endl;
    }

    // Medir el tiempo de codificación
    auto inicio = high_resolution_clock::now();
    string textoCodificado = codificarHuffman(texto, codigos);
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    // Escribir el texto codificado en un archivo binario
    escribirBitsComoBytes(textoCodificado, "textoCodificado.bin");

    // Decodificar el texto codificado
    string textoDecodificado = decodificarHuffman(textoCodificado, raiz);

    // Escribir el texto decodificado en un archivo
    ofstream archivoDecodificado("textoDecodificado.txt");
    if (archivoDecodificado) {
        archivoDecodificado << textoDecodificado;
        archivoDecodificado.close();
        cout << "Texto decodificado guardado en 'textoDecodificado.txt'" << endl;
    } else {
        cerr << "No se pudo crear el archivo de texto decodificado." << endl;
    }

    // Mostrar el tiempo de codificación
    cout << "Tiempo de codificación: " << duracion.count() << " ms" << endl;

    // Liberar la memoria del árbol Huffman
    liberarArbolHuffman(raiz);

    return 0;
}
