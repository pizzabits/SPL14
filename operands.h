#ifndef OPERANDS_H_
#define OPERANDS_H_
#include <stdbool.h>

void SetAddressingType(bool sourceOperand, BYTE addressingType);

bool ParseRegister(bool sourceOperand, bool setAddressingType);

bool ParseImmediateAddressing(bool sourceOperand, bool checkSpecialChar, bool setAddressingType, bool validateAddrType);
/*bool ParseImmediateAddressing(bool sourceOperand, bool checkSpecialChar, bool setAddressingType);*/

bool ParseIndexAddressing(bool sourceOperand);

bool ParseDirectAddressing(bool sourceOperand);

#endif /* OPERANDS_H_ */
