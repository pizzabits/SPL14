#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "general.h"
#include "structs.h"
#include "utils.h"

#if _VERBOSE == 1
#define VERBOSE
#endif

extern const Instruction *GetInstructionData(char *name);
extern const Instruction *inst;

bool ValidateAddressingType(BYTE addressingType, bool sourceOperand)
{
	bool sourceFailed = false;
	bool targetFailed = false;

	switch (addressingType)
	{
		case ZERO:
		{
			if (sourceOperand && !inst->SrcAllow0)
			{
				sourceFailed = true;
			}
			else if (!sourceOperand && !inst->DestAllow0)
			{
				targetFailed = true;
			}
			break;
		}
		case ONE:
		{
			if (sourceOperand && !inst->SrcAllow1)
			{
				sourceFailed = true;
			}
			else if (!sourceOperand && !inst->DestAllow1)
			{
				targetFailed = true;
			}
			break;
		}
		case TWO:
		{
			if (sourceOperand && !inst->SrcAllow2)
			{
				sourceFailed = true;
			}
			else if (!sourceOperand && !inst->DestAllow2)
			{
				targetFailed = true;
			}
			break;
		}
		case THREE:
		{
			if (sourceOperand && !inst->SrcAllow3)
			{
				sourceFailed = true;
			}
			else if (!sourceOperand && !inst->DestAllow3)
			{
				targetFailed = true;
			}
			break;
		}
	}
	if (sourceFailed)
	{
		SetLastError(OPCODE_NOT_ALLOW_TYPE_SOURCE);
		return false;
	}
	else if (targetFailed)
	{
		SetLastError(OPCODE_NOT_ALLOW_TYPE_TARGET);
		return false;
	}
	return true;
}

int ValidateLabel(char *candidate, char endingChar, bool doLengthCheck, bool guessWork, char *target)
{
	const Instruction *inst;
	unsigned int i;
	char c;

	i = 0;
	if (candidate == NULL)
		return i;
	if (guessWork && strchr(candidate, endingChar) != candidate + strlen(candidate) - 1)
		return i;
	/* first char has to be a letter (lower/upper cased) */
	if (!islower(*candidate) && !isupper(*candidate))
	{
		SetLastError(LABEL_MUST_BEGIN_LETTER);
		return i;
	}
	while (*(candidate + i) != endingChar)
	{
		c = *(candidate + i);
		if (i >= MAX_LABEL_LENGTH || (!islower(c) && !isupper(c) && !isdigit(c)))
		{
			SetLastError(LABEL_TOO_LONG_OR_BAD_CHARS);
			return ZERO;
		}
		*(target + i) = c;
		i++;
	}
	*(target + i) = LINE_TERMINATION_CHAR;
	if (doLengthCheck && endingChar != LINE_TERMINATION_CHAR && strlen(candidate) - 1 != i)
	{
		SetLastError(LABEL_MUST_END_CHAR);
		return ZERO;
	}
	inst = GetInstructionData(target);	/* verify that the label isn't badly named */
	if (inst != NULL)
		SetLastError(LABEL_NAMED_OPCODE);
	return i;
}
