	PRESERVE8                       ;specify that the current file requires or preserves eight-byte alignment of the stack.
	AREA MyCode, CODE, READONLY     ;specifies memory
	EXPORT asmmain                  ;declares a symbol that can be used by the linker to resolve symbol references in separate object and library files.
	
asmmain             
	import my_print                 
	LDR r0, =srcstr                 ;Load srcrstr address in r0
	BL my_print                     ;branch
	
	LDR r0, =dststr                 ;Load dstrstr address in r0
	BL my_print                     ;branch     
	
	LDR r1, =srcstr                 ;Load srcstr address in r1
	LDR r0, =dststr                 ;Load dstrstr address in r0
	BL strcopy                      ;branch
	

strcopy
	LDRB r3, [r1], #1               ;load then post-increment
	STRB r3, [r0], #1               ;store then post-increment
	CMP r3, #0                      ;compare to 0
	BNE strcopy                     ;branch not equal
	LDR r0, =srcstr                 ;Load srcstr address in r0
	BL my_print                     ;branch
	    
	LDR r0, =dststr                 ;Load dststr address in r0
	BL my_print                     ;branch
stop	B stop                      ;end code    

	ALIGN                           ;aligns the current location to a specified boundary by padding with zeros or NOP instructions.
	AREA MyData, DATA, READWRITE    ;specifies memory
srcstr DCB "String source", 0       ;create string in memory
dststr DCB "String destination", 0  ;create string in memory
	END                                 