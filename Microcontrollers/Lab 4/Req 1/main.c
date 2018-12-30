/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: Freescale MK60X256VMD100 LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2010 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <MK60D10.H>

#define LED_NUM     3                   /* Number of user LEDs                */
const uint32_t led_mask[] = {1UL << 11, 1UL << 28, 1UL << 29};
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*------------------------------------------------------------------------------
  configer LED pins
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Config(void) {

  SIM->SCGC5    |= (1UL <<  9);        /* Enable Clock to Port A */ 
  PORTA->PCR[11] = (1UL <<  8);        /* Pin is GPIO */
  PORTA->PCR[28] = (1UL <<  8);        /* Pin is GPIO */
  PORTA->PCR[29] = (1UL <<  8);        /* Pin is GPIO */

  PTA->PDOR = (led_mask[0] | 
               led_mask[1] |
               led_mask[2] );          /* switch LEDs off  */
  PTA->PDDR = (led_mask[0] | 
               led_mask[1] |
               led_mask[2] );          /* enable Output */
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_On (uint32_t led) {

  PTA->PCOR   = led_mask[led];
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Off (uint32_t led) {

  PTA->PSOR   = led_mask[led];
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  int num     = -1; 
  int dir     =  1;

  SystemCoreClockUpdate();                      /* Get Core Clock Frequency */
  SysTick_Config(SystemCoreClock/1000);         /* Generate interrupt each 1 ms    */
  
  LED_Config();                             
 
  while(1) {
    /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...             */
    num += dir;
    if (num == LED_NUM) { dir = -1; num =  LED_NUM-1; } 
    else if   (num < 0) { dir =  1; num =  0;         }

    LED_On (num);
    Delay(250);
    LED_Off(num);
    Delay(250);
  }
  
}