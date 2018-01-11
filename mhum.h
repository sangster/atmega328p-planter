#ifndef MHUM_H
#define MHUM_H
/**
 * @file
 */
#include <util/atomic.h>
#include <util/delay.h>
#include <sangster/util.h>
#include <sangster/pinout.h>
#include <sangster/ring_buff_8.h>
#include <stdbool.h>
#include <stdfix.h>
#include <stdio.h>


/*******************************************************************************
 * Constants
 ******************************************************************************/
#define MOISTURE_BOTTOM    0
#define MOISTURE_TOP     650


/*******************************************************************************
 * Types
 ******************************************************************************/
enum pulse_state
{
    LOW,
    HIGH
};
typedef enum pulse_state PulseState;

union dht11_fixed_16
{
    uint16_t reading;
    struct
    {
        uint8_t integral;
        uint8_t decimal; // seems to always be 0x00 with the DHT11
    } fixed;
};
typedef union dht11_fixed_16 Dht11Fixed16;


/*******************************************************************************
 * Functions
 ******************************************************************************/
bool mhum_read(const Pinout, Dht11Fixed16* humid, Dht11Fixed16* temp);

void dht11_to_s(char*, Dht11Fixed16);

uint8_t map_moisture(uint16_t);

#endif//MHUM_H
