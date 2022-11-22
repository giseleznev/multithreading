#include "Lock.h"

#include <atomic>
#include <cassert>

class LockTTAS : public LockClass {
  protected:
    std::atomic<unsigned int> m_spin;

  public:
    LockTTAS() : m_spin(0) {};

    ~LockTTAS() { assert(m_spin.load() == 0); };

    void lock() {
        unsigned int expected;
        do{
            while(m_spin.load());
            expected = 0;
        } while ( !m_spin.compare_exchange_weak(expected, 1));
    };

    void unlock() {
        m_spin.store(0);
    };
};

class LockTTAS_Acquire_Release : public LockTTAS {
  public:
    using LockTTAS::LockTTAS;

    ~LockTTAS_Acquire_Release() { assert(m_spin.load(std::memory_order_relaxed) == 0); };

    void lock() {
        unsigned int expected;
        do{
            while(m_spin.load(std::memory_order_acquire));
            expected = 0;
        } while ( !m_spin.compare_exchange_weak(expected, 1, std::memory_order_relaxed));
    };

    void unlock() {
        m_spin.store(0, std::memory_order_release);
    };
};

class LockTTAS_Yield : public LockTTAS_Acquire_Release {
  public:
    using LockTTAS_Acquire_Release::LockTTAS_Acquire_Release;

    void lock() {
      unsigned int expected;
        do{
            while(m_spin.load(std::memory_order_acquire)) {
              sched_yield();
            };
            expected = 0;
        } while ( !m_spin.compare_exchange_weak(expected, 1, std::memory_order_relaxed));
    };
};

class LockTTAS_Exp : public LockTTAS_Acquire_Release {
  public:
    using LockTTAS_Acquire_Release::LockTTAS_Acquire_Release;

    void lock() {
        useconds_t delay = DELAY;
        unsigned int expected;
        do {
            while(m_spin.load(std::memory_order_acquire)) {
                usleep(delay);
                if (delay < MAX_DELAY)
                {
                    delay *= 2;
                }
            };
            expected = 0;
        } while ( !m_spin.compare_exchange_weak(expected, 1, std::memory_order_relaxed));
    };
};

class LockTTAS_Exp_and_yield : public LockTTAS_Acquire_Release {
  public:
    using LockTTAS_Acquire_Release::LockTTAS_Acquire_Release;

    void lock() {
        useconds_t delay = DELAY;
        unsigned int expected;
        do {
            while(m_spin.load(std::memory_order_acquire)) {
                if (delay >= MAX_DELAY_2){
                  delay = DELAY;
                  sched_yield();
                }
                usleep(delay);
                if (delay < MAX_DELAY)
                {
                    delay *= 2;
                }
            };
            expected = 0;
        } while ( !m_spin.compare_exchange_weak(expected, 1, std::memory_order_relaxed));
    };
};