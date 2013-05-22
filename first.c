#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "first.h"
#include "general.h"
#include "structs.h"
#include "utils.h"
#include "validation.h"

#if _VERBOSE == 1
#define VERBOSE
#endif

extern struct Word data[];
extern struct Label *labels[];
extern struct Label *unresolvedLables[];
extern struct Label *externs[MAX_EXTERNS];
extern struct Label *entries[MAX_ENTRIES];
extern bool localSyntaxError;

extern size_t IC;
extern size_t DC;
extern size_t labelCount;
extern size_t unresolvedLabelCount;
extern size_t entryCount;
extern size_t externCount;
extern size_t aligning;
extern bool doEntryFile;
extern bool doExternFile;

size_t lineCount;
char isLabel = false;
char line[MAX_LINE_LENGTH];
char lineCopy[MAX_LINE_LENGTH];
char labelName[MAX_LABEL_LENGTH];
char *part;
struct WordView *view;
struct Word  *word;
const Instruction *inst;
struct Label *label;
char extraWords = 0;

bool Initialize()
{
	aligning = CalculateAligning();
	externCount = entryCount = labelCount = unresolvedLabelCount = 0;
	doEntryFile = doExternFile = false;
	IC = DC = 0;
	view = alloc(sizeof(word_view_type));
	word = alloc(sizeof(word_type));
	return true;
}

bool Release()
{
	size_t i;
	free(view);
	free(word);
	for (i = 0; i < labelCount; i++)
		free(labels[i]);
	for (i = 0; i < unresolvedLabelCount; i++)
		free(unresolvedLables[i]);
	for (i = 0; i < externCount; i++)
	{
		free(externs[i]);
	}
	for (i = 0; i < entryCount; i++)
		free(entries[i]);
	externCount = entryCount = labelCount = unresolvedLabelCount = 0;
	IC = DC = 0;
	return true;
}

/* ParseDataArray: Stores in the data array.
 * Returns the number of entries used. */
int ParseDataArray()
{
	size_t originalDC;
	int number;
	bool notNumber;

	originalDC = DC;
	notNumber = false;
	/* construct a word for each element in the array and store it */
	while ((part = strtok(NULL, DELIMITERS_AND_COMMA)) != NULL)
	{
		NaN(part, notNumber);
		number = atoi(part);
		if (notNumber == true || (number == ZERO && *part != ZERO_CHAR))
		{
			DC = originalDC;
			SYNTAX_ASSERT(false, DATA_DEF_CONTAINS_NAN);
			return ERROR;
		}
		data[DC].Bits = number;
		DC++;
	}
	SYNTAX_ASSERT(ZERO < DC - originalDC, DATA_DEF_NO_NUMBERS);
	return DC - originalDC;
}

/* Stores an array of characters into data
 * including the termination char.
 * Returns the number of characters stored. */
int ParseStringDefinition()
{
	char *closingQuotationCommaIndex;
	size_t originalDC;

	originalDC = DC;
	/* seek for the starting quotation comma */
	part = strchr(line, QUOTATION_COMMA_CHAR);
	if (NULL == part)
	{
		SYNTAX_ASSERT(false, STRING_DEF_SURROUNDING_QUOT);
		return ERROR;
	}
	/* seek for the ending quotation comma, there should be exactly one */
	closingQuotationCommaIndex = strchr(part + 1, QUOTATION_COMMA_CHAR);
	if (closingQuotationCommaIndex == NULL)
	{
		SYNTAX_ASSERT(false, STRING_DEF_SURROUNDING_QUOT);
		return ERROR;
	}
	part++; /* skip one quotation comma and replace the ending with a termination char */
	while (part < closingQuotationCommaIndex)
	{
		data[DC].Bits = *part;
		DC++;
		part++;
	}
	data[DC].Bits = NULL; /* terminate the string */
	DC++;
	part++; /* skip the closing quotation comma */

	/* verify that skipping the parts really exhausts the whole line */
	part = strtok(part, IGNORED_DELIMITERS);
	if (part != NULL)
	{
		SYNTAX_ASSERT(false, EXPECTED_EOL);
		return ERROR;
	}
	return DC - originalDC;
}

void AddLabel(char *name, size_t originalDC, bool isData)
{
	/* insert a symbol to the symbol table */
	label = alloc(sizeof(label_type));
	strncpy(label->Name, name, strlen(name));
	if (isData)
	{ /* data was stored, save DC and offset from last one */
	/**if (DC > originalDC)*/
		label->DataOffset = DC - originalDC;
		label->Value = originalDC;
	}
	else
	{
		label->Value = IC;
		label->DataOffset = ZERO;
	}
	labels[labelCount] = label;
	labelCount++;
}

