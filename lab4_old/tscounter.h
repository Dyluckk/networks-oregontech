#pragma once
//****************************************************
// Declarations for functions to access a Thread Safe Counter

// Reset the thread safe counter to zero
void tsc_reset();

// Increment the thread safe counter. 
// Return the value after the increment.
int  tsc_increment();

// Decrement the thread safe counter.
// Return the value after the decrement.
int  tsc_decrement();

// return the value of the thread safe counter
int  tsc_value();

