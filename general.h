/* universal definitions */
#define ERROR_STREAM stderr
#define GENERAL_STREAM stdout
#define _VERBOSE 1
#define ERROR -1
#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define LABEL_END_CHAR ':'
#define COMMENT_CHAR ';'
#define MINUS_CHAR '-'
#define PLUS_CHAR '+'
#define ZERO_CHAR '0'
#define ONE_CHAR '1'
#define DOT_CHAR '.'
#define LINE_TERMINATION "\0"
#define LINE_TERMINATION_CHAR '\0'
#define LINE_FEED_CHAR '\n'
#define TAB_CHAR '\t'
#define QUOTATION_COMMA "\""
#define QUOTATION_COMMA_CHAR '"'
#define EMPTY_STRING ""
#define WRITE_PERMISSIONS "w"
#define READ_PERMISSIONS "r"
#define ABC "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
/* universal definitions */

/* design specific definitions */
#define BITS_PER_WORD 20
#define BASE 4
#define START_ADDRESS 100
#define OPCODES 16
#define OPCODE_MAX_LENGTH 5 /* fits for 'stop' and null terminating char */
#define MAX_STRINGS 100
#define MAX_LINES 10000
#define MAX_DATA_LINES 5000
#define MAX_LABEL_LENGTH 30
#define MAX_LABELS 7500
#define MAX_ENTRIES 2000
#define MAX_EXTERNS 2000
#define MAX_LINE_LENGTH 80
#define REGISTERS 8
#define IGNORED_DELIMITERS " \t"
#define DELIMITERS_AND_COMMA ", \t"
#define REGISTER_CHAR 'r'
#define ABSOLUTE_CHAR 'a'
#define RELOCATABLE_CHAR 'r'
#define EXTERNAL_CHAR 'e'
#define OPCODE_ADDRESSING_CHAR '/'
#define OPCODE_ADDRESSING "/"
#define IMMEDIATE_ADDRESSING_CHAR '#'
#define INDEX_ADDRESSING_OPEN_CHAR '{'
#define INDEX_ADDRESSING_CLOSE_CHAR '}'
#define INDEX_ADDRESSING_CHARS "{}"
#define DATA_DEFINITION ".data"
#define STRING_DEFINITION ".string"
#define ENTRY_DEFINITION ".entry"
#define EXTERN_DEFINITION ".extern"
#define ASSEMBLY_FILE_EXTENSION ".as"
#define ENTRY_FILE_EXTENSION ".ent"
#define EXTERN_FILE_EXTENSION ".ext"
#define OBJECT_FILE_EXTENSION ".ob"
#define OBJECT_FILE_HEADER "Base 4 Address\tBase 4 machine code\tAbsolute, relocateable or external\n"
#define OBJECT_CODE_FORMAT "%s\t\t%s\t\t%c\n"
#define OBJECT_DATA_FORMAT "%s\t\t%s\n"
#define EXT_ENT_FILES_FORMAT "%s\t%s\n"
/* design specific definitions */

/* messages sections */
/*	 --> general*/
#define NUMBER_EXPECTED_GOT_NAN "Expected number but got not a number.\n"
#define OPEN_WRITE_FAILED "Could not open for write\n"
#define NOT_ENOUGH_MEMORY "Not enough memory!\n"
#define INITILIZATION_ERROR "Unable to initialize\n"
#define SYNTAX_ERROR_MESSAGE "%s:%d: error: "

/* 	 --> code */
#define EXPECTED_EOL "Expected end of line.\n"
#define NOT_EXPECTING_EOL "End of line unexpected!\n"
#define OPCODE_MUST_TYPECHAR "Cannot find an opcode type character!\n"
#define REGISTER_NUMBER_EXPECTED "Expected register number, got not a number.\n"
#define REGISTER_NUMBER_BOUNDS "Register number out of bounds.\n"
#define UNKNOWN_OPCODE "unknown opcode\n"
#define FATAL_ERROR "Fatal error!\n"
#define UNABLE_SKIP "Could not skip parts\n"
#define WRONG_TYPE "Wrong type preceding opcode. expected 0 or 1\n"
#define WRONG_ADDR_COMB_1ST "Wrong addressing combination on first property.\n"
#define WRONG_ADDR_COMB_2ND "Wrong addressing combination on second property.\n"
#define TOO_MANY_ADDR_PROPS "Too many addressing properties.\n"
#define OPCODE_NOT_EXPECT_OPERANDS "opcode does not expect any operands.\n"
#define EXTERN_NOT_EXPECTING_MORE "Wasn't expecting anymore parts after extern label definition.\n"
#define INDEX_EXPECTS_BRACKETS "Could not find index addressing brackets.\n"
#define LABEL_NAMED_OPCODE "label is named like an opcode.\n"
#define LABEL_MUST_END_CHAR "Label doesn't end with its ending character.\n"
#define LABEL_TOO_LONG_OR_BAD_CHARS "Either the label is too long or contains bad characters.\n"
#define LABEL_MUST_BEGIN_LETTER "Label must begin with a letter!\n"
#define OPCODE_NOT_ALLOW_TYPE_TARGET "opcode doesn't allow the addressing type for target operand.\n"
#define OPCODE_NOT_ALLOW_TYPE_SOURCE "opcode doesn't allow the addressing type for source operand.\n"


/*	 --> definitions */
#define DATA_DEF_CONTAINS_NAN ".data definition contains a value which is not a number.\n"
#define DATA_DEF_NO_NUMBERS ".data definition must define at least one number!\n"
#define DATA_DEF_REDUNDANT "redundant definition of data array since no label found.\n"
#define STRING_DEF_REDUNDANT "redundant definition of string since no label found.\n"
#define STRING_DEF_SURROUNDING_QUOT ".string definition must have surrounding quotation commas.\n"
#define ENTRY_CANT_RESOLVE "Could not resolve entry!\n"
#define ENTRY_EXPECTING_LABEL "Expected a label preceding entry definition.\n"
#define ENTRY_NOT_EXPECTING_MORE "Wasn't expecting anymore parts after entry label definition.\n"
#define ENTRY_EXCEEDED_MAX "Exceeded maximum entries.\n"
#define EXTERN_EXCEEDED_MAX "Exceeded maximum externs.\n"
#define EXTERN_EXPECTING_LABEL "Expected a label preceding extern definition\n"

/* 	 --> with arguments */
#define USAGE_RUN_ARGUMENT "USAGE:\t %s file1_name.as file2_name.as ...\n\n"
#define ERROR_OPEN_READ_FILENAME "Could not open %s for reading!\n"
#define ERROR_OPEN_WRITE_FILENAME "Could not open %s for writing!\n"
#define CHANGE_EXTENSION_ERROR_FILENAME "Could not change extension of %s!\n"
/* end of messages section */
