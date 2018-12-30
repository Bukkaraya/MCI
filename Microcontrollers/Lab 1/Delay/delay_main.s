	PRESERVE8
	AREA MyCode, CODE, READONLY
	EXPORT asmmain
	
asmmain
	import my_print
	 
	LDR r0, =start  ;loads address of start 
	BL my_print		;branch and link to my_print fn
	
	MOV r10, #6		;load r10 with 6
	MOV r11, #0		;load r11 with 0
	
	LDR r7, =num	;loads address of num
loop2
		BL delay_10 ; Delay for 10s
		
		MOV r0, r7 	;loads r0 with r7	
		LDRB r3, [r0]	;load register byte 
		ADD r3, #1		;increment by 1
		STRB r3, [r0]	;store reg byte
		
		BL my_print		;branch and link to my_print fn
		
		ADD r11, #0x1	;increment r11
		CMP r10, r11	;compare to final value
		BNE loop2		;looping

	LDR r0, =done		;loads with done address
	BL my_print			;branch and link to my_print fn

delay_10			
	LDR r0, =0xBEBC200 	;200e6 instructions for the delay
	MOV r1, #0
	
loop
		ADD r1, #0x1 	;increment by 1
		CMP r1, r0 		;compare to 0
		BNE loop		;looping
	BX lr				;branch back to lr
	
	ALIGN
	AREA MyData, DATA, READWRITE
debug DCB "Hello", 0
done  DCB "Done!", 0
start DCB "Start", 0
num	  DCB "00", 0
	END