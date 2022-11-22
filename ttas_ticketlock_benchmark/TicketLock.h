#include "Lock.h"

#include <atomic>

class LockTicket : public LockClass {
  protected:
    std::atomic_size_t now_serving = {0};
    std::atomic_size_t next_ticket = {0};

  public:
    void lock() {
        const auto ticket = next_ticket.fetch_add(1);
        while(now_serving.load() != ticket);
    }

    void unlock() {
        const auto successor = now_serving.load() + 1;
        now_serving.store(successor);
    };
};

class LockTicket_Acquire_Release : public LockTicket {
  public:
    void lock() {
        const auto ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while(now_serving.load(std::memory_order_acquire) != ticket);
    }

    void unlock() {
        const auto successor = now_serving.load(std::memory_order_relaxed) + 1;
        now_serving.store(successor, std::memory_order_release);
    };
};

class LockTicket_Yield : public LockTicket_Acquire_Release {
  public:
    void lock() {
        const auto ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while(now_serving.load(std::memory_order_acquire) != ticket) {
            sched_yield();
        };
    }
};

class LockTicket_Exp : public LockTicket_Acquire_Release {
  public:
    void lock() {
        useconds_t delay = DELAY;
        const auto ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while(now_serving.load(std::memory_order_acquire) != ticket) {
            usleep(delay);
            if (delay < MAX_DELAY)
            {
                delay *= 2;
            }
        };
    }
};

class LockTicket_Exp_and_yield : public LockTicket_Acquire_Release {
  public:
    void lock() {
        useconds_t delay = DELAY;
        const auto ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while(now_serving.load(std::memory_order_acquire) != ticket) {
            if (delay >= MAX_DELAY_2) {
                delay = DELAY;
                sched_yield();
            }
            usleep(delay);
            if (delay < MAX_DELAY_2)
            {
                delay *= 2;
            }
        };
    }
};