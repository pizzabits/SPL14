;	some comment
			.entry		STRADD
			.entry		MAIN
			.extern 	REVERSE
			.extern 	PRTSTR
			.extern 	COUNT
	STRADD:		.data		0
	STR:		.string		"abcdef"
	LASTCHAR:	.data		0
	LEN:		.data		0
	K:		.data		0


;	another comment
	MAIN:		lea/1/1/1	STR{r9},STRADD
			jsr/0		COUNT, r2
			jsr/0		PRTSTR
			mov/1/1/0	STRADD{-5}, LASTCHAR{52}
			mov/1/1/1	STR{7}, r7
			add/0		COUNT{K},r3
			dec/1/1/1	LASd3{K}
			inc/0		K543Fd#
			jsr/0		REVERSE
			jsr/0		PRTSTR
			stop/0
