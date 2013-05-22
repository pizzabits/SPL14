#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "structs.h"
#include "general.h"
#include "utils.h"
#include "validation.h"

#if _VERBOSE == 1
#define VERBOSE
#endif

extern unsigned int IC;
extern Code code[];
extern char *part;
extern struct WordView *view;
extern struct Label *unresolvedLables[];
extern unsigned int unresolvedLabelCount;
extern char extraWords;

void SetAddressingType(bool sourceOperand, BYTE addressingType)
{
	if (sourceOperand)
		view->SrcAddrType = addressingType;
	else
		view->DestAddrType = addressingType;
}

bool ParseRegister(bool sourceOperand, bool setAddressingType)
{
	bool notNumber = false;
	int regNumber;

	/* assuming that register addressing looks like:
	 * "rX" while  0 <= X <= #_of_registers
	 * example: "r4",*/
	if (*part != REGISTER_CHAR)
		return false;
	NaN(part + 1, notNumber);
	if (notNumber)
	{
		SetLastError(REGISTER_NUMBER_EXPECTED);
		return false;
	}
	if ( (regNumber = atoi(part + 1)) >= REGISTERS)
	{
		SetLastError(REGISTER_NUMBER_BOUNDS);
		return false;
	}
	if (!ValidateAddressingType(THREE, sourceOperand))
	{
		/* this is crucial, since the operand is a verified register! */
		SYNTAX_ASSERT(false, GetLastError());
		return false;
	}
	part++;
	if (setAddressingType)
		SetAddressingType(sourceOperand, THREE);
	if (sourceOperand)
		view->SrcReg = regNumber;
	else
		view->DestReg = regNumber;
	return true;
}

bool ParseImmediateAddressing(bool sourceOperand, bool checkSpecialChar, bool setAddressingType, bool validateAddrType)
{
	bool notNumber = false;
	bool mustValidateAddressing = false;

	if (checkSpecialChar)
	{
		if (IMMEDIATE_ADDRESSING_CHAR == *part)
		{
			mustValidateAddressing = true;
		}
		else
			return false;
	}
	if (mustValidateAddressing)
	{
		if (!ValidateAddressingType(ZERO, sourceOperand))
		{
			SYNTAX_ASSERT(false, GetLastError()); /* get the error from ValidateAddrType */
			return false;
		}
	}
	else if (validateAddrType && !ValidateAddressingType(ZERO, sourceOperand))
		return false;
	if (checkSpecialChar)
		part++;
	NaN(part, notNumber);	/* verify that its a number */
	if (notNumber)
	{
		SetLastError(NUMBER_EXPECTED_GOT_NAN);
		if (checkSpecialChar) part--;
		return false;
	}
	if (setAddressingType)
		SetAddressingType(sourceOperand, ZERO);
	/* store code of immediate number as an extra word */
	extraWords++;
	code[IC + extraWords].Word.Bits = atoi(part);
	code[IC + extraWords].Type = ABSOLUTE;
	return true;
}

bool ParseIndexAddressing(bool sourceOperand)
{
	struct Label *labelAddressedWithIndex;
	struct Label *indexAsLabel;
	bool parsed;
	char i;

	if (strchr(part, INDEX_ADDRESSING_OPEN_CHAR) <= part || /* index addressing */
			strchr(part, INDEX_ADDRESSING_CLOSE_CHAR) <= part)
		return false;
	if (!ValidateAddressingType(TWO, sourceOperand))
		return false;
	SetAddressingType(sourceOperand, TWO);
	/* get label referred */

	labelAddressedWithIndex = alloc(sizeof(label_type));
	i = ValidateLabel(part, INDEX_ADDRESSING_OPEN_CHAR, false, false, labelAddressedWithIndex->Name);
	unresolvedLables[unresolvedLabelCount] = labelAddressedWithIndex;
	extraWords++;
	code[IC + extraWords].Word.Bits = unresolvedLabelCount;
	code[IC + extraWords].UnresolvedLabel = true;
	unresolvedLabelCount++;

	/* rest of the part must be the index confined by brackets */
	part += i;
	part = strtok(part, INDEX_ADDRESSING_CHARS);
	if (NULL == part)
	{
		SetLastError(INDEX_EXPECTS_BRACKETS);
		return false;
	}
	/* detect the type of the index - register / label / immediate */
	/* check if its a register */
	parsed = ParseRegister(sourceOperand, false);
	if (parsed)
		return true;
	/* check if its an immediate number, but don't validate addressing type since it's index addressing */
	if (ParseImmediateAddressing(sourceOperand, false, false, false))
		return true;

	/* then its a label, validate it */
	indexAsLabel = alloc(sizeof(label_type));
	ValidateLabel(part, LINE_TERMINATION_CHAR, true, false, indexAsLabel->Name);
	extraWords++;
	code[IC + extraWords].Word.Bits = unresolvedLabelCount;
	code[IC + extraWords].UnresolvedLabel = true;
	unresolvedLables[unresolvedLabelCount] = indexAsLabel;
	unresolvedLabelCount++;
	return true;
}

bool ParseDirectAddressing(bool sourceOperand)
{
	struct Label *directLabel;

	if (!ValidateAddressingType(ONE, sourceOperand))
		return false;
	SetAddressingType(sourceOperand, ONE);
	directLabel = alloc(sizeof(label_type));
	if (ZERO == ValidateLabel(part, LINE_TERMINATION_CHAR, true, false, directLabel->Name))
		return false;
	extraWords++;
	code[IC + extraWords].UnresolvedLabel = true;
	code[IC + extraWords].Word.Bits = unresolvedLabelCount;
	unresolvedLables[unresolvedLabelCount] = directLabel;
	unresolvedLabelCount++;
	return true;
}
