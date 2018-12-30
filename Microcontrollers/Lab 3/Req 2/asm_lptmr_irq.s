	AREA LPTMRint, CODE, READONLY
	EXPORT asm_lptmr_irq
	EXTERN lptmrCounter 	;Import the lptmrCounter Variable from C file
LPTMR0_IRQHandler EQU asm_lptmr_irq+1 ; Figure this out
	EXPORT LPTMR0_IRQHandler

asm_lptmr_irq
	PUSH {lr} ; Push lr to stack
	
	; Load 4004_0000 to r2
	; Which is the CSR address for LPTMR
	LDR r0, =LPTMR_Addr 
	LDR r2, [r0]
	
	; Multiply by 32 to get proper byte offset
	LSL r2, r2, #5
	
	; Load bit band base address
	; Value 4200_0000
	LDR r0, =Aliased_base
	LDR r3, [r0]
	
	; Add the bit band base address to the byte offset
	ADD r2, r2, r3
	
	; Multiply bit offset by 4 and then add to r2 to get bit word address
	MOV r3, #7
	ADD r2, r2, r3, LSL #2
	
	; Move 1 to TCF to reset the LPTMR
	MOV r1, #0x1
	STR r1, [r2]
	
	; Increase lptmr by 1
	LDR r1, =lptmrCounter
	LDR r2, [r1]
	ADD r2, #1
	STR r2, [r1]
	
	; Return back to main
	POP {PC}
	
	
	ALIGN
	AREA MyData, DATA, READWRITE
LPTMR_Addr DCD 0x40040000
Aliased_base DCD 0x42000000
END