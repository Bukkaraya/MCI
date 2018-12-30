#include <MK60D10.H>
#include "board.h"
#include "gpio_pins.h"
#include "fsl_pit_driver.h"
#include "fsl_debug_console.h"

#define BOARD_PIT_INSTANCE  0

#define LED_NUM     3                   /* Number of user LEDs                */
const uint32_t led_mask[] = {1UL << 11, 1UL << 28, 1UL << 29};

volatile uint32_t pitCounter = 0;
uint32_t currentCounter = 0;

pit_user_config_t chn0Confg = {
	.isInterruptEnabled = true,
	.periodUs = 5000000u
};

// PIT Handler
#if FSL_FEATURE_PIT_HAS_SHARED_IRQ_HANDLER
void PIT_IRQHandler(void)
{
    uint32_t i;
    for(i=0; i < 1; i++)  // We only support two channels for pit example
    {
        if (PIT_HAL_IsIntPending(g_pitBase[0], i))
        {
            /* Clear interrupt flag.*/
            PIT_HAL_ClearIntFlag(g_pitBase[0], i);
            pitCounter++;
        }
    }
}
#else

#if (FSL_FEATURE_PIT_TIMER_COUNT > 0U)
void PIT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    PIT_HAL_ClearIntFlag(g_pitBase[0], 0U);
    pitCounter++;
}
#endif

#endif

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
	uint32_t currentLed = 0;
	bool isOn = false;
	PIT_DRV_Init(BOARD_PIT_INSTANCE, false);
	PIT_DRV_InitChannel(BOARD_PIT_INSTANCE, 0, &chn0Confg);
	
	
	
	hardware_init();
	LED_Config();
	
	PRINTF("Starting Timer...\r\n");
  PIT_DRV_StartTimer(BOARD_PIT_INSTANCE, 0);
 
  while(1) {
		if(currentCounter != pitCounter){
			if(isOn){
				PRINTF("Led turning on.\r\n");
				LED_Off(currentLed);
				currentLed = (currentLed + 1) % 3;
				isOn = false;
			}else{
				PRINTF("Led turning off.\r\n");
				LED_On(currentLed);
				isOn = true;
			}
			currentCounter = pitCounter;
		}

  }
  
}

