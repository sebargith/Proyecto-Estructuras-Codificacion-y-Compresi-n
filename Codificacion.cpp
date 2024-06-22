#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>

using namespace std;

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
std::string decodificarHuffman(const std::string& stringCof, NodoHuffman* raiz) {
    std::string stringDecof;
    NodoHuffman* nodoActual = raiz;
    
    for (char bit : stringCof) {
        if (bit == '0') {
            nodoActual = nodoActual->izquierda;
        } else {
            nodoActual = nodoActual->derecha;
        }
        
        // Si llegamos a un nodo hoja
        if (nodoActual->izquierda == nullptr && nodoActual->derecha == nullptr) {
            stringDecof += nodoActual->caracter;
            nodoActual = raiz;
        }
    }
    
    return stringDecof;
}

int main() {
    string texto;
    cout << "Ingrese texto: " << endl;
    getline(cin, texto);

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

    // Codificar el texto usando los códigos de Huffman
    string textoCodificado = codificarHuffman(texto, codigos);

    // Mostrar resultados
    cout << "Texto original: " << texto << endl;
    cout << "Texto codificado: " << textoCodificado << endl;

    return 0;
}
