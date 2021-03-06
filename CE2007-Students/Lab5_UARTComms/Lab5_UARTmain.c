// RSLK Self Test via UART

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

#include "msp.h"
#include <stdint.h>
#include <string.h>
#include "..\inc\UART0.h"
#include "..\inc\EUSCIA0.h"
#include "..\inc\FIFO0.h"
#include "..\inc\Clock.h"
//#include "..\inc\SysTick.h"
#include "..\inc\SysTickInts.h"
#include "..\inc\CortexM.h"
#include "..\inc\TimerA1.h"
//#include "..\inc\Bump.h"
#include "..\inc\BumpInt.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\Motor.h"
#include "../inc/IRDistance.h"
#include "../inc/ADC14.h"
#include "../inc/LPF.h"
#include "..\inc\Reflectance.h"
#include "../inc/TA3InputCapture.h"
#include "../inc/Tachometer.h"

#define P2_4 (*((volatile uint8_t *)(0x42098070)))
#define P2_3 (*((volatile uint8_t *)(0x4209806C)))
#define P2_2 (*((volatile uint8_t *)(0x42098068)))
#define P2_1 (*((volatile uint8_t *)(0x42098064)))
#define P2_0 (*((volatile uint8_t *)(0x42098060)))


// At 115200, the bandwidth = 11,520 characters/sec
// 86.8 us/character
// normally one would expect it to take 31*86.8us = 2.6ms to output 31 characters
// Random number generator
// from Numerical Recipes
// by Press et al.
// number from 0 to 31
uint32_t Random(void){
static uint32_t M=1;
  M = 1664525*M+1013904223;
  return(M>>27);
}
char WriteData,ReadData;
uint32_t NumSuccess,NumErrors;
void TestFifo(void){char data;
  while(TxFifo0_Get(&data)==FIFOSUCCESS){
    if(ReadData==data){
      ReadData = (ReadData+1)&0x7F; // 0 to 127 in sequence
      NumSuccess++;
    }else{
      ReadData = data; // restart
      NumErrors++;
    }
  }
}
uint32_t Size;
int Program5_1(void){
//int main(void){
    // test of TxFifo0, NumErrors should be zero
  uint32_t i;
  Clock_Init48MHz();
  WriteData = ReadData = 0;
  NumSuccess = NumErrors = 0;
  TxFifo0_Init();
  TimerA1_Init(&TestFifo,43);  // 83us, = 12kHz
  EnableInterrupts();
  while(1){
    Size = Random(); // 0 to 31
    for(i=0;i<Size;i++){
      TxFifo0_Put(WriteData);
      WriteData = (WriteData+1)&0x7F; // 0 to 127 in sequence
    }
    Clock_Delay1ms(10);
  }
}

char String[64];
uint32_t MaxTime,First,Elapsed;
int Program5_2(void){
//int main(void){
    // measurement of busy-wait version of OutString
  uint32_t i;
  DisableInterrupts();
  Clock_Init48MHz();
  UART0_Init();
  WriteData = 'a';
  SysTick_Init(0x1000000,2); //OHL - using systick INT api
  MaxTime = 0;
  while(1){
    Size = Random(); // 0 to 31
    for(i=0;i<Size;i++){
      String[i] = WriteData;
      WriteData++;
      if(WriteData == 'z') WriteData = 'a';
    }
    String[i] = 0; // null termination
    First = SysTick->VAL;
    UART0_OutString(String);
    Elapsed = ((First - SysTick->VAL)&0xFFFFFF)/48; // usec

    if(Elapsed > MaxTime){
        MaxTime = Elapsed;
    }
    UART0_OutChar(CR);UART0_OutChar(LF);
    Clock_Delay1ms(100);
  }
}

