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
// SDK Included Files
#include "fsl_lptmr_driver.h"
#include "board.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define LPTMR_INSTANCE     0U

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

volatile uint32_t lptmrCounter=0;

extern void asmmain(void);
extern void LPTMR0_IRQHandler(void);
///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////


void my_print(int d){
    printf("Interrupt No. %d\r\n", d);
}

void print_int(int d){
	printf("%d\r\n", d);
}


/*!
 * @brief The example uses LPTMR to generate interrupt each 1 second.
 *        When interrupt occurs, LED1 changes status & print to terminal
 */
int main (void)
{
		
		uint32_t currentCounter = 0;
    hardware_init();
		
		
	
		

    LED1_EN;

		asmmain();
		

}

/*******************************************************************************
 * EOF
 ******************************************************************************/