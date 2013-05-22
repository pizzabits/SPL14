#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "general.h"
#include "structs.h"

#if _VERBOSE == 1
#define VERBOSE
#endif

extern unsigned int IC;
extern Code code[];
extern char *part;
extern struct WordView *view;
extern char extraWords;
extern const Instruction *inst;
extern struct WordView *view;
extern struct Word  *word;
extern bool isLabel;
extern bool localSyntaxError;
extern char line[];
extern char lineCopy[];

const Instruction *GetInstructionData(char *name)
{
	int i;
	static Instruction opcodes[OPCODES] = {
   /* Defines the design's operation codes and rules applied to them.
	* cat  name   #   src0   src1   src2   src3   dest0  dest1  dest2  dest3
	* ====================================================================== */
	{ TWO, "mov", 0,  true,  true,  true,  true,  false, true,  true,  true  },
	{ TWO, "cmp", 1,  true,  true,  true,  true,  true,  true,  true,  true  },
	{ TWO, "add", 2,  true,  true,  true,  true,  false, true,  true,  true  },
	{ TWO, "sub", 3,  true,  true,  true,  true,  false, true,  true,  true  },
	{ ONE, "not", 4,  false, false, false, false, false, true,  true,  true  },
	{ ONE, "clr", 5,  false, false, false, false, false, true,  true,  true  },
	{ TWO, "lea", 6,  false, true,  true,  true,  false, true,  true,  true  },
	{ ONE, "inc", 7,  false, false, false, false, false, true,  true,  true  },
	{ ONE, "dec", 8,  false, false, false, false, false, true,  true,  true  },
	{ ONE, "jmp", 9,  false, false, false, false, false, true,  true,  true  },
	{ ONE, "bne", 10, false, false, false, false, false, true,  true,  true  },
	{ ONE, "red", 11, false, false, false, false, false, true,  true,  true  },
	{ ONE, "prn", 12, false, false, false, false, true,  true,  true,  true  },
	{ ONE, "jsr", 13, false, false, false, false, false, true,  false, false },
	{ ZERO,"rts", 14, false, false, false, false, false, false, false, false },
	{ ZERO,"stop",15, false, false, false, false, false, false, false, false }
	};

	i = 0;
	while (i < OPCODES) {
		if (strcmp(opcodes[i].Name, name) == 0)
		{
			return &opcodes[i];
		}
		i++;
	}
	return (Instruction *) NULL;
}

const Instruction *ParseOpcode()
{
	int i;
	char *typeCharPosition;
	char opcode[OPCODE_MAX_LENGTH] = EMPTY_STRING;

	i = 0;
	typeCharPosition = strchr(part, OPCODE_ADDRESSING_CHAR); /* split by addressing character */
	if (typeCharPosition == NULL)
	{
		SYNTAX_ASSERT(false, OPCODE_MUST_TYPECHAR);
		return (inst = NULL);
	}

	/* copy the opcode until the finishing type char */
	while (part != typeCharPosition && i < OPCODE_MAX_LENGTH)
	{
		opcode[i] = *part;
		i++;
		part++;
	}
	inst = GetInstructionData(opcode);
	SYNTAX_ASSERT(inst != NULL, UNKNOWN_OPCODE);
	return inst;
}

/* Translate the code of the current part,
 * assuming that part addresses the first character in an operand. */
bool TranslateOperand(bool sourceOperand)
{
	if (ParseImmediateAddressing(sourceOperand, true, true, true))	/* immediate operand */
		return true;
	if (localSyntaxError)
		return false;
	if (ParseRegister(sourceOperand, true))							/* register operand */
		return true;
	if (ParseIndexAddressing(sourceOperand))						/* varied index operand */
		return true;
	if (ParseDirectAddressing(sourceOperand)) 						/* direct addressing with label */
		return true;
	return false; /* syntax error */
}