int Program5_3(void){
//int main(void){
    // measurement of interrupt-driven version of OutString
  uint32_t i;
  DisableInterrupts();
  Clock_Init48MHz();
  EUSCIA0_Init();
  WriteData = 'a';
  SysTick_Init(0x1000000,2); //OHL - using systick INT api
  MaxTime = 0;
  EnableInterrupts();
  while(1){
    Size = Random(); // 0 to 31
    for(i=0;i<Size;i++){
      String[i] = WriteData;
      WriteData++;
      if(WriteData == 'z') WriteData = 'a';
    }
    String[i] = 0; // null termination
    First = SysTick->VAL;
    EUSCIA0_OutString(String);
    Elapsed = ((First - SysTick->VAL)&0xFFFFFF)/48; // usec
    if(Elapsed > MaxTime){
        MaxTime = Elapsed;
    }
    EUSCIA0_OutChar(CR);EUSCIA0_OutChar(LF);
    Clock_Delay1ms(100);
  }
}
int Program5_4(void){
//int main(void){
    // demonstrates features of the EUSCIA0 driver
  char ch;
  char string[20];
  uint32_t n;
  DisableInterrupts();
  Clock_Init48MHz();  // makes SMCLK=12 MHz
  EUSCIA0_Init();     // initialize UART
  EnableInterrupts();
  EUSCIA0_OutString("\nLab 5 Test program for EUSCIA0 driver\n\rEUSCIA0_OutChar examples\n");
  for(ch='A'; ch<='Z'; ch=ch+1){// print the uppercase alphabet
     EUSCIA0_OutChar(ch);
  }
  EUSCIA0_OutChar(LF);
  for(ch='a'; ch<='z'; ch=ch+1){// print the lowercase alphabet
    EUSCIA0_OutChar(ch);
  }
  while(1){
    EUSCIA0_OutString("\n\rInString: ");
    EUSCIA0_InString(string,19); // user enters a string
    EUSCIA0_OutString(" OutString="); EUSCIA0_OutString(string); EUSCIA0_OutChar(LF);

    EUSCIA0_OutString("InUDec: ");   n=EUSCIA0_InUDec();
    EUSCIA0_OutString(" OutUDec=");  EUSCIA0_OutUDec(n); EUSCIA0_OutChar(LF);
    EUSCIA0_OutString(" OutUFix1="); EUSCIA0_OutUFix1(n); EUSCIA0_OutChar(LF);
    EUSCIA0_OutString(" OutUFix2="); EUSCIA0_OutUFix2(n); EUSCIA0_OutChar(LF);

    EUSCIA0_OutString("InUHex: ");   n=EUSCIA0_InUHex();
    EUSCIA0_OutString(" OutUHex=");  EUSCIA0_OutUHex(n); EUSCIA0_OutChar(LF);
  }
}


//-------------For IR Sensors (case 2)-----------------------
//Copied from ADCmain (Lab4)
volatile uint32_t nr,nc,nl;
volatile uint32_t ADCflag = 0;
void SensorRead_ISR(void){  // runs at 2000 Hz
    uint32_t raw17,raw12,raw16;
    P1OUT ^= 0x01;         // profile
    P1OUT ^= 0x01;         // profile
    ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
    nr = LPF_Calc(raw17);  // right is channel 17 P9.0
    nc = LPF_Calc2(raw12); // center is channel 12, P4.1
    nl = LPF_Calc3(raw16); // left is channel 16, P9.1
    ADCflag = 1;           // semaphore
    P1OUT ^= 0x01;         // profile
}


void IRSensor_Init(){
    uint32_t raw17,raw12,raw16;
    uint32_t s;
    ADCflag = 0;
    s = 256; // replace with your choice
    ADC0_InitSWTriggerCh17_12_16();   // initialize channels 17,12,16
    ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
    LPF_Init(raw17,s);     // P9.0/channel 17
    LPF_Init2(raw12,s);    // P4.1/channel 12
    LPF_Init3(raw16,s);    // P9.1/channel 16

}
//-----------------------------------------------------------


//------------For Bump Switch (case 3)-----------------------

