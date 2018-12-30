/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
#include <stdlib.h>
// SDK Included Files
#include "board.h"
#include "fsl_pmc_hal.h"
#include "fsl_adc16_driver.h"
#include "fsl_debug_console.h"
#include "gpio_pins.h"
#include "fsl_pit_driver.h"
#include <MK60D10.H>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief These values are used to get the temperature. DO NOT MODIFY
 * The method used in this demo to calculate temperature of chip is mapped to
 * Temperature Sensor for the HCS08 Microcontroller Family document (Document Number: AN3031)
 */
#define ADCR_VDD                (65535U)    // Maximum value when use 16b resolution
#define V_BG                    (1000U)     // BANDGAP voltage in mV (trim to 1.0V)
#define V_TEMP25                (716U)      // Typical converted value at 25 oC in mV
#define M                       (1620U)     // Typical slope:uV/oC
#define STANDARD_TEMP           (25)

#define ADC16_INSTANCE                (0)   // ADC instacne
#define ADC16_TEMPERATURE_CHN         (kAdc16Chn26) // Temperature Sensor Channel
#define ADC16_BANDGAP_CHN             (kAdc16Chn27) // ADC channel of BANDGAP
#define ADC16_CHN_GROUP               (0)   // ADC group configuration selection

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
int32_t ADC16_Measure(void);
void calibrateParams(void);
int32_t get_current_temperature(uint32_t adcValue);

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
uint32_t adcValue = 0;               // ADC value
uint32_t adcrTemp25 = 0;             // Calibrated ADCR_TEMP25
uint32_t adcr100m = 0;               // calibrated conversion value of 100mV
adc16_converter_config_t adcUserConfig;   // structure for user config
///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Measures internal temperature of chip.
 *
 * This function used the input of user as trigger to start the measurement.
 * When user press any key, the conversion will begin, then print
 * converted value and current temperature of the chip.
 */
 
 
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
uint32_t get_segment_value(int input){
	uint32_t segment_bits = 0;
	
	switch(input){
		case 0:
			segment_bits = 0xc0;
			break;
		case 1:
			segment_bits = 0xf9;
			break;
		case 2:
			segment_bits = 0x24;
			break;
		case 3:
			segment_bits = 0x30;
			break;
		case 4:
			segment_bits = 0x19;
			break;
		case 5:
			segment_bits = 0x92;
		  break;
		case 6:
			segment_bits = 0x02;
			break;
		case 7:
			segment_bits = 0xf8;
			break;
		case 8:
			segment_bits = 0x00;
			break;
		case 9:
			segment_bits = 0x98;
			break;
		default:
			segment_bits = 0x00;
			break;
	}
	
	return segment_bits;
	
}

__INLINE static void set_digit(int digit, bool decimal_on){
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
 
 
int main(void)
{
    // Init hardware
    hardware_init();
		PORTA->PCR[7] = 0x0;
    // Initialization ADC for
    // 12bit resolution.
    // interrupt mode and hw trigger disabled,
    // normal convert speed, VREFH/L as reference,
    // disable continuous convert mode.
    ADC16_DRV_StructInitUserConfigDefault(&adcUserConfig);
    // Use 16bit resolution if enable.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION >= 16)
    adcUserConfig.resolution = kAdc16ResolutionBitOf16;
#endif

#if (  defined(FRDM_KL43Z)   /* CPU_MKL43Z256VLH4 */ \
    || defined(TWR_KL43Z48M) /* CPU_MKL43Z256VLH4 */ \
    || defined(FRDM_KL27Z)   /* CPU_MKL27Z64VLH4  */ \
    )
    adcUserConfig.refVoltSrc = kAdc16RefVoltSrcOfValt;
#endif
    ADC16_DRV_Init(ADC16_INSTANCE, &adcUserConfig);
		
	bool is_on = false;
	uint32_t current_digit = 0;
	PIT_DRV_Init(BOARD_PIT_INSTANCE, false);
	PIT_DRV_InitChannel(BOARD_PIT_INSTANCE, 0, &chn0Confg);
	
	
	bool decimals[] = {false, false, true};

	hardware_init();
	GPIO_Config();
	int number[3];
	
	

	// Start Timer
	PIT_DRV_StartTimer(BOARD_PIT_INSTANCE, 0);
	uint32_t remainder;
	uint32_t voltage;
	
	
	while(true){
			voltage = ADC16_Measure();
		
		number[0] = (voltage % 10);
		number[1] = (voltage %100 - number[0])/10;
		number[2] = (voltage % 1000 - number[0] -number[1]*10) /100;
	
	
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

/*!
 * @brief Gets current temperature of chip.
 *
 * This function gets convertion value, converted temperature and print them to terminal.
 */
int32_t ADC16_Measure(void)
{
    adc16_chn_config_t chnConfig;

    // Configure the conversion channel
    // differential and interrupt mode disable.
    chnConfig.chnIdx     = (adc16_chn_t)kAdc16Chn10;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    chnConfig.diffConvEnable = false;
#endif
    chnConfig.convCompletedIntEnable  = false;

    // Software trigger the conversion.
    ADC16_DRV_ConfigConvChn(ADC16_INSTANCE, ADC16_CHN_GROUP, &chnConfig);

    // Wait for the conversion to be done.
    ADC16_DRV_WaitConvDone(ADC16_INSTANCE, ADC16_CHN_GROUP);

    // Fetch the conversion value.
    adcValue = ADC16_DRV_GetConvValueRAW(ADC16_INSTANCE, ADC16_CHN_GROUP);

    // Calculates adcValue in 16bit resolution
    // from 12bit resolution in order to convert to temperature.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION < 16)
    adcValue = adcValue << 4;
#endif
    //PRINTF("\r\n Temperature %d", get_current_temperature(adcValue));

    // Pause the conversion.
    ADC16_DRV_PauseConv(ADC16_INSTANCE, ADC16_CHN_GROUP);
		
		printf("%ld ", adcValue);
		
		return ((adcValue  * 330) / 65535);

}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////