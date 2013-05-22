ehello: .data     		  7   ,   -57,17   ,		9
.data     		  7   ,   -57fr,17   ,		9
NUMBERS: .data 		  7   ,   -57,17   ,		9
STR: .string   		     "abc def"
       	  .string    			     "abc def"
	"
loop: mov/0			    #-3			   ,r1
movi/0    #-3		,	 r1
mov 			    #-3,r1
mov/      #-3,r2
loop: sub/0    #1   , r1
mov   #-3, r1
loop: sub/0    r2   , r1
loop: mov/0   r2Label, r1
loop: jmp/1/0/0 END