volatile uint8_t bumpData, bumpFlag;  // mailbox
void HandleBump(uint8_t bumpSensor){
   bumpData = bumpSensor;
   bumpFlag = 1;
   P4->IFG &= ~0xED;                  // clear interrupt flags (reduce possibility of extra interrupt)
}

//-----------------------------------------------------------


//-----------For Tachometer (case 5)-------------------------
//Copied from Tachmain (Lab4)
uint16_t Period0;              // (1/SMCLK) units = 83.3 ns units
uint16_t First0;               // Timer A3 first edge, P10.4
int Done0;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure0(uint16_t time){
  P2_0 = P2_0^0x01;           // thread profile, P2.0
  Period0 = (time - First0)&0xFFFF; // 16 bits, 83.3 ns resolution
  First0 = time;                   // setup for next
  Done0 = 1;
}
uint16_t Period2;              // (1/SMCLK) units = 83.3 ns units
uint16_t First2;               // Timer A3 first edge, P8.2
int Done2;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure2(uint16_t time){
  P2_2 = P2_2^0x01;           // thread profile, P2.4
  Period2 = (time - First2)&0xFFFF; // 16 bits, 83.3 ns resolution
  First2 = time;                   // setup for next
  Done2 = 1;
}

void TimedPause(uint32_t time){
  Clock_Delay1ms(time);          // run for a while and stop
  Motor_Stop();
  while(LaunchPad_Input()==0);  // wait for touch
  while(LaunchPad_Input());     // wait for release
}

#define PERIOD 1000  // must be even

void toggle_GPIO(void){
    P2_4 ^= 0x01;     // create output
}

void Tachometer_Init(){
    P2->SEL0 &= ~0x11;
    P2->SEL1 &= ~0x11;  // configure P2.0 and P2.4 as GPIO
    P2->DIR |= 0x11;    // P2.0 and P2.4 outputs
    First0 = First2 = 0; // first will be wrong
    Done0 = Done2 = 0;   // set on subsequent
}

//---------------------------------------------------------------

void RSLK_Reset(void){
    //Initialise modules used e.g. Reflectance Sensor, Bump Switch, Motor, Tachometer etc
    // ... ...
    DisableInterrupts();
    Clock_Init48MHz();  // makes SMCLK=12 MHz
    //SysTick_Init(48000,2);  // set up SysTick for 1000 Hz interrupts
    Motor_Init();
    Motor_Stop();
    LaunchPad_Init();
    //Bump_Init();
    //Bumper_Init();
    BumpInt_Init(&HandleBump);
    Reflectance_Init();
    IRSensor_Init();
    Tachometer_Init();
    EUSCIA0_Init();     // initialize UART
    EnableInterrupts();
}



