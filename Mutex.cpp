//
// Created by Akiva on 2020-06-01.
//

#include "Mutex.h"
#include <stdio.h>

Mutex::Mutex() {
    if (pthread_mutex_init(&mutex, NULL)) perror("Mutex::ctor");
}

Mutex::~Mutex() {
    if (pthread_mutex_destroy(&mutex)) perror("Mutex::ctor");
}

void Mutex::lock() {
    if (pthread_mutex_lock(&mutex)) perror("Mutex::lock");
    _isLocked = true;
}

void Mutex::unlock() {
    _isLocked = false;
    if (pthread_mutex_unlock(&mutex)) perror("Mutex::unlock");
}

bool Mutex::isLocked() const {
    return _isLocked;
}
