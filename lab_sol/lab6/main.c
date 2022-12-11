#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define BUFLEN 100
#define MAXMEM 65536

char AsmCode[MAXMEM][BUFLEN], MachCode[MAXMEM][BUFLEN];
int asmLines, machLines;
char src1[5], src2[5], dst[5], imm5[10], imm16[20], trapvect8[10], BaseR[5], * arg[4];
int PCoffset9, PCoffset11, offset6, LC, PC;

struct label {
	char symbol[60];
	int address;
};
struct label SymbolTable[MAXMEM];
int TableCount = 0;

enum ins {
	BR = 0, ADD, LD, ST, JSR,
	AND, LDR, STR, RTI, NOT,
	LDI, STI, JMP, RET, LEA, TRAP
};

static char* ins[] = {
	"BR", "ADD", "LD", "ST", "JSR","JSRR",
	"AND", "LDR", "STR", "RTI", "NOT",
	"LDI", "STI", "JMP", "RET","LEA", "TRAP"
};

static char* dir[] = {
	".ORIG", ".END", ".BLKW", ".STRINGZ", ".FILL"
};

static char* pseudo[] = {
	"HALT", "IN", "OUT", "GETC", "PUTS","PUTSP"
};

char* trapvector[] = { "x20","x21","x22","x23","x24","x25" };