// RSLK Self-Test
int main(void) {
    uint32_t cmd=0xDEAD, menu=0;
    uint32_t choice = 0xDEAD, repeat = 1;
    uint8_t reflectanceData;
    uint32_t delay = 3000; //For Motor Test, set as 3 seconds for each movement
    uint32_t pwm = 3000;
    uint32_t i = 0, n, count = 0;
    volatile uint8_t bumpTemp;

    DisableInterrupts();
    Clock_Init48MHz();  // makes SMCLK=12 MHz
    //SysTick_Init(48000,2);  // set up SysTick for 1000 Hz interrupts
    Motor_Init();
    Motor_Stop();
    //Motor_InitSimple();
    //Motor_StopSimple();
    LaunchPad_Init();
    //Bump_Init();
    //Bumper_Init();
    BumpInt_Init(&HandleBump);
    Reflectance_Init();
    IRSensor_Init();
    Tachometer_Init();
    EUSCIA0_Init();     // initialize UART
    EnableInterrupts();

  while(1){                     // Loop forever
      // write this as part of Lab 5
      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("RSLK Testing"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[0] RSLK Reset"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[1] Motor Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[2] IR Sensor Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[3] Bumper Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[4] Reflectance Sensor Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[5] Tachometer Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[6] Search object and trace"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

      EUSCIA0_OutString("CMD: ");
      cmd=EUSCIA0_InUDec();
      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

      switch(cmd){
          case 0: //RSLK Reset
              RSLK_Reset();
              EUSCIA0_OutString("Reset Completed!!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              menu =1;
              cmd=0xDEAD;
              break;

          case 1: //Motor Test
              repeat = 1;
              delay = 3000;
              pwm = 3000;
              EUSCIA0_OutString("Please enter pwm (from 0 to 10000, the higher this value is, the faster the robot moves): "); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              pwm = EUSCIA0_InUDec();
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

              EUSCIA0_OutString("Please enter time duration (in seconds): "); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              delay = EUSCIA0_InUDec();
              delay *= 1000;
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

              while(repeat){
                  EUSCIA0_OutString("What motion would you like to test?"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  EUSCIA0_OutString("[0] Forward"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  EUSCIA0_OutString("[1] Backward"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  EUSCIA0_OutString("[2] Rotate Left"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  EUSCIA0_OutString("[3] Rotate Right"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  EUSCIA0_OutString("[4] Exit"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  EUSCIA0_OutString("Enter your choice: ");
                  choice=EUSCIA0_InUDec();
                  EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  switch(choice){
                  case 0 : //Move Forward
                      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("Robot Moving Forward!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      Motor_Forward(pwm, pwm);
                      Clock_Delay1ms(delay);
                      Motor_Stop();

                      break;
                  case 1: //Move Backward
                      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("Robot Moving Backward!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      Motor_Backward(pwm,pwm);
                      Clock_Delay1ms(delay);
                      Motor_Stop();

                      break;
                  case 2://Turn Left
                      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("Robot Rotating Left!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      Motor_Left(pwm, pwm);
                      Clock_Delay1ms(delay);
                      Motor_Stop();

                      break;
                  case 3://Turn right
                      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("Robot Rotating Right!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      Motor_Right(pwm, pwm);//Only left wheel is activated, left wheel turns backward to turn left
                      Clock_Delay1ms(delay); //Delay same amount of time as movement
                      Motor_Stop();
                  break;
                  default:
                      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("Exiting Motor Test!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      repeat = 0;

                  }
                  EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              }
              menu =1;
              cmd=0xDEAD;
              break;

          case 2://IR Sensor Test
              EUSCIA0_OutString("IR Sensor Test, 30 readings will be displayed."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              TimerA1_Init(&SensorRead_ISR,250);    // 2000 Hz sampling
              UART0_OutString("GP2Y0A21YK0F test\nValvano Oct 2017\nConnect analog signals to P9.0,P4.1,P9.1\n");
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              i = 0;
              while(i < 30){
                  for(n=0; n<2000; n++){
                      while(ADCflag == 0){};
                      ADCflag = 0; // show every 2000th point
                  }
                  UART0_OutUDec5(LeftConvert(nl));UART0_OutString(" cm,");
                  UART0_OutUDec5(CenterConvert(nc));UART0_OutString(" cm,");
                  UART0_OutUDec5(RightConvert(nr));UART0_OutString(" cm\r\n");
                  i++;
              }
              menu =1;
              cmd=0xDEAD;
              break;

          case 3://Bumper Test
              bumpTemp = bumpData = 0x3F;
              bumpFlag = 0;
              count = 0;
              i = 0;

              EUSCIA0_OutString("Bumper Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Bumper Test using Edge Interrupt selected"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Please press bump switch you wish to test..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              while(1){ // polling
                  WaitForInterrupt();
                  if(bumpFlag == 1) {
                      bumpTemp = bumpData;
                      break;
                  }
              }
              EUSCIA0_OutString("Bumper has been pressed..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Displaying Results"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Each bit corresponds to the bump number. E.g. Bit 1 refers to bump 1"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Bump Switch Readings in hexadecimal format: 0x"); EUSCIA0_OutUHex(bumpTemp);EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF); //Line to display bump data
              EUSCIA0_OutString("Now Displaying which corresponding bump was pressed:"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              for (i = 0; i<6; i++){
                  if(bumpTemp % 2 == 0){
                      EUSCIA0_OutString("Bump Number ");
                      EUSCIA0_OutUDec(i);
                      EUSCIA0_OutString(" was pressed");EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  }
                  bumpTemp = bumpTemp>>1;
              }
              bumpData = 0x3F;
              bumpFlag = 0;
              menu=1;
              cmd=0xDEAD;
              break;

          case 4: //Reflectance sensor test
              count = 0;
              reflectanceData = 0;
              EUSCIA0_OutString("Reflectance Sensor Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Displaying 30 results with 1 sec interval"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              while(count < 30){
                  reflectanceData = Reflectance_Read(1000);
                  for(i = 0; i < 8; i++){
                      EUSCIA0_OutUDec(reflectanceData%2);
                      reflectanceData = reflectanceData >> 1;
                  }
                  EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  Clock_Delay1ms(1000);
                  count ++;
                }
              menu =1;
              cmd=0xDEAD;
              break;

          case 5: //Tachometer Test
              bumpFlag = 0; //Exit program using bum switch as interrupt
              EUSCIA0_OutString("Tachometer test:"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Periods will be printed every 1 seconds"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Push one of the side buttons to start!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Press bump switch to exit test!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              TimerA1_Init(&toggle_GPIO,10);    // 50Khz sampling
              TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
              TimedPause(500);
              Motor_Forward(3000,3000); // 50%
              EnableInterrupts();
              while(1){
                  WaitForInterrupt();
                  UART0_OutString("Period0 = ");UART0_OutUDec5(Period0);UART0_OutString(" Period2 = ");UART0_OutUDec5(Period2);UART0_OutString(" \r\n");
                  if(bumpFlag == 1) break;
                  Clock_Delay1ms(1000);
              }
              Motor_Stop();
              menu =1;
              cmd=0xDEAD;
              break;

          case 6:
              // Additional Function: Find an object in a planet and go there
              bumpFlag = 0;
              pwm = 2000;
              EUSCIA0_OutString("Put the car on table and put one object around."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Stop movement by pressing any launch pad buttons"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              TimerA1_Init(&SensorRead_ISR,250);
              EnableInterrupts();
              while(1){
                  Motor_Forward(pwm, pwm);
                  if(RightConvert(nr) < LeftConvert(nl) && RightConvert(nr) < CenterConvert(nc)){ // Object is on the right
                      Motor_Stop();
                      Motor_Right(pwm, pwm);
                      Clock_Delay1ms(500); //Turn for 0.5 seconds
                      Motor_Stop();
                      Motor_Forward(pwm,pwm);
                  }
                  else if(CenterConvert(nc) < LeftConvert(nl) && CenterConvert(nc) < RightConvert(nr)){ // Object is in front
                      Motor_Forward(pwm,pwm);
                      Clock_Delay1ms(500);
                      Motor_Stop();
                  }
                  else if(LeftConvert(nl) < RightConvert(nr) && LeftConvert(nl) < CenterConvert(nc)){ // Object is on the left
                      Motor_Stop();
                      Motor_Left(pwm, pwm);
                      Clock_Delay1ms(500); //Turn for 0.5 seconds
                      Motor_Stop();
                      Motor_Forward(pwm,pwm);
                  }


                  UART0_OutUDec5(LeftConvert(nl));UART0_OutString(" cm,");
                  UART0_OutUDec5(CenterConvert(nc));UART0_OutString(" cm,");
                  UART0_OutUDec5(RightConvert(nr));UART0_OutString(" cm\r\n");
                  if(bumpFlag == 1) break;
              }
              Motor_Stop();
              menu =1;
              cmd=0xDEAD;
              break;

          default:
              menu=1;
              break;
      }

      if(!menu)Clock_Delay1ms(3000);
      else{
          menu=0;
      }

      // ....
      // ....
  }
}
