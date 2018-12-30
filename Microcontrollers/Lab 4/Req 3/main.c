#include <MK60D10.H>
#include "board.h"
#include "gpio_pins.h"
#include "fsl_pit_driver.h"
#include "fsl_debug_console.h"

#define BOARD_PIT_INSTANCE  0

#define PIN_NUM     11                 /* Number of user LEDs                */
const uint32_t pin_mask[] = {1UL, 1UL << 1, 1UL << 2, 1UL << 3,
	1UL << 4, 1UL << 5, 1UL << 6, 1UL << 7, 1UL << 8, 1UL << 9, 1UL << 10};
volatile uint32_t pitCounter = 0;
uint32_t currentCounter = 0;

// PIT Timer Code

pit_user_config_t chn0Confg = {
	.isInterruptEnabled = true,
	.periodUs = 2500u
};

// PIT Handler

#if (FSL_FEATURE_PIT_TIMER_COUNT > 0U)
void PIT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    PIT_HAL_ClearIntFlag(g_pitBase[0], 0U);
    pitCounter++;
}
#endif


// End Pit Timer Code


// Initialize Pins
__INLINE static void GPIO_Config(void){
	// Enable Clock to Port C using SIM
	SIM->SCGC5 |= (1U << 11);
	
	// Enable PTC0 through 10 to be a GPIO pin
	// This is done by setting 8th bit to logic 1 
	// in the corresponding PCR register
	for(int i = 0; i < 11; i++){
		PORTC->PCR[i] = (1U << 8);
	}
	
	// Set the pins to GPIO output by
	// Setting the corresponding pin location 
	// to logic 1
	uint32_t pddr_reg = 0U;
	for(int i = 0; i < PIN_NUM; i++){
		pddr_reg |= pin_mask[i];
	}
	
	
	PTC->PDDR = pddr_reg;
}

// End GPIO Config

// Turn digit n on
__INLINE static void digit_on(uint32_t digit_num){
	PTC->PSOR = pin_mask[digit_num + 8];
}

__INLINE static void clear_port(void){
	PTC->PCOR = 0xffffffff;
}


// 7 Segment LUT
uint32_t get_segment_value(char input){
	uint32_t segment_bits = 0;
	
	switch(input){
		case '0':
			segment_bits = 0xc0;
			break;
		case '1':
			segment_bits = 0xf9;
			break;
		case '2':
			segment_bits = 0x24;
			break;
		case '3':
			segment_bits = 0x30;
			break;
		case '4':
			segment_bits = 0x19;
			break;
		case '5':
			segment_bits = 0x92;
		  break;
		case '6':
			segment_bits = 0x02;
			break;
		case '7':
			segment_bits = 0xf8;
			break;
		case '8':
			segment_bits = 0x00;
			break;
		case '9':
			segment_bits = 0x98;
			break;
		default:
			segment_bits = 0x00;
			break;
	}
	
	return segment_bits;
	
}

__INLINE static void set_digit(char digit, bool decimal_on){
		uint32_t segment_bits = get_segment_value(digit);
		if(decimal_on){
			uint32_t temp = (0xff >> 1);
			segment_bits &= temp;
		}else{
			uint32_t temp = (1U << 7);
			segment_bits |= temp;
		}
		
		PTC->PSOR = segment_bits;
}



/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	bool is_on = false;
	uint32_t current_digit = 0;
	PIT_DRV_Init(BOARD_PIT_INSTANCE, false);
	PIT_DRV_InitChannel(BOARD_PIT_INSTANCE, 0, &chn0Confg);
	
	char number[] = "542";
	bool decimals[] = {false, false, true};

	hardware_init();
	GPIO_Config();

	// Start Timer
	PIT_DRV_StartTimer(BOARD_PIT_INSTANCE, 0);
	while(true){
		if(currentCounter != pitCounter){
			if(is_on){
				is_on = false;
				clear_port();
				current_digit = (current_digit + 1) % 3;
			}else{
				is_on = true;
				set_digit(number[current_digit], decimals[current_digit]);
				digit_on(current_digit);
			}
			
			currentCounter = pitCounter;
		}
	}
  
}