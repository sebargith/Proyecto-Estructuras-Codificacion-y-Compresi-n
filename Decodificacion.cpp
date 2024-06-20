#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Nodo del árbol de Huffman
struct NodoHuffman {
    char simbolo;
    int frequencia;
    NodoHuffman* left;
    NodoHuffman* right;

    NodoHuffman(char s, int f) : simbolo(s), frequencia(f), left(nullptr), right(nullptr) {}
};

// Comparador para la cola de prioridad
struct Comparador {
    bool operator()(NodoHuffman* left, NodoHuffman* right) {
        return left->frequencia > right->frequencia;
    }
};

// Función para contar frecuencias de caracteres
std::unordered_map<char, int> countFrequencias(const std::string& str) {
    std::unordered_map<char, int> frequencias;
    for (char ch : str) {
        frequencias[ch]++;
    }
    return frequencias;
}

// Funcion para construir el árbol de Huffman
NodoHuffman* generarArbolHuffman(const std::unordered_map<char, int>& frequencias) {
    std::priority_queue<NodoHuffman*, std::vector<NodoHuffman*>, Comparador> minHeap;
    
    for (const auto& pair : frequencias) {
        minHeap.push(new NodoHuffman(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        NodoHuffman* left = minHeap.top();
        minHeap.pop();
        NodoHuffman* right = minHeap.top();
        minHeap.pop();
        
        NodoHuffman* merged = new NodoHuffman('\0', left->frequencia + right->frequencia);
        merged->left = left;
        merged->right = right;
        
        minHeap.push(merged);
    }
    
    return minHeap.top();
}

// Función para generar códigos de Huffman
void generarCodigosHuffman(NodoHuffman* node, const std::string& code, std::unordered_map<char, std::string>& codigosHuffman) {
    if (!node) return;

    if (node->simbolo != '\0') {
        codigosHuffman[node->simbolo] = code;
    }

    generarCodigosHuffman(node->left, code + "0", codigosHuffman);
    generarCodigosHuffman(node->right, code + "1", codigosHuffman);
}

// Función para codificar un string usando los códigos de Huffman
std::string encodeString(const std::string& str, const std::unordered_map<char, std::string>& codigosHuffman) {
    std::string encodedString;
    for (char ch : str) {
        encodedString += codigosHuffman.at(ch);
    }
    return encodedString;
}

// Funcián para decodificar un string codificado usando el árbol de Huffman
std::string decodeString(const std::string& encodedStr, NodoHuffman* root) {
    std::string decodedString;
    NodoHuffman* currentNode = root;
    
    for (char bit : encodedStr) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }
        
        // Si llegamos a un nodo hoja
        if (currentNode->left == nullptr && currentNode->right == nullptr) {
            decodedString += currentNode-> simbolo;
            currentNode = root;
        }
    }
    
    return decodedString;
}


int main() {
    std::string str;
    
//string texto;
    cout << "Ingrese texto: " << endl;
    getline(cin, str);
    
    // Contar frecuencias
    auto frequencias = countFrequencias(str);
    
    // Construir el árbol de Huffman
    NodoHuffman* huffmanTree = generarArbolHuffman(frequencias);
    
    // Generar códigos de Huffman
    std::unordered_map<char, std::string> codigosHuffman;
    generarCodigosHuffman(huffmanTree, "", codigosHuffman);
    
    // Codificar el string
    std::string encodedString = encodeString(str, codigosHuffman);
    
    // Decodificar el string codificado
    std::string decodedString = decodeString(encodedString, huffmanTree);
    
    // Imprimir resultados
    std::cout << "Huffman Codes:\n";
    for (const auto& pair : codigosHuffman) {
        std::cout << pair.first << ": " << pair.second << "\n";
    }
    
    //std::cout << "\nOriginal string: " << str << "\n";
    std::cout << "Encoded string: " << encodedString << "\n";
    std::cout << "Decoded string: " << decodedString << "\n";
    std::cout << "Original length (bits): " << str.length() * 8 << "\n";
    std::cout << "Encoded length (bits): " << encodedString.length() << "\n";

    return 0;
}

