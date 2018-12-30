	PRESERVE8
	AREA MyCode, CODE, READONLY
	EXPORT asmmain
	
asmmain
	import my_print
	import print_int
	import putchar
	import getchar
	
	LDR r10, =input_str
	BL f_gets
	
	LDR r10, =input_str
	BL str_tok
	
	MOV r6, #'+'
	MOV r1, r12
	; Check for negative number
	; If negative then get two's complement
	LDR r8, =result
	MOV r2, r12
	AND r2, #0x80000000
	CMP r2, #0x80000000
	BNE convert_to_ascii
	MVN r1, r12
	ADD r1, #1
	MOV r6, #'-'

convert_to_ascii
	BL bin_to_ascii
	
	LDR r0, =new_line
	BL my_print
	
	MOV r0, r6
	BL putchar
	
	LDR r0, =result
	BL my_print
	
	
	
stop	B stop


; BEGIN STRTOK FUNCTION
; String tokenizer function which is clearly better than the
; inbuilt strtok
str_tok
	LDR r9, =temp
	MOV r5, lr
	MOV r7, #0
str_tok_loop1
	LDRB r7, [r10], #1
	
	CMP r7, #' '
	BEQ str_tok_conv_push
	CMP r7, #'+'
	BEQ add_num
	CMP r7, #'-'
	BEQ sub_num
	CMP r7, #'*'
	BEQ mul_num
	CMP r7, #'/'
	BEQ div_num
	CMP r7, #'='
	BEQ str_tok_done
	
	
	STRB r7, [r9], #1
	B str_tok_loop1
str_tok_conv_push
	MOV r0, #0
	STRB r0, [r9], #1
	LDR r1, =temp
	BL ascii_to_bin
	PUSH {r3}
	LDR r9, =temp
	B str_tok_loop1

str_tok_done
	POP {r12}
	MOV lr, r5
	BX lr

add_num
	POP {r11, r12}
	ADD r11, r11, r12
	PUSH {r11}
	
	B str_tok_black_magic
	
sub_num
	POP {r11, r12}
	SUB r11, r12, r11
	PUSH {r11}
	
	B str_tok_black_magic

mul_num
	POP {r11, r12}
	MUL r11, r12, r11
	PUSH {r11}
	
	B str_tok_black_magic

div_num
	POP {r11, r12}
	MOV r1, r11
	MOV r2, r12
	BL div
	PUSH {r3}
	
	B str_tok_black_magic

; Not black magic
; Please don't dock marks
str_tok_black_magic
	LDRB r0, [r10], #0
	CMP r0, #' '
	BNE str_tok_loop1
	ADD r10, #1
	LDR r9, =temp
		
	B str_tok_loop1

; END FUNCTION

; Function to get string from user
; String is stored in the address of string
; specified in the R10 register
; Destorys R8, R0, R5
; Address of String in R10
f_gets
	MOV r1, lr
	PUSH {r1}
f_gets_loop
	BL getchar
	CMP r0, #13
	BEQ f_gets_break
	STRB r0, [r10], #1
	BL putchar
	B f_gets_loop
f_gets_break
	MOV r5, #0
	STRB r5, [r10], #1
	POP {r1}
	MOV lr, r1
	BX lr

; Convert ASCII to Binary
; Address of String in R1
; Binary Number in R3
; R9 and R2 gets destroyed
ascii_to_bin
	MOV r3, #0; Store constant 0 in R3
	MOV r9, #10; Store constant 10 in R9
	LDRB r2, [r1], #1;Get input string and increment pointer
	CMP r2, #0;Check for end of string
	BEQ a_b_done
	SUB r2, #'0';Subtract 0 to convert from ascii to binary
	ADD r3, r2 ;add number to the ones position of output
a_b_loop
		LDRB r2, [r1], #1;Get input string and increment pointer
		CMP r2, #0;Check for end of string
		BEQ a_b_done
		MUL r3, r3, r9 ;multiply current output by 10
		SUB r2, #'0';Subtract 0 to convert from ascii to binary
		ADD r3, r2 ;add number to the ones position of output
		B a_b_loop
a_b_done
		BX lr 
		
		
	

; Convert Binary to ASCII
; Address of string in R8
; Number in R1
; Stores string in address at R8
; Destroys R0, R8
bin_to_ascii
	MOV r2, #10 ;Store constant 10 in R2
	MOV r9, lr ;copy return to R9
	MOV r0, #0 ;Store constant 10 in R0
	PUSH {r0} ;store null for end of string
b_a_loop
		BL div ;call divide function
		ADD r4, #'0' ;add 0 to get ascii representation
		MOV r1, r3 
		PUSH {r4} ;pushing character on stack
		CMP r1, r2
		BLT b_a_loop_done ;check for end character
		B b_a_loop
b_a_loop_done
	ADD r1, #'0' ;make first character ascii
	STRB r1, [r8], #1 ;storing string in memory and incrementing mem adress
b_a_str_loop
	POP {r0} ;popping character
	CMP r0, #0 ;check if no more characters
	BEQ b_a_done
	STRB r0, [r8], #1 ;storing string in memory and incrementing mem adress
	B b_a_str_loop
b_a_done
	MOV r0, #0 ;move constant null in r0
	STRB r0, [r8], #1 ;place null character at string
	MOV lr, r9 ;copy R9 to lr
	BX lr
		


; Division Function
; Input R1 - Dividend
; Input R2 - Divisor
; Quotient in R3
; Remainder in R4
; Destroys R1
div
	MOV r3, #0
	MOV r4, #0
div_loop
		sub r1, r1, r2
		add r3, #1
		cmp r1, r2
		BLT div_done
		B div_loop
div_done
	MOV r4, r1
	BX lr

	
	ALIGN
	AREA MyData, DATA, READWRITE
result SPACE 24
	ALIGN
input_str SPACE 20
	ALIGN
debug DCB "Hello", 0
	ALIGN
temp SPACE 24
	ALIGN
new_line DCB "\r\n", 0
	END