bool AddEntryOrExtern(bool entry, char *start)
{
	char *actual;
	size_t size;

	start += strlen(entry ? ENTRY_DEFINITION : EXTERN_DEFINITION) + 1;
	if ((actual = strpbrk(start, ABC)) == NULL)
	{
		SYNTAX_ASSERT(false, entry ? ENTRY_EXPECTING_LABEL : EXTERN_EXPECTING_LABEL);
		return false;
	}

	label = alloc(sizeof(label_type));

	if ((size = ValidateLabel(actual, LINE_TERMINATION_CHAR, true, false, label->Name)) == ZERO)
		return false;
	if (NULL != strtok(actual + strlen(label->Name), IGNORED_DELIMITERS))
	{
		SYNTAX_ASSERT(false, entry ? ENTRY_NOT_EXPECTING_MORE : EXTERN_NOT_EXPECTING_MORE);
		return false;
	}

	if (entry)
	{
		ASSERT(entryCount < MAX_ENTRIES, ENTRY_EXCEEDED_MAX);
		entries[entryCount] = label;
		entryCount++;
	}
	else
	{
		ASSERT(externCount < MAX_EXTERNS, EXTERN_EXCEEDED_MAX);
		externs[externCount] = label;
		externCount++;
	}

	return true;
}

/* Returns true if the parsing covered the whole input. */
bool ParseFirstPart()
{
	char *entryOrExternStart;

	if (NULL == part)
		return true;
	/* check for comment, assuming that a commented line must starts with a comment char */
	if (strchr(part, COMMENT_CHAR) == part)
		return true;
	/* check for label */
	if (ValidateLabel(part, LABEL_END_CHAR, true, true, labelName) > ZERO && labelName != NULL)
	{
		isLabel = true;
		return false;
	}
	else if (strstr(part, DATA_DEFINITION) != NULL)
	{ /* data array without a label */
		ParseDataArray();
		return true;
	}
	else if (strstr(part, STRING_DEFINITION) != NULL)
	{ /* string definition without a label */
		ParseStringDefinition();
		return true;
	}
	/* TODO: deal with .entry and .extern - NO LABEL ALLOWED PRIOR ! ! ! */
	else if ((entryOrExternStart = strstr(part, EXTERN_DEFINITION)) != NULL)
	{
		if (!AddEntryOrExtern(false, entryOrExternStart))
			return false;
		return true;
	}
	else if ((entryOrExternStart = strstr(part, ENTRY_DEFINITION)) != NULL)
	{
		if (!AddEntryOrExtern(true, entryOrExternStart))
			return false;
		return true;
	}
	else if (!TranslateInstruction())
	{
		char *lastError = GetLastError();

		SYNTAX_ASSERT(strcmp(lastError, EMPTY_STRING) == ZERO, lastError);
		SetLastError(EMPTY_STRING);
		return false;
	}
	return true;
}

void ParsePseudoAssemblyFile(char *filename)
{
	FILE *file;
	char *line;
	char *lastError;

	line = (char *) alloc(MAX_LINE_LENGTH * 2);
	ASSERT(Initialize(), INITILIZATION_ERROR);
	file = fopen(filename, READ_PERMISSIONS);
	if (NULL == file)
	{
		FatalError(GENERAL_STREAM, ERROR_OPEN_READ_FILENAME, filename);
	}

	lineCount = 1;

	while (NULL != (line = fgets(line, MAX_LINE_LENGTH * 2, file)))
	{
		if (NULL != strchr(line, LINE_FEED_CHAR))
			line[strlen(line) - 1] = LINE_TERMINATION_CHAR;
		ParseLine(line);
		lineCount++;
		localSyntaxError = false;
	}
	fclose(file);
	return;
}

bool ParseLine(char * inputLine)
{
	size_t originalDC;

	/* setup */
	isLabel = false;
	memset(word, 0, sizeof(word_type));
	memset(view, 0, sizeof(word_view_type));
	memset(labelName, 0, MAX_LABEL_LENGTH);

	strncpy(line, inputLine, MAX_LINE_LENGTH);
	strncpy(lineCopy, inputLine, MAX_LINE_LENGTH);

#ifdef VERBOSE
	printf("Parsing '%s'\n", inputLine);
#endif

	/* split the line into part */
	part = strtok(lineCopy, IGNORED_DELIMITERS);
	if (ParseFirstPart())
		return true;
	else if (localSyntaxError)
		return false;
	/* continue iterating over the split'd part */
	if (NULL == (part = strtok(NULL, IGNORED_DELIMITERS)))
	{
		SYNTAX_ASSERT(false, NOT_EXPECTING_EOL);
		return false;
	}

	originalDC = DC;
	if (strstr(part, DATA_DEFINITION) != NULL)
	{ /* data array */
		if (!isLabel)
		{
			SYNTAX_ASSERT(false, DATA_DEF_REDUNDANT);
			return false;
		}
		/* construct a word-per-element of the array and store it */
		if (ERROR != ParseDataArray())
			AddLabel(labelName, originalDC, true);
		return false;
	}
	else if (strstr(part, STRING_DEFINITION) != NULL)
	{ /* string definition */
		if (!isLabel)
		{
			SYNTAX_ASSERT(false, STRING_DEF_REDUNDANT);
			return false;
		}
		/* construct a word-per-character of the string and store it */
		if (ERROR != ParseStringDefinition())
			AddLabel(labelName, originalDC, true);
		return false;
	}
	else
	{	/* instruction */
		if (isLabel)
			AddLabel(labelName, ZERO, false);
		if (!TranslateInstruction())
			return false;
	}
	return true;
}
