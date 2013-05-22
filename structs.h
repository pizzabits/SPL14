#include <stdbool.h>
#include "utils.h"
#include "general.h"

struct Label {
	char 	Name[MAX_LABEL_LENGTH];
	size_t 	Value;				/* if data, this equals DC, if code - IC */
	BYTE 	DataOffset;			/* offset from Value in the data array */
} label_type;

struct WordView {
	size_t Comb : 2; 			/* if type indicates 1 then:
								   0 - applies to left half of both of the operands.
								   1 - applies to left half of source operand,  right half of destination operand.
								   2 - applies to right half of source operand, left half of destination operand.
								   3 - applies to right half of both of the operands.  */
	size_t DestReg : 3; 	   	/* r0 , r1, ..., r7 */
	size_t DestAddrType : 2; 	/* 0 - immediate number
								   1 - dereference a label's address
								   2 - addressing using index
								   3 - direct register */
	size_t SrcReg : 3; 	    	/* r0 , r1, ..., r7 */
	size_t SrcAddrType : 2;  	/* analogous to DestAddrType */
	size_t OpcodeNumber : 4;	/* number between 0 to OPCODES */
	size_t Type : 1;		    /* 0 indicates that the operation will apply to all bits,
								   1 indicates that the operation will apply to halves of the operands. */
	size_t Unused : 3;	    	/* Unused bits. might be since our pseudo CPU aligns to 20 bits wide words. */
} word_view_type;

struct Word {
	size_t Bits : BITS_PER_WORD;
} word_type;

enum CodeTypes { ABSOLUTE, RELOCATABLE, EXTERNAL };

typedef struct {
	struct Word Word;			/* machine coded instruction or data  */
	bool UnresolvedLabel;		/* if set then word defines an offset in an unresolved labels table */
	enum CodeTypes Type;
} Code;

typedef struct {
	BYTE OpcodeCategory;		/* number of operands expected */
	char *Name;					/* opcode name */
	BYTE Number;				/* opcode number */
	BYTE SrcAllow0;				/* a set bit means that the opcode supports an addressing type for an operand */
	BYTE SrcAllow1;
	BYTE SrcAllow2;
	BYTE SrcAllow3;
	BYTE DestAllow0;
	BYTE DestAllow1;
	BYTE DestAllow2;
	BYTE DestAllow3;
} Instruction;
