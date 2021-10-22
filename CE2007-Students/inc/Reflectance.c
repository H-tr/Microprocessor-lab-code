// Reflectance.c
// Provide functions to take measurements using a QTR-8RC reflectance
// sensor array (Pololu part number 961).  This works by outputting to
// the sensor, waiting, then reading the digital value of each of the
// eight phototransistors.  The more reflective the target surface is,
// the faster the voltage decays.

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/


// reflectance LED illuminate connected to P5.3
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include <stdint.h>
#include "msp432.h"
#include "..\inc\Clock.h"

void Port5_Init(void);
void Port7_Init(void);
void Port7_Output(void);
void Port7_Input(void);

// ------------Reflectance_Init------------
// Initialize the GPIO pins associated with the QTR-8RC reflectance sensor.
// Port7 are configured as inputs to read the reflectance value.
// Port5.3 is configured as output to control the IR LED firing
// Infrared illumination LEDs are initially off.
// Input: none
// Output: none
void Reflectance_Init(void){
    Port5_Init();
    Port7_Init();
}

// Initialise Port 5 for Reflectance Sensing
void Port5_Init(void){
    // write this as part of Lab 2
    P5->SEL0 &= ~0x08;
    P5->SEL1 &= ~0x08;
    P5->DIR |= 0x08;
    P5->OUT |= 0x08;
}

// Initialise Port 7 for Reflectance Sensing
void Port7_Init(void){
    // write this as part of Lab 2
    P7->SEL0 &= ~0x0FF;
    P7->SEL1 &= ~0x0FF;
    P7->DIR &= ~0x0FF;
}

// Set Port7 to output high to charge the capacitors
void Port7_Output(void){
    // write this as part of Lab 2
    P7->DIR |= 0xFF;
    P7->OUT |= 0xFF;
}

// Initialise Port7 to input mode
void Port7_Input(void){
    // write this as part of Lab 2
    P7->DIR &= ~0xFF;
}

// ------------Reflectance_Read------------
// Read the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Read(uint32_t time){
    uint8_t result;
    // write this as part of Lab 2
    P5->OUT = 0x08;
    Port7_Output();
    Clock_Delay1us(10);
    Port7_Input();
    Clock_Delay1us(time);
    result = P7->IN & 0xFF;
    P5->OUT = 0x0;

  return result;
}

// ------------Reflectance_Center------------
// Read *ONLY* the two center sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: 0 (off road), 1 off to left, 2 off to right, 3 on road
// (Left,Right) Sensors
// 1,1          both sensors   on line
// 0,1          just right     off to left
// 1,0          left left      off to right
// 0,0          neither        lost
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Center(uint32_t time){
    uint8_t result;
    // write this as part of Lab 2
    result = Reflectance_Read(time);
    result = (result & 0x18) >> 3;

    return result;
}


// Perform sensor integration
// Input: data is 8-bit result from line sensor
// Output: position in 0.1mm relative to center of line
int32_t Reflectance_Position(uint8_t data){
    uint32_t position;

    // write this as part of Lab 2
    int result = 0;
    int mask = 0x80, num = 0, i;
    int weight[8] = {332, 237, 142, 47, -47, -142, -237, -332};

    for (i= 0; i < 8; ++i) {
        if (mask & data) {
            ++num;
            result += weight[i];
        }
        mask = (mask >> 1);
    }
    position = result / num;

    return position;
}


// ------------Reflectance_Start------------
// Begin the process of reading the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// Input: none
// Output: none
// Assumes: Reflectance_Init() has been called
void Reflectance_Start(void){
    // write this as part of Lab 3
    P5->OUT = 0x08;
    Port7_Output();
    Clock_Delay1us(10);
    Port7_Input();
}


// ------------Reflectance_End------------
// Finish reading the eight sensors
// Read sensors
// Turn off the 8 IR LEDs
// Input: none
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
// Assumes: Reflectance_Start() was called 1 ms ago
uint8_t Reflectance_End(void){
    uint8_t result = 0;
    // write this as part of Lab 3

    result = P7->IN & 0xFF;
    P5->OUT = 0x0;
    return result;
}