/*functions*/
char* myitoa(int num, char* str, int radix) {
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", temp;
	unsigned unum;
	int i = 0, j, k;

	if (radix == 10 && num < 0) {
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;
	do {
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	if (str[0] == '-') k = 1;
	else k = 0;

	for (j = k; j <= (i - k - 1) / 2.0; j++) {
		temp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = temp;
	}
	return str;
}

int StringToNum(char* string) {
	int i = 0, NegFlag = 1, DecValue = 0;
	char Bits[10] = { 0 };
	char* endptr, * head = string;

	if (*(string++) == '-') {
		NegFlag = -1;
		string++;
	}
	while (*(string + i) != '\0') {
		Bits[i] = *(string + i);
		i++;
	}

	if (*head == 'X' || *head == 'x') DecValue = NegFlag * strtol(Bits, &endptr, 16);
	if (*head == '#') DecValue = NegFlag * strtol(Bits, &endptr, 10);
	if (*head == 'b' || *head == 'B') DecValue = NegFlag * strtol(Bits, &endptr, 2);

	return DecValue;
}

char* DecToBinString(int dec, int offsetlen) {
	char binString[33] = { '\0' };  //signed 32 bits
	char* extension[] = { "","0","00","000","0000","00000","000000",
						  "0000000","00000000" ,"000000000","0000000000","00000000000",
						  "000000000000","0000000000000","00000000000000" ,"000000000000000" };
	char temp[32] = { '\0' };
	int i = 0, NegFlag = 0;

	if (dec < 0) NegFlag = 1;
	myitoa(dec, binString, 2);     //deciaml number to binary string, in 2's complement

	//extension
	if (NegFlag == 0) {
		strcat(temp, extension[offsetlen - strlen(binString)]);
		strcat(temp, binString);            //sign extension
		strcpy(binString, temp);
	}
	//cut
	else {
		for (i = 0; i < offsetlen; i++) {
			binString[i] = binString[i + 32 - offsetlen];
		}
		binString[i] = '\0';
	}
	return binString;
}

void LowerToUpper(char buf[]) {
	for (int i = 0; i < strlen(buf); i++) {
		if (buf[i] >= 'a' && buf[i] <= 'z') {
			buf[i] = buf[i] - 'a' + 'A';
		}
	}
}

int IfBlankLine(int curline) {
	char blankline[] = { "\n" };
	int spaceflag = 0;

	if (strcmp(AsmCode[curline], blankline) == 0) {
		return 1;
	}
	else {
		for (int i = 0; AsmCode[curline][i] != '\0'; i++) {
			if (AsmCode[curline][i] != ' ') {
				spaceflag = 1;
				break;
			}
		}
		if (spaceflag == 0) return 1;
		else return 0;
	}
}

int IfRealLabel(char* curline) {
	int i = 0;
	char* CodeToken = (char*)malloc(sizeof(char) * BUFLEN);
	char copy[BUFLEN];

	strcpy(copy, curline);
	CodeToken = strtok(copy, " ");

	for (int i = 0; i < 5; i++) {
		if (strcmp(dir[i], CodeToken) == 0) {
			return 0;
		}
	}
	for (int i = 0; i < 6; i++) {
		if (strcmp(pseudo[i], CodeToken) == 0) {
			return 0;
		}
	}
	for (int i = 0; i < 17; i++) {
		if (strcmp(ins[i], "BR") == 0) {
			if (strcmp(CodeToken, "BR") == 0 || strcmp(CodeToken, "BRNZP") == 0 ||
				strcmp(CodeToken, "BRNZ") == 0 || strcmp(CodeToken, "BRNP") == 0 ||
				strcmp(CodeToken, "BRZP") == 0 || strcmp(CodeToken, "BRN") == 0 ||
				strcmp(CodeToken, "BRZ") == 0 || strcmp(CodeToken, "BRP") == 0)
				return 0;
		}

		else {
			if (strcmp(ins[i], CodeToken) == 0) {
				return 0;
			}
		}
	}
	//not directives, pseudo or instructions, should be a label
	return 1;
}

int GetStartLocation(char* curline) {
	char* CodeToken[2], copy[BUFLEN];
	int location;

	strcpy(copy, curline);
	for (int i = 0; i < 2; i++) {
		CodeToken[i] = (char*)malloc(sizeof(char) * BUFLEN);
	}
	CodeToken[0] = strtok(copy, " ");  //.ORIG
	CodeToken[1] = strtok(NULL, " ");   //start location
	location = StringToNum(CodeToken[1]);

	return location;
}

int WhichIns(char* ins) {
	if (strcmp(ins, "ADD") == 0) return ADD;
	else if (strcmp(ins, "AND") == 0) return AND;
	else if (strcmp(ins, "BR") == 0 || strcmp(ins, "BRNZP") == 0 ||
		strcmp(ins, "BRNZ") == 0 || strcmp(ins, "BRNP") == 0 ||
		strcmp(ins, "BRZP") == 0 || strcmp(ins, "BRN") == 0 ||
		strcmp(ins, "BRZ") == 0 || strcmp(ins, "BRP") == 0) return BR;
	else if (strcmp(ins, "LD") == 0) return LD;
	else if (strcmp(ins, "ST") == 0) return ST;
	else if (strcmp(ins, "JSR") == 0 || strcmp(ins, "JSRR") == 0) return JSR;
	else if (strcmp(ins, "LDR") == 0) return LDR;
	else if (strcmp(ins, "STR") == 0) return STR;
	else if (strcmp(ins, "RTI") == 0) return RTI;
	else if (strcmp(ins, "NOT") == 0) return NOT;
	else if (strcmp(ins, "LDI") == 0) return LDI;
	else if (strcmp(ins, "STI") == 0) return STI;
	else if (strcmp(ins, "JMP") == 0) return JMP;
	else if (strcmp(ins, "RET") == 0) return RET;
	else if (strcmp(ins, "LEA") == 0) return LEA;
	else if (strcmp(ins, "TRAP") == 0 || strcmp(ins, "HALT") == 0 || strcmp(ins, "IN") == 0 ||
		strcmp(ins, "OUT") == 0 || strcmp(ins, "GETC") == 0 || strcmp(ins, "PUTS") == 0 || strcmp(ins, "PUTSP") == 0)
		return TRAP;
	else return -1;     //directives
}

char* RegToBinString(char* reg) {
	char* regBinStr[] = { "000","001","010","011","100","101","110","111" };

	for (int i = 0; i < 8; i++) {
		if (*(reg + 1) == (i + '0')) {
			return regBinStr[i];
		}
	}
}

int GetLabelAddress(char* arg) {
	for (int i = 0; i < TableCount; i++) {
		if (strcmp(SymbolTable[i].symbol, arg) == 0) {
			return SymbolTable[i].address;
		}
	}
}

void FirstPass() {
	int i, j = 0, strhead = 0, strtail = 0;
	char* CodeToken = (char*)malloc(sizeof(char) * BUFLEN);
	char copy[BUFLEN];

	LC = GetStartLocation(AsmCode[0]);                     //initialize location counter

	for (i = 1; i < asmLines - 1; i++) {
		if (IfRealLabel(AsmCode[i])) {
			strcpy(copy, AsmCode[i]);
			CodeToken = strtok(copy, " ");
			SymbolTable[j].address = LC;
			strcpy(SymbolTable[j].symbol, CodeToken);      //construct symbol table
			j++;
		}
		if (strstr(AsmCode[i], ".STRINGZ") != NULL) {
			for (int j = 0; j < strlen(AsmCode[i]); j++) {
				if (AsmCode[i][j] == '\"' && strhead == 0) {
					strhead = j + 1;
					continue;
				}
				if (AsmCode[i][j] == '\"' && strhead != 0) {
					strtail = j;
					break;
				}
			}
			LC += strtail - strhead + 1;
			strtail = strhead = 0;
		}
		else if (strstr(AsmCode[i], ".BLKW") != NULL) {
			strcpy(copy, AsmCode[i]);
			CodeToken = strtok(copy, "#");
			CodeToken = strtok(NULL, "#");
			LC += atoi(CodeToken);
		}
		else LC++;
	}
	TableCount = j;
}

void SecondPass() {
	int i = 0, j = 0, k = 1;
	char* opcode = (char*)malloc(sizeof(char) * 4), copy[BUFLEN];
	char nzpBits[4];

	for (i = 0; i < 4; i++) {
		arg[i] = (char*)malloc(sizeof(char) * BUFLEN);
	}

	LC = GetStartLocation(AsmCode[0]);
	strcat(MachCode[0], DecToBinString(LC, 16));   //start of program

	for (i = 1; i < asmLines - 1; i++, k++) {
		PC = LC + 1;
		//get string of instruction
		strcpy(copy, AsmCode[i]);
		opcode = strtok(copy, " ,");
		if (IfRealLabel(AsmCode[i])) {
			opcode = strtok(NULL, " ,");
		}
		do {
			arg[j] = strtok(NULL, " ,");
			j++;
		} while (arg[j - 1] != NULL);      //get all arguments
		j = 0;               //set counter to 0

		switch (WhichIns(opcode)) {
		case BR:
			opcode = strtok(opcode, "BR");   //get nzp bits
			if (opcode == NULL || strcmp(opcode, "NZP") == 0) strcpy(nzpBits, "111");
			else if (strcmp(opcode, "NZ") == 0) strcpy(nzpBits, "110");
			else if (strcmp(opcode, "NP") == 0) strcpy(nzpBits, "101");
			else if (strcmp(opcode, "ZP") == 0) strcpy(nzpBits, "011");
			else if (strcmp(opcode, "N") == 0) strcpy(nzpBits, "100");
			else if (strcmp(opcode, "Z") == 0) strcpy(nzpBits, "010");
			else if (strcmp(opcode, "P") == 0) strcpy(nzpBits, "001");

			if (*arg[0] != '#') {
				PCoffset9 = GetLabelAddress(arg[0]) - PC;
				arg[0] = DecToBinString(PCoffset9, 9);
			}
			else arg[0] = DecToBinString(StringToNum(arg[0]), 9);

			strcat(MachCode[k], "0000");
			strcat(MachCode[k], nzpBits);
			strcat(MachCode[k], arg[0]);
			break;
		case ADD:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			arg[1] = RegToBinString(arg[1]);
			strcpy(src1, arg[1]);
			strcat(MachCode[k], "0001");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], src1);

			if (strncmp(arg[2], "R", 1) == 0) {       //ADD DR, SR1, SR2
				arg[2] = RegToBinString(arg[2]);
				strcpy(src2, arg[2]);
				strcat(MachCode[k], "000");
				strcat(MachCode[k], src2);
			}
			else {                                    //ADD DR SR1, imm5
				arg[2] = DecToBinString(StringToNum(arg[2]), 5);
				strcpy(imm5, arg[2]);
				strcat(MachCode[k], "1");
				strcat(MachCode[k], imm5);
			}
			break;
		case AND:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			arg[1] = RegToBinString(arg[1]);
			strcpy(src1, arg[1]);
			strcat(MachCode[k], "0101");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], src1);

			if (strncmp(arg[2], "R", 1) == 0) {       //AND DR, SR1, SR2
				arg[2] = RegToBinString(arg[2]);
				strcpy(src2, arg[2]);
				strcat(MachCode[k], "000");
				strcat(MachCode[k], src2);
			}
			else {                                    //AND DR SR1, imm5
				arg[2] = DecToBinString(StringToNum(arg[2]), 5);
				strcpy(imm5, arg[2]);
				strcat(MachCode[k], "1");
				strcat(MachCode[k], imm5);
			}
			break;
		case JMP:
			arg[0] = RegToBinString(arg[0]);
			strcpy(BaseR, arg[0]);
			strcat(MachCode[k], "1100");
			strcat(MachCode[k], "000");
			strcat(MachCode[k], BaseR);
			strcat(MachCode[k], "000000");
			break;
		case JSR:
			if (strcmp(opcode, "JSRR") == 0) {          //JSRR
				arg[0] = RegToBinString(arg[0]);
				strcpy(BaseR, arg[0]);
				strcat(MachCode[k], "0100");
				strcat(MachCode[k], "000");
				strcat(MachCode[k], BaseR);
				strcat(MachCode[k], "000000");
			}
			else {                                    //JSR
				if (*arg[0] != '#') {
					PCoffset11 = GetLabelAddress(arg[0]) - PC;
					arg[0] = DecToBinString(PCoffset11, 11);
				}
				else arg[0] = DecToBinString(StringToNum(arg[0]), 11);
				strcat(MachCode[k], "0100");
				strcat(MachCode[k], "1");
				strcat(MachCode[k], arg[0]);
			}
			break;
		case LD:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			if (*arg[1] != '#') {   //LD with label
				PCoffset9 = GetLabelAddress(arg[1]) - PC;
				arg[1] = DecToBinString(PCoffset9, 9);
			}
			else {   //LD with imm
				arg[1] = DecToBinString(StringToNum(arg[1]), 9);
			}

			strcat(MachCode[k], "0010");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], arg[1]);
			break;
		case LDI:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			if (*arg[1] != '#') {   //LDI with label
				PCoffset9 = GetLabelAddress(arg[1]) - PC;
				arg[1] = DecToBinString(PCoffset9, 9);
			}
			else {   //LDI with imm
				arg[1] = DecToBinString(StringToNum(arg[1]), 9);
			}

			strcat(MachCode[k], "1010");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], arg[1]);
			break;
		case LDR:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			arg[1] = RegToBinString(arg[1]);
			strcpy(BaseR, arg[1]);
			offset6 = StringToNum(arg[2]);
			arg[2] = DecToBinString(offset6, 6);

			strcat(MachCode[k], "0110");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], BaseR);
			strcat(MachCode[k], arg[2]);
			break;
		case LEA:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			if (*arg[1] != '#') {   //LEA with label
				PCoffset9 = GetLabelAddress(arg[1]) - PC;
				arg[1] = DecToBinString(PCoffset9, 9);
			}
			else {   //LEA with imm
				arg[1] = DecToBinString(StringToNum(arg[1]), 9);
			}

			strcat(MachCode[k], "1110");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], arg[1]);
			break;
		case NOT:
			arg[0] = RegToBinString(arg[0]);
			strcpy(dst, arg[0]);
			arg[1] = RegToBinString(arg[1]);
			strcpy(src1, arg[1]);

			strcat(MachCode[k], "1001");
			strcat(MachCode[k], dst);
			strcat(MachCode[k], src1);
			strcat(MachCode[k], "111111");
			break;
		case RET:     //JMP R7
			strcat(MachCode[k], "1100000111000000");
			break;
		case RTI:
			strcat(MachCode[k], "1000000000000000");
			break;
		case ST:
			arg[0] = RegToBinString(arg[0]);
			strcpy(src1, arg[0]);
			if (*arg[1] != '#') {   //ST with label
				PCoffset9 = GetLabelAddress(arg[1]) - PC;
				arg[1] = DecToBinString(PCoffset9, 9);
			}
			else arg[1] = DecToBinString(StringToNum(arg[1]), 9);

			strcat(MachCode[k], "0011");
			strcat(MachCode[k], src1);
			strcat(MachCode[k], arg[1]);
			break;
		case STI:
			arg[0] = RegToBinString(arg[0]);
			strcpy(src1, arg[0]);
			if (*arg[1] != '#') {   //STI with label
				PCoffset9 = GetLabelAddress(arg[1]) - PC;
				arg[1] = DecToBinString(PCoffset9, 9);
			}
			else arg[1] = DecToBinString(StringToNum(arg[1]), 9);

			strcat(MachCode[k], "1011");
			strcat(MachCode[k], src1);
			strcat(MachCode[k], arg[1]);
			break;
		case STR:
			arg[0] = RegToBinString(arg[0]);
			strcpy(src1, arg[0]);
			arg[1] = RegToBinString(arg[1]);
			strcpy(BaseR, arg[1]);
			offset6 = StringToNum(arg[2]);
			arg[2] = DecToBinString(offset6, 6);

			strcat(MachCode[k], "0111");
			strcat(MachCode[k], src1);
			strcat(MachCode[k], BaseR);
			strcat(MachCode[k], arg[2]);
			break;
		case TRAP:
			strcat(MachCode[k], "11110000");
			if (arg[0] == NULL) {     //example form: GETC
				if (strcmp(opcode, "GETC") == 0) {
					arg[0] = DecToBinString(StringToNum(trapvector[0]), 8);   //change trapvector to binary string
				}
				else if (strcmp(opcode, "OUT") == 0) {
					arg[0] = DecToBinString(StringToNum(trapvector[1]), 8);
				}
				else if (strcmp(opcode, "PUTS") == 0) {
					arg[0] = DecToBinString(StringToNum(trapvector[2]), 8);
				}
				else if (strcmp(opcode, "IN") == 0) {
					arg[0] = DecToBinString(StringToNum(trapvector[3]), 8);
				}
				else if (strcmp(opcode, "PUTSP") == 0) {
					arg[0] = DecToBinString(StringToNum(trapvector[4]), 8);
				}
				else {
					arg[0] = DecToBinString(StringToNum(trapvector[5]), 8);
				}
				strcat(MachCode[k], arg[0]);
			}
			else {      //example form: TRAP x25
				arg[0] = DecToBinString(StringToNum(arg[0]), 8);
				strcpy(trapvect8, arg[0]);
				strcat(MachCode[k], trapvect8);
			}
			break;
		default:    //directives
			if (strcmp(opcode, ".FILL") == 0) {
				arg[0] = DecToBinString(StringToNum(arg[0]), 16);
				strcpy(imm16, arg[0]);
				strcat(MachCode[k], imm16);
			}
			else if (strcmp(opcode, ".BLKW") == 0) {
				for (int counter = 0; counter < StringToNum((arg[0])); counter++) {
					strcat(MachCode[k], "0111011101110111");    //x7777
					k++;
					LC++;
				}
				k--;
				LC--;
			}
			else if (strcmp(opcode, ".STRINGZ") == 0) {
				strcpy(copy, AsmCode[i]);
				int strhead = 0, strtail = 0, temp;

				for (int i = 0; i < strlen(copy); i++) {
					if (copy[i] == '\"' && strhead == 0) {
						strhead = i + 1;
						continue;
					}
					if (copy[i] == '\"' && strhead != 0) {
						strtail = i;
						break;
					}
				}
				for (int i = strhead; i < strtail; i++, k++) {
					temp = (int)copy[i];
					arg[0] = DecToBinString(temp, 16);
					strcat(MachCode[k], arg[0]);
					LC++;
				}
				strcat(MachCode[k], "0000000000000000");
				LC--;
			}
			break;
		}
		LC++;
	}
	machLines = k;
}

