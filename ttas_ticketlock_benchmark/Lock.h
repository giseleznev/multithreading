#pragma once
#include <pthread.h>
#include <unistd.h>

const useconds_t DELAY = 1;
const useconds_t MAX_DELAY = DELAY * 16;
const useconds_t MAX_DELAY_2 = DELAY * 8;

class LockClass {
  public:
    virtual void lock() = 0;

    virtual void unlock() = 0;

    virtual ~LockClass(){};
};