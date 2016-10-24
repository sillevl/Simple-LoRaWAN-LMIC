#include "mbed.h"
#include "lmic.h"
#include "mbed_debug.h"

static u1_t irqlevel = 0;
static u4_t ticks = 0;

static Timer timer;
static Ticker ticker;

static void reset_timer( void ) {
    ticks += timer.read_us( ) >> 6;
    timer.reset( );
}

void hal_init( void ) {
     __disable_irq( );
     irqlevel = 0;

    // configure timer
    timer.start( );
    ticker.attach_us( reset_timer, 10000000 ); // reset timer every 10sec
     __enable_irq( );
}

void hal_disableIRQs( void ) {
    __disable_irq( );
    irqlevel++;
}

void hal_enableIRQs( void ) {
    if( --irqlevel == 0 )
    {
        __enable_irq( );
    }
}

void hal_sleep( void ) {
    // NOP
}

u4_t hal_ticks( void ) {
    hal_disableIRQs( );
    int t = ticks + ( timer.read_us( ) >> 6 );
    hal_enableIRQs( );
    return t;
}

static u2_t deltaticks( u4_t time ) {
    u4_t t = hal_ticks( );
    s4_t d = time - t;
    if( d <= 0 ) {
        return 0;    // in the past
    }
    if( ( d >> 16 ) != 0 ) {
        return 0xFFFF; // far ahead
    }
    return ( u2_t )d;
}

void hal_waitUntil( u4_t time ) {
    while( deltaticks( time ) != 0 ); // busy wait until timestamp is reached
}

u1_t hal_checkTimer( u4_t time ) {
    return ( deltaticks( time ) < 2 );
}

void hal_failed( void ) {
    while( 1 );
}