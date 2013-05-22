#ifndef VALIDATION_H_
#define VALIDATION_H_

bool ValidateAddressingType(BYTE addressingType, bool sourceOperand);

int ValidateLabel(char * candidate, char endingChar, bool doLengthCheck, bool guessWork, char * target);

#endif /* VALIDATION_H_ */
