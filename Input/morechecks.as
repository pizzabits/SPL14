MAIN:		mov/0		LENGTH, r1
		lea/1/1/1	STR{LENGTH}, r4
LOOP:		jmp/1/0/0	END
		prn/1/1/0	STR{r3}
		sub/0		#1, r1
		inc/0		r0
		mov/0		r3,STR{7}
		bne/0		LOOP
END:		stop/0

STR:		.string 	"abcdef"
LENGTH:		.data 		6
K:		.data 		2

; some more definitions
    
    		.data 		1
    		.data 		+1 
    		.data 		-1 
    		.data 		1, 2 
    		.data 		1 ,2  
    		.data 		1 ,   2
    		.data 		1 , -2, +13, 12352534 ,00
    
		.string     	"asd" 
		.string     	"asbd asd asdkl qwe"    
		.string     	"asd ' asdasd ' asd"
; bad quotation commas
		.string 	"asdd "asdasd"asdadd "
		.string 	"asdd \r\0\rdasd"asdadd "

; some more commands

		mov/0   	r3, r4

Z:  		mov/0 		STR, r1
AS: 		mov/1/0/0 	R1,r1
R1: 		sub/1/1/1 	r1    ,r2
    		stop/0
    		bne/1/1/0 	LOOP       
    
    
; correct addressing

		cmp/0 		#1, #2
		cmp/0 		AS{Z}, R1{K}
		prn/0 		#1
		jsr/0   	END
