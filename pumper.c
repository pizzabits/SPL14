#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "general.h"
#include "structs.h"
#include "utils.h"

#if _VERBOSE == 1
#define VERBOSE
#endif

size_t IC=0;						/* instruction code counter */
size_t DC=0;						/* data counter */
size_t externCount;
size_t entryCount;
size_t labelCount;
size_t unresolvedLabelCount;
size_t aligning;
char *lastError;
char *inputFilename;
bool localSyntaxError;
bool globalSyntaxError;
bool doEntryFile;
bool doExternFile;
Code code[MAX_LINES];				/* machine coded code */
struct Word data[MAX_DATA_LINES];	/* machine coded data */
struct Label *labels[MAX_LABELS];
struct Label *unresolvedLables[MAX_LABELS];
struct Label *entries[MAX_ENTRIES];
struct Label *externs[MAX_EXTERNS];

char errorMessage[MAX_LINE_LENGTH];
char GetCodeType(enum CodeTypes type);

void FixDataAddresses()
{
	size_t i;

	i = 0;
	while (i < labelCount)
	{
		if (labels[i]->DataOffset != ZERO) 			/* data label - refers to the part added after code */
			labels[i]->Value += START_ADDRESS + IC;	/* add the relative address to IC */
		else
			labels[i]->Value += START_ADDRESS;		/* code label */
		i++;
	}
}

void ResolveLabels()
{
	size_t i, j;

	for (i=0; i < unresolvedLabelCount; i++)	/* resolve unresolved labels */
	{
#ifdef VERBOSE
		printf("resolving label '%s' index: %d\n", unresolvedLables[i]->Name, i);
#endif
		for (j = 0; j < labelCount; j++)
		{
			if (strcmp(unresolvedLables[i]->Name, labels[j]->Name) == ZERO)
			{
#ifdef VERBOSE
				printf("found at index %d of labels. copying addressing values.\n", j);
#endif
				unresolvedLables[i]->Value = labels[j]->Value;
				unresolvedLables[i]->DataOffset = labels[j]->DataOffset;
				break;	/* resolved a label */
			}
		}
	}
}

/* WriteObjectAndExternOutput: writes the object and extern files for
 * the current input assembly, assuming that all the information
 * (code, data, resolved labels etc) was collected by the first iteration.
 * the code and data arrays are being iterated for the second and last time. */
void WriteObjectAndExternOutput(char *assemblyFilename)
{
	char *objectFilename;
	char *externFilename;
	FILE *obj;
	FILE *ext;
	size_t i, j;
	char *codedString;
	char *addrDummy;
	char *codeDummy;

	codedString = (char *) alloc(MAX_LINE_LENGTH);
	addrDummy = (char *) alloc(MAX_LINE_LENGTH);
	codeDummy = (char *) alloc(MAX_LINE_LENGTH);

	objectFilename = GetCopy(assemblyFilename);
	if (doExternFile)
		externFilename = GetCopy(assemblyFilename);

	/* create new object and extern files */
	objectFilename = ChangeExtension(objectFilename, OBJECT_FILE_EXTENSION);
	if (NULL == objectFilename)
	{
		FatalError(GENERAL_STREAM, CHANGE_EXTENSION_ERROR_FILENAME, assemblyFilename);
	}
	if (doExternFile)
	{
		externFilename = ChangeExtension(externFilename, EXTERN_FILE_EXTENSION);
		if (NULL == externFilename)
			FatalError(GENERAL_STREAM, CHANGE_EXTENSION_ERROR_FILENAME, assemblyFilename);
	}
	obj = fopen(objectFilename, WRITE_PERMISSIONS);
	ASSERT(NULL != obj, OPEN_WRITE_FAILED);

	if (doExternFile)
	{
		ext = fopen(externFilename, WRITE_PERMISSIONS);
		ASSERT(NULL != ext, OPEN_WRITE_FAILED);
	}

	WriteObjectHeader(obj);

	for (i = 0; i < IC; i++)	/* perform another pass on the code and data */
	{
		if (code[i].UnresolvedLabel)
		{
			if (ZERO == unresolvedLables[code[i].Word.Bits]->Value)		/* check for extern */
			{	/* look for the name in externs table */
				for (j = 0; j < externCount; j++)
				{
					if (strcmp(externs[j]->Name, unresolvedLables[code[i].Word.Bits]->Name) == ZERO)
					{
						code[i].Type = EXTERNAL;	/* the code references to an extern */
#ifdef VERBOSE
						printf("extern %s\t%d\n", externs[j]->Name, START_ADDRESS + i);
#endif
						itob(START_ADDRESS + i, addrDummy, BASE);
						fprintf(ext, EXT_ENT_FILES_FORMAT, externs[j]->Name, addrDummy);
					}
				}
				itob(unresolvedLables[code[i].Word.Bits]->Value, codeDummy, BASE); /* value equals 0 */
				Align(codeDummy, aligning);
				fprintf(obj, OBJECT_CODE_FORMAT, addrDummy, codeDummy, GetCodeType(code[i].Type));
#ifdef VERBOSE
				printf("%s\t%s\n", addrDummy, codeDummy);
#endif
			}
			else
			{	/* addressing to a resolved label. */
				code[i].Type = RELOCATABLE;
				itob(i + START_ADDRESS, addrDummy, BASE);
				itob(unresolvedLables[code[i].Word.Bits]->Value, codeDummy, BASE);
				Align(codeDummy, aligning);
				fprintf(obj, OBJECT_CODE_FORMAT, addrDummy, codeDummy, GetCodeType(code[i].Type));
#ifdef VERBOSE
				printf("%s\t%d\n", unresolvedLables[code[i].Word.Bits]->Name, START_ADDRESS + i);
#endif
			}
			code[i].Word.Bits = unresolvedLables[code[i].Word.Bits]->Value;
		}
		else
		{	/* regular absolute code */
			code[i].Type = ABSOLUTE;
			itob(i + START_ADDRESS, addrDummy, BASE);
			itob(code[i].Word.Bits, codeDummy, BASE);
			Align(codeDummy, aligning);
			fprintf(obj, OBJECT_CODE_FORMAT, addrDummy, codeDummy, GetCodeType(code[i].Type));
		}
	}

	if (doExternFile)
	{
		fclose(ext);	/* done writing externs file */
		free(externFilename);
	}

	for (; i < IC + DC; i++)
	{	/* data section */
		itob(i + START_ADDRESS, addrDummy, BASE);
		itob(data[i - IC].Bits, codeDummy, BASE);
		Align(codeDummy, aligning);
		fprintf(obj, OBJECT_DATA_FORMAT, addrDummy, codeDummy);
	}

	fclose(obj);
	free(objectFilename);
	free(addrDummy);
	free(codedString);
	free(codeDummy);
}

