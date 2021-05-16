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
    // Completar (Ejercicio 4)
    std::vector<std::string> archivosRestantes = filePaths;
    std::mutex mutexArchivosRestantes;

    std::vector<std::thread> threads(cantThreads);

    for (std::thread &t : threads) {
        t = std::thread([&archivosRestantes, &mutexArchivosRestantes, &hashMap] () {
            while (true){
                mutexArchivosRestantes.lock();
                if(archivosRestantes.empty()){
                    mutexArchivosRestantes.unlock();
                    break;
                }
                std::string archivoARecorrer = archivosRestantes.back();
                archivosRestantes.pop_back();
                mutexArchivosRestantes.unlock();
                cargarArchivo(hashMap,archivoARecorrer);
            }
        });
    }
    for (std::thread &t : threads) {
        t.join();
    }
}

#endif
