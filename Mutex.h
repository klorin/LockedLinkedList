//
// Created by Akiva on 2020-06-01.
//

#ifndef BARRIER_MUTEX_H
#define BARRIER_MUTEX_H

#include <pthread.h>

class Mutex {
private:
    pthread_mutex_t mutex;
    bool _isLocked = false;

public:
    Mutex();

    ~Mutex(); //NOTE: destroys lock

    void lock();

    void unlock();

    bool isLocked() const;
};

#endif //BARRIER_MUTEX_H
