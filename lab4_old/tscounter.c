//****************************************************
// Definitions for functions to access a Thread Safe Counter
#include <pthread.h>

#include "tscounter.h"

static pthread_mutex_t g_ts_lock = PTHREAD_MUTEX_INITIALIZER;

static int g_ts_counter = 0;

// Reset the thread safe counter to zero
void tsc_reset()
{
    g_ts_counter = 0;
}

// Increment the thread safe counter. 
// Return the value after the increment.
int  tsc_increment()
{
    int value;

    pthread_mutex_lock(&g_ts_lock);
    value = ++g_ts_counter;
    pthread_mutex_unlock(&g_ts_lock);

    return value;
}

// Decrement the thread safe counter.
// Return the value after the decrement.
int  tsc_decrement()
{
    int value;

    pthread_mutex_lock(&g_ts_lock);
    value = --g_ts_counter;
    pthread_mutex_unlock(&g_ts_lock);

    return value;
}

// return the value of the thread safe counter
int  tsc_value()
{
    return g_ts_counter;
}

