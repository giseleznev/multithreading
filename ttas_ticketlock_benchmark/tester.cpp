#include "TTASLock.h"
#include "TicketLock.h"
#include <fstream>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

const int N_Threads_max = 30;
const int TimesToRepeat = 100;
auto max_times = std::ofstream("max_times.txt");

struct DataStruct{
    LockClass* Lock;
    int DefendedValue;
} typedef DataStruct;

void* IncreaseValue(void* args) {
    auto Data = (DataStruct*)(args);
    Data->Lock->lock();
    Data->DefendedValue /*++*/ = 0;
    for( volatile int i = 0; i < 1000000; i ++ ) { Data->DefendedValue++; }
    Data->Lock->unlock();
}

bool TestLock(int N_threads, LockClass* Lock) {
    auto Data = DataStruct{Lock, 0};
    pthread_t tid[N_threads];
    for( int thread_index = 0; thread_index < N_threads; thread_index ++ ){
        pthread_create(&tid[thread_index], NULL, IncreaseValue, (void*)&Data);
    }
    for( int thread_index = 0; thread_index < N_threads; thread_index ++ ){
       pthread_join(tid[thread_index], NULL);
    }
    return true; /*Data.DefendedValue == N_threads;*/
}

void RunTester(LockClass* Lock) {
    std::string lockname = typeid(*Lock).name();
    auto file = std::ofstream(lockname);
    struct timespec mt1, mt2;
    long int tt, max_tt = 0;
    for ( int N_threads = 1; N_threads < N_Threads_max; N_threads++ ) {
        printf("N_threads = %d\n", N_threads);
        tt = 0;
        for( int i = 0; i < TimesToRepeat; i ++ ) {
            printf("%d, ", i);
            clock_gettime(CLOCK_REALTIME, &mt1);
            assertm(TestLock(N_threads, Lock), lockname);
            clock_gettime(CLOCK_REALTIME, &mt2);
            tt += 1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec) / N_threads;
            max_tt = std::max(tt, max_tt);
        }
        tt = tt / TimesToRepeat;
        file << " " << tt << std::endl;
        printf("N_threads\n", N_threads);
    }
    max_times << lockname << " " << max_tt << std::endl;
}

int main() {
    {
        LockTicket lock1; RunTester(&lock1);
        LockTTAS lock2; RunTester(&lock2);
    }
    {
        LockTicket_Acquire_Release lock1; RunTester(&lock1);
        LockTTAS_Acquire_Release lock2; RunTester(&lock2);
    }
    {
        LockTicket_Yield lock1; RunTester(&lock1);
        LockTTAS_Yield lock2; RunTester(&lock2);
    }
    {
        LockTicket_Exp lock1; RunTester(&lock1);
        LockTTAS_Exp lock2; RunTester(&lock2);
    }
    {
        LockTicket_Exp_and_yield lock1; RunTester(&lock1);
        LockTTAS_Exp_and_yield lock2; RunTester(&lock2);
    }
}