///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////

#define BOARD_MOTOR_FTM 0

// Standard C Included Files
#include <stdio.h>
#include <string.h>
// SDK Included Files
#include "board.h"
#include "fsl_ftm_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"



////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void delay(void){
	for(int i = 0; i < 50001; i++){}
}

/*!
 * @brief Use FTM in PWM mode
 *
 * This function use PWM to controll brightness of a LED.
 * LED is brighter and then dimmer, continuously.
 */
int main(void)
{
    bool brightnessUp = true;       /*! Indicate LED is brighter or dimmer*/
    ftm_user_config_t ftmInfo;
    // Configure ftm params with frequency 24kHZ
    ftm_pwm_param_t ftmParam = {
        .mode                   = kFtmEdgeAlignedPWM,
        .edgeMode               = kFtmLowTrue,
        .uFrequencyHZ           = 24000u,
        .uDutyCyclePercent      = 0,
        .uFirstEdgeDelayPercent = 0,
    };
    // Init hardware
    hardware_init();
		
		SIM_SCGC5 |= (1U << 11);
		SIM_SCGC6 |= (1U << 24);
		
		PORTC->PCR[1] |= (1U << 10) | (1U << 6);

    // Init OSA.
    OSA_Init();


#if defined (FRDM_K22F)
    LED1_EN;
    LED1_OFF;
#endif

    // Initialize FTM module,
    // configure for software trigger.
    memset(&ftmInfo, 0, sizeof(ftmInfo));
    ftmInfo.syncMethod = kFtmUseSoftwareTrig;
    FTM_DRV_Init(BOARD_MOTOR_FTM, &ftmInfo);
    FTM_DRV_SetClock(BOARD_MOTOR_FTM, kClock_source_FTM_SystemClk, kFtmDividedBy1);

    while(1)
    {
        // Start PWM mode with updated duty cycle.
        FTM_DRV_PwmStart(BOARD_MOTOR_FTM, &ftmParam, 0U);

        // Software trigger to update registers.
        FTM_HAL_SetSoftwareTriggerCmd(g_ftmBase[BOARD_MOTOR_FTM], true);
				
			
			  PRINTF("Enter your duty cycle (in percentage):\r\n");
				scanf("%d", &ftmParam.uDutyCyclePercent);
				PRINTF("\r\n");
				
        
        
				
    } // end while

}

/*******************************************************************************
 * EOF
 ******************************************************************************/

