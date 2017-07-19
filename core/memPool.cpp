/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memPool.h"

////////////////
Locker::Locker()
{
    pthread_mutex_init(&_mutex, NULL);
}

Locker::~Locker()
{
    pthread_mutex_destroy(&_mutex);
}

void Locker::lock()
{
    pthread_mutex_lock(&_mutex);
}

void Locker::unlock()
{
    pthread_mutex_unlock(&_mutex);
}

////////////////