char *SkipParts(char *original, char *workspace, char count, const char *delimiters)
{
	char *split;

	strncpy(workspace, original, MAX_LINE_LENGTH);
	split = strtok(workspace, delimiters);
	while (count > ZERO)
	{
		if (split == NULL)
		{
			SYNTAX_ASSERT(false, UNABLE_SKIP);
			return NULL;
		}
		part = strtok(NULL, DELIMITERS_AND_COMMA);
		count--;
	}
	return part;
}

void WordViewToBitsWord(struct Word *word, struct WordView *view)
{
	word->Bits |= view->Type << 16;
	word->Bits |= view->OpcodeNumber << 12;
	word->Bits |= view->SrcAddrType << 10;
	word->Bits |= view->SrcReg << 7;
	word->Bits |= view->DestAddrType << 5;
	word->Bits |= view->DestReg << 2;
	word->Bits |= view->Comb;
}

bool ParseTypesAndCombinations()
{
	char *addressingParts;

	addressingParts = strtok(part, OPCODE_ADDRESSING);
	if (addressingParts == NULL || (*addressingParts != ZERO_CHAR && *addressingParts != ONE_CHAR))
	{
		SYNTAX_ASSERT(false, WRONG_TYPE);
		return false;
	}
	view->Type = *addressingParts - ZERO_CHAR;
	if (view->Type == ONE) /* then expecting 2 addressing properties */
	{
		/* get the first property of comb */
		addressingParts = strtok(NULL, OPCODE_ADDRESSING);
		if (addressingParts == NULL ||  (*addressingParts != ZERO_CHAR && *addressingParts != ONE_CHAR))
		{
			SYNTAX_ASSERT(false, WRONG_ADDR_COMB_1ST);
			return false;
		}
		view->Comb = *addressingParts - ZERO_CHAR;
		view->Comb <<= 1;
		/* get the second property of comb */
		addressingParts = strtok(NULL, OPCODE_ADDRESSING);
		if (addressingParts == NULL ||  (*addressingParts != ZERO_CHAR && *addressingParts != ONE_CHAR))
		{
			SYNTAX_ASSERT(false, WRONG_ADDR_COMB_2ND);
			return false;
		}
		view->Comb += *addressingParts - ZERO_CHAR;
	}
	/* verify that there aren't any more addressing options */
	addressingParts = strtok(NULL, OPCODE_ADDRESSING);
	if (addressingParts != NULL)
	{
		SYNTAX_ASSERT(false, TOO_MANY_ADDR_PROPS);
		return false;
	}
	view->OpcodeNumber = inst->Number; /* assign opcode number */
	return true;
}

bool TranslateInstruction()
{
	char skipCount;

	/*reset relevant counters */
	extraWords = 0;
	skipCount = isLabel ? ONE : ZERO;
	if (ParseOpcode() == NULL)
		return false;
	skipCount++;
	if (!ParseTypesAndCombinations())
		return false;
	part = SkipParts(line, lineCopy, skipCount, DELIMITERS_AND_COMMA);

	switch(inst->OpcodeCategory)
	{
		case ZERO:
		if (part != NULL)
		{
			SYNTAX_ASSERT(false, OPCODE_NOT_EXPECT_OPERANDS);
			return false;
		}
		break;
		case ONE:
		if (!TranslateOperand(false))  /* treat operand as target */
			return false;
		break;
		case TWO:
		{
			if (!TranslateOperand(true))
				return false;
			skipCount++;
			part = SkipParts(line, lineCopy,skipCount, DELIMITERS_AND_COMMA);
			if (!TranslateOperand(false))
				return false;
		}
		break;
	}

	/* verify that skipping the parts really exhausts the whole line */
	part = SkipParts(line, lineCopy, skipCount + 1, DELIMITERS_AND_COMMA);
	if (part != NULL)
	{
		SYNTAX_ASSERT(false, EXPECTED_EOL);
		return false;
	}
	WordViewToBitsWord(word, view);
	code[IC].Word.Bits = word->Bits;
	code[IC].Type = ABSOLUTE;
	code[IC].UnresolvedLabel = false;
	IC += 1 + extraWords; /* one word of code translation + amount of extra words */
	return true;
}
