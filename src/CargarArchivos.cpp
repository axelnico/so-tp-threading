#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <thread>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}


void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    std::atomic<int> indiceARevisar(0);

    std::vector<std::thread> threads(cantThreads);

    for (std::thread &t : threads) {
        t = std::thread([&indiceARevisar,&filePaths, &hashMap] () {
            while (true){
                unsigned int filaARecorrer = indiceARevisar.fetch_add(1);
                if(filaARecorrer >= filePaths.size()){
                    break;
                }
                cargarArchivo(hashMap,filePaths[filaARecorrer]);
            }
        });
    }
    for (std::thread &t : threads) {
        t.join();
    }
}

#endif
