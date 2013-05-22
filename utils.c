#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "utils.h"
#include "general.h"
#include "structs.h"

#if _VERBOSE == 1
#define VERBOSE
#endif

char *lastError;
extern char *inputFilename;
extern bool localSyntaxError;
extern bool globalSyntaxError;
extern size_t lineCount;

void SetLastError(char *message)
{
	lastError = message;
}

char *GetLastError()
{
	return lastError;
}

void FatalError(FILE *stream, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
	exit(ERROR);
}

void SyntaxError(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	fprintf(ERROR_STREAM, SYNTAX_ERROR_MESSAGE, inputFilename, lineCount);
	vfprintf(ERROR_STREAM, format, args);
	va_end(args);
	localSyntaxError = globalSyntaxError = true;
}

void *alloc(size_t size)
{
	void *memory;

	memory = calloc(1, size);
	ASSERT(memory != NULL, NOT_ENOUGH_MEMORY);
	return memory;
}

bool HasExtension(char *filename, const char *extension)
{
	return strstr(filename, extension) ==
			filename + strlen(filename) - strlen(extension);
}

char *ChangeExtension(char *filename, char *changeTo)
{
	BYTE i, d;
	char *extensionStart;

	if (filename == NULL || changeTo == NULL)
		return NULL;

	i = 0;

	filename = (char *)realloc(filename, strlen(filename) + strlen(changeTo));
	ASSERT(filename != NULL, NOT_ENOUGH_MEMORY);

	extensionStart = strrchr(filename, DOT_CHAR);
	if (NULL == extensionStart)
	{
		printf("strchr returned %s\nbut strrchr failed.\n", strchr(filename, DOT_CHAR));
		printf("%s\n", filename);
		return NULL;
	}
	i = 0;
	while ( (*(extensionStart + i) = *(changeTo + i)) != LINE_TERMINATION_CHAR ) i++;
	return filename;
}

char *GetCopy(char *original)
{
	char *copy;
	size_t len;

	if (NULL == original)
		return NULL;

	len = strlen(original);
	copy = (char *)alloc(len + 1);
	strncpy(copy, original, len);
	copy[len] = LINE_TERMINATION_CHAR;
	return copy;
}

/*
 * reverse string s in place
 */
void reverse(char *s)
{
	size_t c, i, j;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/*
 * convert n to characters in s
 * and returns s
 * free to choose base
 */
char *itob(size_t n, char *s, BYTE b)
{
	size_t i;

	i = 0;
	do
	{								/* generate digits in reverse order */
		s[i++] = n % b + ZERO_CHAR;	/* get next digit */
	}
	while ((n /= b) > 0);			/* delete it */
	s[i] = LINE_TERMINATION_CHAR;
	reverse(s);
	return s;
}

size_t CalculateAligning()
{
	size_t bits, base;

	bits = 0;
	base = BASE - 1; /* a number in current base is at most BASE-1 */

	do
	{
		bits++;
	}
	while ((base /= TWO) > 0);
	return BITS_PER_WORD / bits;
}

/* warning: candidate must be large enough to contain extra padding size.
			assuming that candidate is null-terminated. */
void PushStringBy(char *candidate, int paddingSize)
{
	int len;
	ASSERT(strcmp(candidate, EMPTY_STRING), "empty candidate. WHAT?!\n");

	len = strlen(candidate);	/* remember candidate's length */
	while (len >= 0)
	{	/* copy candidate's chars to their new locations, including the null terminating char */
		candidate[len + paddingSize] = candidate[len];
		len--;
	}
	paddingSize--;				/* pad with 0..paddingSize-1 zeros */
	while (paddingSize >= 0)
	{
		candidate[paddingSize] = ZERO_CHAR;
		paddingSize--;
	}
}

void Align(char *codedString, int aligning)
{
	BYTE zeroesToPad;
	size_t len = strlen(codedString);
	zeroesToPad = aligning - len;
	if (zeroesToPad == 0)
		return;
	PushStringBy(codedString, zeroesToPad);
}