int main() {
	int i = 0, j = 0;
	char buf, temp[BUFLEN];

	buf = getchar();
	while (buf != EOF) {
		if (buf == '\n') {
			if (!IfBlankLine(i)) {
				i++;
				j = 0;
			}
			else {
				for (int k = 0; k < BUFLEN; k++) {
					AsmCode[i][k] = '\0';
				}
				j = 0;
			}
		}
		else {
			AsmCode[i][j] = buf;
			j++;
		}
		buf = getchar();
	}
	asmLines = i;

	for (int i = 0; i < asmLines; i++) {
		strcpy(temp, AsmCode[i]);
		LowerToUpper(temp);
		if (strstr(temp, ".STRINGZ") == NULL) {
			LowerToUpper(AsmCode[i]);
		}
		else {
			for (int j = 0; j < strlen(AsmCode[i]); j++) {
				if (AsmCode[i][j] != '\"') {
					if (AsmCode[i][j] >= 'a' && AsmCode[i][j] <= 'z') {
						AsmCode[i][j] = AsmCode[i][j] - 'a' + 'A';
					}
				}
				else break;
			}
		}
	}  //convert to upper case for check

	FirstPass();
	SecondPass();

	for (int i = 0; i < machLines; i++) {
		printf("%s\n", MachCode[i]);
	}

	system("pause");

	return 0;
}