#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>

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
            descomprimido += descomprimido.substr(pos, par.lon);
        }
        if (par.sigchar != '\0') {
            descomprimido += par.sigchar;
        }
    }
    return descomprimido;
}

// Dataset: Texto de ejemplo para probar las implementaciones
const std::string dataset = R"(
¿Dónde vive y cuál es su hábitat?
Las hormigas suelen vivir bajo tierra en colonias organizadas. En estas colonias hay dos tipos
de hormigas: la reina, única hembra reproductora de gran tamaño y que puede vivir hasta 30 años;
y las obreras, de menos tamaño y sus funciones son de recolección de alimentos y cuidado de crías.
Nos encontramos, por tanto, ante unos insectos hipersociales que en algunos casos son confundidos
con las temitas y cuya jerarquía guarda ciertas similitudes con la de las abejas.

Una de sus características destacadas es su capacidad para trazar hileras de una especie 
de hormiga granívora propia de nuestros secarrales en altitudes superiores a 500 metros.

Estas hormigas se distribuyen por el Mediterráneo occidental y la costa Atlántica, 
incluido el norte de África occidental.

¿Qué comen las hormigas y cómo se reproducen?
Siempre van de un lado a otro recogiendo semillas u otros elementos que 
utilizan para alimentarse y construir sus nidos. Y lamentablemente no siempre vuelven con alguna 
semilla. Según el profesor Espadaler, mirmecólogo: “A menudo regresan de vacío. No es grave. 
El sistema acepta perfectamente los errores e ineficacias”.

Los principales alimentos de las hormigas lo constituyen las plantas, la miel y el néctar.  
Las hormigas obreras son las encargadas de salir de sus nidos subterráneos en busca de alimento, 
para lo que necesitan construir caminos estables que se utilizan durante meses o años.

Cabe destacar que estas obreras no buscan el alimento en grupo, como sí sucede en otras muchas 
especies. Lo hacen de manera individual y logran que los caminos con otras colonias no se crucen 
en ningún momento. Se comunican entre ellas mediante sustancias químicas (secreción de feromonas 
percibidas por sus antenas) que llegan a alertar a las demás de los peligros existentes.

En lo relativo al ciclo reproductivo el mundo de las hormigas no iba a resultar menos atractivo. 
Tras la pérdida de la reina en una colonia se da la partenogénesis, una forma de reproducción basada 
en el desarrollo de células sexuales femeninas no fecundadas.
)";

int main() {
    SetConsoleOutputCP(CP_UTF8); // Carácteres en español
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
    std::cout << "Tamaño de Compresión LZ" << ComprimidoLZ.size() * sizeof(ParLZ) * 8 << " bits\n";

    return 0;
}
