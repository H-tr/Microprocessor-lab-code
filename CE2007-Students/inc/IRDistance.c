// IRDistance.c
// Runs on MSP432
// Provide mid-level functions that convert raw ADC
// values from the GP2Y0A21YK0F infrared distance sensors to
// distances in mm.
// Jonathan Valvano
// May 25, 2017

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

// Pololu #3543 Vreg (5V regulator output) connected to all three Pololu #136 GP2Y0A21YK0F Vcc's (+5V) and MSP432 +5V (J3.21)
// Pololu #3543 Vreg (5V regulator output) connected to positive side of three 10 uF capacitors physically near the sensors
// Pololu ground connected to all three Pololu #136 GP2Y0A21YK0F grounds and MSP432 ground (J3.22)
// Pololu ground connected to negative side of all three 10 uF capacitors
// MSP432 P9.0 (J5) (analog input to MSP432) connected to right GP2Y0A21YK0F Vout
// MSP432 P4.1 (J1.5) (analog input to MSP432) connected to center GP2Y0A21YK0F Vout
// MSP432 P9.1 (J5) (analog input to MSP432) connected to left GP2Y0A21YK0F Vout

#include <stdint.h>
#include "../inc/ADC14.h"
#include "msp.h"



int32_t LeftConvert(int32_t nl){        // returns left distance in mm
  // write this for Lab 4
    uint32_t length=0;
    int x1, x2, y1, y2;
    if (14380 <= nl && nl < 16251){
        x1 = 16251;
        x2 = 14380;
        y1 = 50;
        y2 = 100;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (7269 <= nl && nl < 14380) {
        x1 = 14380;
        x2 = 7269;
        y1 = 100;
        y2 = 200;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (5379 <= nl && nl < 7269) {
        x1 = 7269;
        x2 = 5379;
        y1 = 200;
        y2 = 300;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (4331 <= nl && nl < 5379) {
        x1 = 5379;
        x2 = 4331;
        y1 = 300;
        y2 = 400;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (3379 <= nl && nl < 4331) {
        x1 = 4331;
        x2 = 3379;
        y1 = 400;
        y2 = 500;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (3008 <= nl && nl < 3379) {
        x1 = 3379;
        x2 = 3008;
        y1 = 500;
        y2 = 600;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (2842 <= nl && nl < 3008) {
        x1 = 3008;
        x2 = 2842;
        y1 = 600;
        y2 = 700;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    } else if (2716 <= nl && nl < 2842) {
        x1 = 2842;
        x2 = 2716;
        y1 = 700;
        y2 = 800;
        length = (uint32_t)(1.0*(nl - x2)/(x1-x2)*(y1-y2)+y2);
    }

    return length; // replace this line
}

int32_t CenterConvert(int32_t nc){   // returns center distance in mm
  // write this for Lab 4
    uint32_t length=0;
    int x1, x2, y1, y2;
        if (14380 <= nc && nc < 16251){
            x1 = 16251;
            x2 = 14380;
            y1 = 50;
            y2 = 100;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (7269 <= nc && nc < 14380) {
            x1 = 14380;
            x2 = 7269;
            y1 = 100;
            y2 = 200;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (5379 <= nc && nc < 7269) {
            x1 = 7269;
            x2 = 5379;
            y1 = 200;
            y2 = 300;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (4331 <= nc && nc < 5379) {
            x1 = 5379;
            x2 = 4331;
            y1 = 300;
            y2 = 400;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (3379 <= nc && nc < 4331) {
            x1 = 4331;
            x2 = 3379;
            y1 = 400;
            y2 = 500;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (3008 <= nc && nc < 3379) {
            x1 = 3379;
            x2 = 3008;
            y1 = 500;
            y2 = 600;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (2842 <= nc && nc < 3008) {
            x1 = 3008;
            x2 = 2842;
            y1 = 600;
            y2 = 700;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (2716 <= nc && nc < 2842) {
            x1 = 2842;
            x2 = 2716;
            y1 = 700;
            y2 = 800;
            length = (uint32_t)(1.0*(nc - x2)/(x1-x2)*(y1-y2)+y2);
        }

    return length; // replace this line
}

int32_t RightConvert(int32_t nr){      // returns right distance in mm
  // write this for Lab 4
    uint32_t length=0;
    int x1, x2, y1, y2;
        if (14380 <= nr && nr < 16251){
            x1 = 16251;
            x2 = 14380;
            y1 = 50;
            y2 = 100;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (7269 <= nr && nr < 14380) {
            x1 = 14380;
            x2 = 7269;
            y1 = 100;
            y2 = 200;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (5379 <= nr && nr < 7269) {
            x1 = 7269;
            x2 = 5379;
            y1 = 200;
            y2 = 300;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (4331 <= nr && nr < 5379) {
            x1 = 5379;
            x2 = 4331;
            y1 = 300;
            y2 = 400;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (3379 <= nr && nr < 4331) {
            x1 = 4331;
            x2 = 3379;
            y1 = 400;
            y2 = 500;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (3008 <= nr && nr < 3379) {
            x1 = 3379;
            x2 = 3008;
            y1 = 500;
            y2 = 600;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (2842 <= nr && nr < 3008) {
            x1 = 3008;
            x2 = 2842;
            y1 = 600;
            y2 = 700;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        } else if (2716 <= nr && nr < 2842) {
            x1 = 2842;
            x2 = 2716;
            y1 = 700;
            y2 = 800;
            length = (uint32_t)(1.0*(nr - x2)/(x1-x2)*(y1-y2)+y2);
        }

    return length; // replace this line
}
