	PRESERVE8
	AREA MyCode, CODE, READWRITE    ;specifies memory
	EXPORT asmmain                  ;declares a symbol that can be used by the linker to resolve symbol references in separate object and library files.
	EXTERN lptmrCounter
	import my_print


LPTMR0_CSR EQU 0x40040000  	;CSR register address
LPTMR0_PSR EQU 0x40040004  	;PSR register address
LPTMR0_CMR EQU 0x40040008  	;CMR register address
LPTMR0_CNR EQU 0x4004000C  	;CNR register address
SIM_SCGC5  EQU 0x40048038   ; This enables the lptmr to enable interrupts
NVIC_Value EQU 0x00200000
NVIC_Addr  EQU 0xe000e108

asmmain

	LDR r0, =SIM_SCGC5 ; enable the lptmr
	MOV r1, #1
	LDR r2, [r0]
	ORR r2, r2, r1 ; oring setting last bit to one
	STR r2, [r0] 
	
	;enable NVIC
	LDR r2, =NVIC_Value
	LDR r1, =NVIC_Addr
	STR r2, [r1]
	
	LDR r0, =LPTMR0_CSR
	MOV r1, #0x40 ; setting 6th bit to one
	STR r1, [r0]
	
	
	ADD r0, #0x4 ; increasing the r0 with 4 byte
	MOV r1, #0x5 ; prescale off and timer mode on
	STR r1, [r0]
	
	
	
	ADD r0, #0x4
	MOV r1, #0x1338 ; setting timer to interept every 5 seconds 
	STR r1, [r0]
	
	SUB r0, #0x8
	MOV r1, #0x1
	
	LDR r2, [r0]
	ORR r2, r2, r1
	
	STR r2, [r0]
	MOV r10, #0
	MOV r7, #0
	
	LDR r5, =lptmrCounter
	
counter_loop
	B counter_loop
	BX lr

	END