/*
 * ECE 266 Lab 2 starter code, Fall 2020
 *
 * lab2_main.c
 *
 * Created by Zhao Zhang
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include "launchpad.h"
#include "seg7.h"

// 7-segment coding table. See https://en.wikipedia.org/wiki/Seven-segment_display. The segments
// are named as A, B, C, D, E, F, G. In this coding table, segments A-G are mapped to bits 0-7.
// Bit 7 is not used in the coding. This display uses active high signal, in which '1' turns ON a
// segment, and '0' turns OFF a segment.
static uint8_t seg7Coding[10] = {
        0b00111111, 		// digit 0
        0b00000110, 		// digit 1
        0b01011011,			// digit 2
        0b01001111,			// digit 3
        0b01100110,         // digit 4
        0b01101101,         // digit 5
        0b01111101,         // digit 6
        0b00000111,         // digit 7
        0b01111111,         // digit 8
        0b01100111          // digit 9
        };

// The colon status: if colon == 0b10000000, then the colon is on,
// otherwise it is off.
static uint8_t colon = 0;
static uint32_t seconds=0;
// Update the clock display
void clockUpdate(uint32_t time)						// pointer to a 4-byte array
{
    uint8_t code[4];			// The 7-segment code for the four clock digits

    // Display 01:23 on the 7-segment displays
    // The colon ':' will flash on and off every 0.5 seconds
    code[0] = seg7Coding[seconds%10] + colon;
    code[1] = seg7Coding[(seconds%60)/10] + colon;
    code[2] = seg7Coding[(seconds%600)/60] + colon;
    code[3] = seg7Coding[(seconds%3600)/600] + colon;





    // Calculate the display digits and colon setting for the next update
    if (colon == 0b00000000)
    {
        colon = 0b10000000;
        seconds++;
    }
    else
    {
        colon = 0b00000000;
    }
    seg7Update(code);
    // Call back after 1 second
    schdCallback(clockUpdate, time + 500);
}
void
checkPushButton(uint32_t time)
{
    // Read the pushbutton state; see pbRead() in launchpad.h
    int code = pbRead();
    uint32_t delay = 50;

    switch (code) {
    case 1:
        //since display is managed by clockUpdate, this function simply increases the seconds counter,
        // wrap around is managed by clockUpdate
        seconds+=60;

        // Use an inertia for soft de-bouncing
        delay = 250;
        break;

    case 2:
        //since display is managed by clockUpdate, this function simply increases the seconds counter,
        // wrap around is managed by clockUpdate
        seconds+=1;

        // Use an inertia for soft de-bouncing
        delay = 250;
        break;
    }

    schdCallback(checkPushButton, time + delay);
}
int main(void)
{
    lpInit();
    seg7Init();
    uprintf("%s\n\r", "Lab 2: Wall clock");

    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(clockUpdate, 1000);
    schdCallback(checkPushButton, 1000);

    // Loop forever
    while (true)
    {
        schdExecute();
    }
}
