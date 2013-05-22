#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>

typedef unsigned char BYTE;

#define ASSERT(cond, message) { if (!(cond)) FatalError(GENERAL_STREAM, (message)); }

#define SYNTAX_ASSERT(cond, message) { if (!(cond)) SyntaxError((message)); }

#define NaN(numberString, result) { \
	char i = 0; \
	while (*(numberString + i) != LINE_TERMINATION_CHAR) { \
		if (!isdigit(*(numberString + i))) \
			if (MINUS_CHAR != *(numberString + i) && PLUS_CHAR != \
					*(numberString + i)) { \
				result = true; \
				break; \
			} \
		i++; \
	} \
}

void SetLastError(char *message);

char *GetLastError();

void FatalError(FILE *stream, const char *format, ...);

void *alloc(size_t size);

bool HasExtension(char *filename, const char *extension);

char *ChangeExtension(char *filename, char *changeTo);

char *GetCopy(char *original);

void reverse(char *s);

char *itob(size_t n, char *s, BYTE b);

size_t CalculateAligning();

void PushStringBy(char *candidate, int paddingSize);

void Align(char *codedString, int aligning);


#endif /* UTILS_H_ */
