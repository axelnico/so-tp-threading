#include <iostream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"
#include <chrono>

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cout << "Error: faltan argumentos." << std::endl;
        std::cout << std::endl;
        std::cout << "Modo de uso: " << argv[0] << " <threads_lectura> <threads_maximo>" << std::endl;
        std::cout << "    " << "<archivo1> [<archivo2>...]" << std::endl;
        std::cout << std::endl;
        std::cout << "    threads_lectura: "
            << "Cantidad de threads a usar para leer archivos." << std::endl;
        std::cout << "    threads_maximo: "
            << "Cantidad de threads a usar para computar máximo." << std::endl;
        std::cout << "    archivo1, archivo2...: "
            << "Archivos a procesar." << std::endl;
        return 1;
    }
    int cantThreadsLectura = std::stoi(argv[1]);
    int cantThreadsMaximo = std::stoi(argv[2]);

    std::vector<std::string> filePaths = {};
    for (int i = 3; i < argc; i++) {
        filePaths.push_back(argv[i]);
    }

    HashMapConcurrente hashMap{}; // = HashMapConcurrente();

    auto inicioCarga = std::chrono::steady_clock::now();
    cargarMultiplesArchivos(hashMap, cantThreadsLectura, filePaths);
    auto finCarga = std::chrono::steady_clock::now();

    auto inicioMaximo = std::chrono::steady_clock::now();
    auto maximo = hashMap.maximoParalelo(cantThreadsMaximo);
    auto finMaximo = std::chrono::steady_clock::now();

    double tiempoCarga = std::chrono::duration<double,std::milli>(finCarga - inicioCarga).count();
    double tiempoMaximo = std::chrono::duration<double,std::milli>(finMaximo - inicioMaximo).count();

    // Imprimimos el tiempo de ejecucion por stderr
    std::clog << tiempoCarga << "-"  << tiempoMaximo  << std::endl;

    std::cout << maximo.first << " " << maximo.second << std::endl;

    return 0;
}
