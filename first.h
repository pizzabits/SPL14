#ifndef FIRST_H_
#define FIRST_H_

#include <stdbool.h>

bool ParseLine(char * inputLine);

void ParsePseudoAssemblyFile(char * filename);

bool ParseFirstPart();

bool AddExtern(char *start);

bool AddEntry(char *start);

void AddLabel(char *name, size_t originalDC, bool isData);

int ParseStringDefinition();

int ParseDataArray();

bool Initialize();

bool Release();

#endif /* FIRST_H_ */
