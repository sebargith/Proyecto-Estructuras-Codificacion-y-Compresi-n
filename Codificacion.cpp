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

int main() {

    return 0;
}
