#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int hash = this->hashIndex(clave);
    this->mutexBuckets[hash].lock();
    bool found = false;
    for(auto &par : *this->tabla[hash]){
        if (par.first == clave){
            par.second++;
            found = true;
            break;
        }
    }
    if(!found){
        hashMapPair par (clave,1);
        this->tabla[hash]->insertar(par);
    }
    this->mutexBuckets[hash].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> todasLasClaves;
    for (auto & index : tabla) {
        for (auto &p : *index) {
            todasLasClaves.push_back(p.first);
        }
    }
    return todasLasClaves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    unsigned int hash = this->hashIndex(clave);
    for(auto &par : *this->tabla[hash]){
        if (par.first == clave){
           return par.second;
        }
    }
    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        this->mutexBuckets[index].lock();
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
        this->mutexBuckets[index].unlock();
    }
    return *max;
}



hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    std::vector<unsigned int> filasRestantes;
    for(int i = 0 ; i < HashMapConcurrente::cantLetras ; i ++){
        filasRestantes.push_back(i);
    }
    std::mutex mutexFilasRestantes;
    hashMapPair max;
    max.second = 0;
    std::mutex mutexMaximo;

    std::vector<std::thread> threads(cant_threads);

    for (std::thread &t : threads) {
        t = std::thread([&filasRestantes, &mutexFilasRestantes,&max,&mutexMaximo,this] () {
            while (true){
                mutexFilasRestantes.lock();
                if(filasRestantes.empty()){
                    mutexFilasRestantes.unlock();
                    break;
                }
                unsigned int filaARecorrer = filasRestantes.back();
                filasRestantes.pop_back();
                mutexFilasRestantes.unlock();
                hashMapPair maxLocal;
                maxLocal.second = 0;
                this->mutexBuckets[filaARecorrer].lock();
                for (auto &p : *tabla[filaARecorrer]) {
                    if (p.second > maxLocal.second) {
                        maxLocal.first = p.first;
                        maxLocal.second = p.second;
                    }
                }
                this->mutexBuckets[filaARecorrer].unlock();
                mutexMaximo.lock();
                if(maxLocal.second > max.second){
                    max.second = maxLocal.second;
                    max.first = maxLocal.first;
                }
                mutexMaximo.unlock();
            }
        });
    }
    for (std::thread &t : threads) {
        t.join();
    }
    return max;
}

#endif