void ResolveEntriesLocations()
{
	size_t i, j;

	for (i = 0; i < labelCount; i++)
		for (j = 0; j < entryCount; j++)
			if (entries[j]->Value == ZERO && strcmp(labels[i]->Name, entries[j]->Name) == ZERO)
				entries[j]->Value = labels[i]->Value;
}

void SecondIteration(char *assemblyFilename)
{
	FixDataAddresses();
	ResolveEntriesLocations();
	ResolveLabels();
	WriteObjectAndExternOutput(assemblyFilename);
	WriteEntryOutput(assemblyFilename);
}

void WriteEntryOutput(char *assemblyFilename)
{
	char *filename ;
	FILE *output ;
	size_t i;
	char *addrDummy;

	if (!doEntryFile)
		return;

	addrDummy = (char *) alloc(MAX_LINE_LENGTH);
	filename = GetCopy(assemblyFilename);
	filename = ChangeExtension(filename, ENTRY_FILE_EXTENSION);

	if (NULL == filename)
	{
		FatalError(GENERAL_STREAM, CHANGE_EXTENSION_ERROR_FILENAME, assemblyFilename);
	}
	output = fopen(filename, WRITE_PERMISSIONS);
	ASSERT(NULL != output, OPEN_WRITE_FAILED);

	for (i = 0; i < entryCount; i++)
	{
		itob(entries[i]->Value, addrDummy, BASE);
#ifdef VERBOSE
		printf(EXT_ENT_FILES_FORMAT, entries[i]->Name, addrDummy);
#endif
		fprintf(output, EXT_ENT_FILES_FORMAT, entries[i]->Name, addrDummy);
	}
	fclose(output);
	free(addrDummy);
	free(filename);
}

void WriteObjectHeader(FILE *output)
{
	char codedString[MAX_LINE_LENGTH] = LINE_TERMINATION;
	/* write object header */
	fprintf(output, OBJECT_FILE_HEADER);
	itob(IC, codedString, BASE);
	fprintf(output, "\t\t%s\t", codedString);
	itob(DC, codedString, BASE);
	fprintf(output, "%s\n", codedString);
}

char GetCodeType(enum CodeTypes type)
{
	switch (type)
	{
		case ABSOLUTE:
			return ABSOLUTE_CHAR;
			break;
		case RELOCATABLE:
			return RELOCATABLE_CHAR;
			break;
		case EXTERNAL:
			return EXTERNAL_CHAR;
			break;
	}
	ASSERT(false, FATAL_ERROR);
	return ABSOLUTE; /* make pedantic happy! */
}

int main(int argc, char **argv)
{
	int i;

	lastError = EMPTY_STRING;

	if (argc < TWO)
		FatalError(GENERAL_STREAM, USAGE_RUN_ARGUMENT, argv[ZERO]);

	i = ONE;
	while (i < argc)
	{
		if (!HasExtension(argv[i], ASSEMBLY_FILE_EXTENSION))
			FatalError(GENERAL_STREAM, CHANGE_EXTENSION_ERROR_FILENAME, argv[i]);
#ifdef VERBOSE
		printf("Parsing input #%d, %s\n", i, argv[i]);
#endif

		globalSyntaxError = localSyntaxError = false;

		inputFilename = argv[i];
		ParsePseudoAssemblyFile(inputFilename);
		if (!globalSyntaxError)
		{
			if (ZERO < externCount)
				doExternFile = true;
			if (ZERO < entryCount)
				doEntryFile = true;
			SecondIteration(inputFilename);
		}
		Release();
		i++;
	}
#ifdef VERBOSE
	printf("DONE.\n");
#endif
	return ONE;
}
