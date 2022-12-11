#include <math.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
using namespace std;
// global variable
map<string, int> SA;  // the map that represents the symbol_address table
int start_address;    // the address after .ORIG
int current_address;  // record the address of the current instruction

// return the first word(divided by space) in the sentence
string get_first_word(string str) {
    int i = 0, j = 0, k = 0;

    while (str[i] == ' ')
        i++;
    string ret = "";
    for (k = 0, j = i; str[j] != ' ' && str[j] != '\0'; k++, j++) {
        ret.resize(k + 1);
        ret[k] = str[j];
    }

    ret[k] = '\0';
    return ret;
}
// return 16^n
int favorite(int n) {
    int i;
    long total = 1;
    if (n >= 1) {
        for (i = 0; i < n; i++) {
            total *= 16;
        }
    }
    return total;
}
// turn a character to the correspond number in hex representation
int hex2num(char ch) {
    int value = 0;
    if (ch >= '0' && ch <= '9') {
        value = ch - 48;
    }
    if (ch >= 'A' && ch <= 'F') {
        value = ch - 65 + 10;
    }
    if (ch >= 'a' && ch <= 'f') {
        value = ch - 97 + 10;
    }
    return value;
}
// translate a hex string to it's value
int atoi_hex(string strInValue) {
    strInValue = get_first_word(strInValue);
    int j = 0;
    int flag = 1;
    if (strInValue[0] == '-') {
        flag = -1;
        j++;
    }
    char hex[9];
    char str[9];
    int cnt = 0, i = 0;
    int sum = 0;
    while (strInValue[j] != '\0' && strInValue[j] != ' ') {
        hex[cnt] = strInValue[j];
        cnt++;
        j++;
    }
    hex[cnt] = '\0';
    for (i = 0; i < cnt; i++) {
        str[i] = hex[cnt - 1 - i];
    }
    str[cnt] = '\0';
    for (i = 0; i < cnt; i++) {
        sum += favorite(i) * hex2num(str[i]);
    }
    return flag * sum;
}

int s_find(string str,
           char c,
           int time)  // This function return the position of the time'th
                      // character 'c' in the string str
{
    int ret = -1;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == c && --time <= 0) {
            ret = i;
            break;
        }
    }
    return ret;
}

// change a decimical number n to it's "digit" digits 2's complements
string dec_to_2com(int n, int digit) {
    string imm;
    int temp = n;
    imm.resize(digit + 1);
    imm.assign(digit, '0');
    int i = digit - 1;
    if (temp >= 0) {
        while (n != 0) {
            if (i < 0)
                return NULL;
            imm[i--] = n % 2 + '0';
            n /= 2;
        }
    } else if (temp < 0) {
        n = -(temp + 1);
        while (n != 0) {
            if (i < 0)
                return NULL;
            imm[i--] = n % 2 + '0';
            n /= 2;
        }

        for (int r = 0; r < digit; r++)
            imm[r] = '1' - imm[r] + '0';
    }
    imm[digit] = '\0';
    return imm;
}
// change a hexadecimical number n to it's "digit" digits 2's complements
string hex_to_2com(int n, int digit) {
    string imm;
    int temp = n;

    imm.resize(digit + 1);
    imm.assign(digit, '0');
    int i = digit - 1;
    if (temp >= 0) {
        while (n != 0) {
            if (i < 0)
                return NULL;
            imm[i--] = n % 2 + '0';
            n /= 2;
        }
    } else if (temp < 0) {
        n = -(temp + 1);
        while (n != 0) {
            if (i < 0)
                return NULL;
            imm[i--] = n % 2 + '0';
            n /= 2;
        }

        for (int r = 0; r < digit; r++)
            imm[r] = '1' - imm[r] + '0';  //ת���ɷ���
    }
    imm[digit] = '\0';
    return imm;
}

//-------------------------------------------------------------------------------------------------------
// translate each instrutions(from a lc-3 code line string to it's machine code
// string(end with \n)) mainly by handlding the operation on STRING
string trans_ADD(string sen) {
    string code;
    code.resize(18);
    code = "0001";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, 'R', 2) + 1;
    int SR1 = sen[d2] - '0';
    code.append(dec_to_2com(SR1, 3));

    int d3 = s_find(sen, '#', 1);
    if (d3 != -1) {
        code.append("1");
        int imm5 = atoi(sen.substr(d3 + 1, 3).c_str());
        code.append(dec_to_2com(imm5, 5));
    }
    int d4 = s_find(sen, 'X', 1);
    if (d4 != -1) {
        code.append("1");
        int imm5 = atoi_hex(sen.substr(d4 + 1, 3).c_str());
        code.append(dec_to_2com(imm5, 5));
    } else if (d3 == -1 && d4 == -1) {
        code.append("000");
        d3 = s_find(sen, 'R', 3) + 1;
        int SR3 = sen[d3] - '0';
        code.append(dec_to_2com(SR3, 3));
    }
    code.append("\n");
    return code;
}
string trans_AND(string sen) {
    string code;
    code.resize(18);
    code = "0101";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, 'R', 2) + 1;
    int SR1 = sen[d2] - '0';
    code.append(dec_to_2com(SR1, 3));

    int d3 = s_find(sen, '#', 1);
    if (d3 != -1) {
        code.append("1");
        int imm5 = atoi(sen.substr(d3 + 1, 3).c_str());
        code.append(dec_to_2com(imm5, 5));
    }
    int d4 = s_find(sen, 'X', 1);
    if (d4 != -1) {
        code.append("1");
        int imm5 = atoi_hex(sen.substr(d4 + 1, 3).c_str());
        code.append(dec_to_2com(imm5, 5));
    } else if (d3 == -1 && d4 == -1) {
        code.append("000");
        d3 = s_find(sen, 'R', 3) + 1;
        int SR3 = sen[d3] - '0';
        code.append(dec_to_2com(SR3, 3));
    }
    code.append("\n");
    return code;
}
string trans_NOT(string sen) {
    string code;
    code.resize(18);
    code = "1001";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, 'R', 2) + 1;
    int SR = sen[d2] - '0';
    code.append(dec_to_2com(SR, 3));
    code.append("111111");
    code.append("\n");
    return code;
}
string trans_LD(string sen) {
    string code;
    code.resize(18);
    code = "0010";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, '#', 1);
    int off9;
    if (d2 != -1) {
        off9 = atoi(sen.substr(d2 + 1, 4).c_str());
    } else {
        int d3 = s_find(sen, ',', 1) + 1;
        string last = sen.substr(d3, sen.length() - d3);
        string label = get_first_word(last);
        int address = 3000;

        if (SA.find(label) != SA.end()) {
            address = SA.find(label)->second;
        }
        off9 = address - current_address;
    }
    code.append(dec_to_2com(off9, 9));
    code.append("\n");
    return code;
}
string trans_LDR(string sen) {
    string code;
    code.resize(18);
    code = "0110";
    int d1 = s_find(sen, 'R', 2) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, 'R', 3) + 1;
    int BR = sen[d2] - '0';
    code.append(dec_to_2com(BR, 3));

    int d3 = s_find(sen, '#', 1);
    if (d3 != -1) {
        int imm6 = atoi(sen.substr(d3 + 1, 3).c_str());
        code.append(dec_to_2com(imm6, 6));
    }
    int d4 = s_find(sen, 'X', 1);
    if (d4 != -1) {
        int imm6 = atoi_hex(sen.substr(d4 + 1, 3).c_str());
        code.append(dec_to_2com(imm6, 6));
    }
    code.append("\n");
    return code;
}
string trans_LDI(string sen) {
    string code;
    code.resize(18);
    code = "1010";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, '#', 1);
    int off9;
    if (d2 != -1) {
        off9 = atoi(sen.substr(d2 + 1, 4).c_str());
    } else {
        int d3 = s_find(sen, ',', 1) + 1;
        string last = sen.substr(d3, sen.length() - d3);
        string label = get_first_word(last);
        int address = 3000;

        if (SA.find(label) != SA.end()) {
            address = SA.find(label)->second;
        }
        off9 = address - current_address;
    }
    code.append(dec_to_2com(off9, 9));
    code.append("\n");
    return code;
}
string trans_LEA(string sen) {
    string code;
    code.resize(18);
    code = "1110";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, '#', 1);
    int off9;
    if (d2 != -1) {
        off9 = atoi(sen.substr(d2 + 1, 4).c_str());
    } else {
        int d3 = s_find(sen, ',', 1) + 1;
        string last = sen.substr(d3, sen.length() - d3);
        string label = get_first_word(last);
        int address = 3000;

        if (SA.find(label) != SA.end()) {
            address = SA.find(label)->second;
        }
        off9 = address - current_address;
    }
    code.append(dec_to_2com(off9, 9));
    code.append("\n");
    return code;
}
string trans_ST(string sen) {
    string code;
    code.resize(18);
    code = "0011";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, '#', 1);
    int off9;
    if (d2 != -1) {
        off9 = atoi(sen.substr(d2 + 1, 4).c_str());
    } else {
        int d3 = s_find(sen, ',', 1) + 1;
        string last = sen.substr(d3, sen.length() - d3);
        string label = get_first_word(last);
        int address = 3000;

        if (SA.find(label) != SA.end()) {
            address = SA.find(label)->second;
        }
        off9 = address - current_address;
    }
    code.append(dec_to_2com(off9, 9));
    code.append("\n");
    return code;
}
string trans_STR(string sen) {
    string code;
    code.resize(18);
    code = "0111";
    int d1 = s_find(sen, 'R', 2) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, 'R', 3) + 1;
    int BR = sen[d2] - '0';
    code.append(dec_to_2com(BR, 3));

    int d3 = s_find(sen, '#', 1);
    if (d3 != -1) {
        int imm6 = atoi(sen.substr(d3 + 1, 3).c_str());
        code.append(dec_to_2com(imm6, 6));
    }
    int d4 = s_find(sen, 'X', 1);
    if (d4 != -1) {
        int imm6 = atoi_hex(sen.substr(d4 + 1, 3).c_str());
        code.append(dec_to_2com(imm6, 6));
    }
    code.append("\n");
    return code;
}
string trans_STI(string sen) {
    string code;
    code.resize(18);
    code = "1011";
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    int d2 = s_find(sen, '#', 1);
    int off9;
    if (d2 != -1) {
        off9 = atoi(sen.substr(d2 + 1, 4).c_str());
    } else {
        int d3 = s_find(sen, ',', 1) + 1;
        string last = sen.substr(d3, sen.length() - d3);
        string label = get_first_word(last);
        int address = 3000;

        if (SA.find(label) != SA.end()) {
            address = SA.find(label)->second;
        }
        off9 = address - current_address;
    }
    code.append(dec_to_2com(off9, 9));
    code.append("\n");
    return code;
}
string trans_JMP(string sen) {
    string code;
    code.resize(18);
    code = "1100";
    code.append("000");
    int d1 = s_find(sen, 'R', 1) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    code.append("000000");
    code.append("\n");
    return code;
}
string trans_JSRR(string sen) {
    string code;
    code.resize(18);
    code = "0100";
    code.append("000");
    int d1 = s_find(sen, 'R', 3) + 1;
    int DR = sen[d1] - '0';
    code.append(dec_to_2com(DR, 3));
    code.append("000000");
    code.append("\n");
    return code;
}
string trans_JSR(string sen) {
    string code;
    code.resize(18);
    code = "01001";
    int off11;

    int d = s_find(sen, 'R', 1) + 1;

    string last = sen.substr(d, sen.length() - d);
    string label = get_first_word(last);
    int address = 3000;

    if (SA.find(label) != SA.end()) {
        address = SA.find(label)->second;
    }
    off11 = address - current_address;
    code.append(dec_to_2com(off11, 11));
    code.append("\n");
    return code;
}
string trans_RTI(string sen) {
    string code;
    code.resize(18);
    code = "1000000000000000";
    code.append("\n");
    return code;
}
string trans_RET(string sen) {
    string code;
    code.resize(18);
    code = "1100000111000000";
    code.append("\n");
    return code;
}

string trans_BR(string sen) {
    string code;
    code.resize(18);
    code = "0000";
    int NZP = 0;
    string br_part = get_first_word(sen);

    if (s_find(br_part, 'N', 1) != -1) {
        NZP += 4;
    }
    if (s_find(br_part, 'Z', 1) != -1) {
        NZP += 2;
    }
    if (s_find(br_part, 'P', 1) != -1) {
        NZP += 1;
    }
    if (s_find(br_part, 'N', 1) == -1 && s_find(br_part, 'Z', 1) == -1 &&
        s_find(br_part, 'P', 1) == -1) {
        NZP = 7;
    }
    code.append(dec_to_2com(NZP, 3));

    int d2 = s_find(sen, '#', 1);
    int off9;
    if (d2 != -1) {
        off9 = atoi(sen.substr(d2 + 1, 4).c_str());
    } else {
        int d3 =
            max(max(s_find(br_part, 'R', 1) + 1, s_find(br_part, 'N', 1) + 1),
                max(s_find(br_part, 'Z', 1) + 1, s_find(br_part, 'P', 1) + 1)) +
            s_find(sen, 'B', 1);
        string last = sen.substr(d3, sen.length() - d3);
        string label = get_first_word(last);
        int address = 3000;

        if (SA.find(label) != SA.end()) {
            address = SA.find(label)->second;
        }
        off9 = address - current_address;
    }

    code.append(dec_to_2com(off9, 9));
    code.append("\n");
    return code;
}
string trans_TRAP(string sen) {
    string code;
    code.resize(18);
    code = "11110000";
    string fw = get_first_word(sen);
    string trap_vector8 = "00000000";
    if (fw == "GETC")
        trap_vector8 = dec_to_2com(0x20, 8);
    else if (fw == "OUT")
        trap_vector8 = dec_to_2com(0x21, 8);
    else if (fw == "PUTS")
        trap_vector8 = dec_to_2com(0x22, 8);
    else if (fw == "IN")
        trap_vector8 = dec_to_2com(0x23, 8);
    else if (fw == "PUTSP")
        trap_vector8 = dec_to_2com(0x24, 8);
    else if (fw == "HALT")
        trap_vector8 = dec_to_2com(0x25, 8);
    else  // written in "TRAP vector8" formula
    {
        int d = s_find(sen, 'X', 1) + 1;
        trap_vector8 =
            dec_to_2com(atoi_hex(sen.substr(d, sen.length() - d).c_str()), 8);
    }
    code.append(trap_vector8);
    code.append("\n");
    return code;
}
// pre
string trans_FILL(string sen) {
    string code = "";
    code.resize(18);
    int imm16 = 0;
    int d = s_find(sen, '#', 1);
    if (d != -1)
        imm16 = atoi(sen.substr(d + 1, sen.length() - d).c_str());
    else if (s_find(sen, 'X', 1) != -1) {
        d = s_find(sen, 'X', 1);
        imm16 = atoi_hex(sen.substr(d + 1, sen.length() - d).c_str());
    } else
        return NULL;  // Error(FILL LABEL)

    code.append(dec_to_2com(imm16, 16));
    code.append("\n");
    return code;
}
string trans_BLKW(string sen) {
    string line = "";
    line.resize(18);
    line = "0111011101110111\n";
    int d = s_find(sen, '#', 1);
    int imm16 = atoi(sen.substr(d + 1, sen.length() - d).c_str());
    string codes = "";
    codes.resize(d * 17 + 1);
    for (int i = 0; i < imm16; i++) {
        codes.append(line);
    }

    return codes;
}
string trans_STRINGZ(string sen) {
    string codes = "";
    int d1 = s_find(sen, '"', 1) + 1;
    int d2 = s_find(sen, '"', 2) - 1;
    if (d1 > d2) {
        codes.resize(18);
        codes.append("0000000000000000\n");
        return codes;
    }
    string str = sen.substr(d1, d2 - d1 + 1);

    codes.resize(str.length() * 17 + 1);
    string line = "";
    line.resize(18);
    int i = 0;
    while (str[i] == '\0')
        i++;
    while (str[i] != '\0') {
        line = "";
        line.append(dec_to_2com((int)str[i], 16));
        line.append("\n");
        codes.append(line);
        i++;
    }
    line = "";
    line.append("0000000000000000\n");
    codes.append(line);
    return codes;
}
string trans_ORIG(string sen) {
    return trans_FILL(sen);
}

//-------------------------------------------------------------------------------------------------------

// a combining translating function, call the trans_ function according to
// "type" notice that "sen" is the lc-3 line that has been pretreat, type is
// usually its first word
string translate(string sen, string type) {
    if (type == "ADD")
        return trans_ADD(sen);
    else if (type == "AND")
        return trans_AND(sen);
    else if (type == "NOT")
        return trans_NOT(sen);
    else if (type == "LD")
        return trans_LD(sen);
    else if (type == "LDR")
        return trans_LDR(sen);
    else if (type == "LDI")
        return trans_LDI(sen);
    else if (type == "LEA")
        return trans_LEA(sen);
    else if (type == "ST")
        return trans_ST(sen);
    else if (type == "STR")
        return trans_STR(sen);
    else if (type == "STI")
        return trans_STI(sen);
    else if (type == "JMP")
        return trans_JMP(sen);
    else if (type == "JSRR")
        return trans_JSRR(sen);
    else if (type == "JSR")
        return trans_JSR(sen);
    else if (type == "RET")
        return trans_RET(sen);
    else if (type == "RTI")
        return trans_RTI(sen);
    else if (type == "BR" || type == "BRN" || type == "BRZ" || type == "BRP" ||
             type == "BRNZ" || type == "BRNP" || type == "BRZP" ||
             type == "BRNZP") {
        return trans_BR(sen);
    } else if (type == "TRAP" || type == "GETC" || type == "OUT" ||
               type == "PUTS" || type == "IN" || type == "PUTSP" ||
               type == "HALT")
        return trans_TRAP(sen);
    else if (type == ".FILL")
        return trans_FILL(sen);
    else if (type == ".BLKW")
        return trans_BLKW(sen);
    else if (type == ".STRINGZ")
        return trans_STRINGZ(sen);
    else if (type == ".ORIG")
        return trans_ORIG(sen);
    else
        return "Error!";
}

// very important procedure!
// Capitalization -> Comments Deleting ->Label Deleting and Symbol_Adrress table
// construction
void Pretreat(string& txt) {
    SA.clear();
    int first = 0;
    while (txt[first] == '\0')
        first++;
    int i = first, j = 0;
    // For the first traversal , turn all English letters in txt (except the
    // string in "") into uppercase to avoid trouble disadvantage: Lables can't
    // be distinguished by upper or lower case of it's character
    int flag = 1;
    while (txt[i] != '\0') {
        if (txt[i] >= 'a' && txt[i] <= 'z' && flag == 1) {
            txt[i] = txt[i] - ('a' - 'A');
        }
        if (txt[i] == '"')
            flag *= -1;
        i++;
    }

    // the second traversal, delete comments, delete labels and construct the
    // symbol_address table.
    i = first;
    string line = "";
    line.resize(300);
    current_address = -1;
    int temp_address = -1;
    while (txt[i] != '\0') {
        while (txt[i] != '\n' && txt[i] != '\0') {
            line[j] = txt[i];
            i++;
            j++;
        }
        line[j] = '\0';

        // delete comments
        for (int k = 0; k < line.length(); k++) {
            if (line[k] == ';') {
                txt.erase(i - j + k, j - k);
                line.erase(k, j - k);
                i -= (j - k);
                j -= (j - k);
                break;
            }
        }

        // construct symblo_address table
        // notice that .BLKW and .STRINGZ occupies may not only one location in
        // memory.
        string fw = get_first_word(line);
        if (fw[0] != '\0')
            temp_address = current_address + 1;

        if (fw[0] == '\0') {
            line.clear();
            line.resize(300);
            j = 0;
            if (txt[i] == '\n')
                i++;
            continue;
        } else if (fw == ".BLKW" ||
                   get_first_word(line.substr(fw.length() + 1,
                                              line.length() - fw.length())) ==
                       ".BLKW") {
            int d = s_find(line, '#', 1);
            int imm16 = atoi(line.substr(d + 1, 6).c_str());
            current_address += imm16;
        } else if (fw == ".STRINGZ" ||
                   get_first_word(line.substr(fw.length() + 1,
                                              line.length() - fw.length())) ==
                       ".STRINGZ") {
            int d1 = s_find(line, '"', 1) + 1;
            int d2 = s_find(line, '"', 2) - 1;

            current_address += d2 - d1 + 2;
        } else
            current_address++;

        if (fw == ".ORIG") {
            start_address =
                atoi_hex(line.substr(s_find(line, 'G', 1) + 1,
                                     line.length() - s_find(line, 'G', 1)));
            current_address = start_address - 1;
        }
        // If the first word is not the instruction set, it is listed in symbol_
        // Address table and deleted in the code
        else if (fw != "ADD" && fw != "AND" && fw != "NOT" && fw != "LD" &&
                 fw != "LDR" && fw != "LDI" && fw != "ST" && fw != "STR" &&
                 fw != "STI" && fw != "TRAP" && fw != "LEA" && fw != "RTI" &&
                 fw != "JMP" && fw != "JSR" && fw != "RET" && fw != "JSRR" &&
                 fw != ".FILL" && fw != ".STRINGZ" && fw != ".BLKW" &&
                 fw != "BR" && fw != "BRN" && fw != "BRZ" && fw != "BRP" &&
                 fw != "BRNZ" && fw != "BRNP" && fw != "BRZP" &&
                 fw != "BRNZP" && fw != "GETC" && fw != "PUTS" &&
                 fw != "PUTSP" && fw != "IN" && fw != "OUT" && fw != "HALT" &&
                 fw != ".END" && fw[0] != '\0') {
            SA.insert(make_pair(fw, temp_address));
            txt.erase(i - j + s_find(line, fw[0], 1), fw.length());
            i -= fw.length();
        } else if (fw == ".END") {
            txt.erase(i - j + s_find(line, fw[0], 1), fw.length());
            i -= fw.length();
        }

        line.clear();
        line.resize(300);
        j = 0;
        if (txt[i] == '\n')
            i++;
    }
}

// after pretreat, every line in txt can be direcly translated, then do the
// assemble
string Assemble(string txt) {
    string mach_code = "";
    mach_code.resize(10000);
    mach_code[0] = '\0';

    int first = 0;
    while (txt[first] == '\0')
        first++;
    int i = first, j = 0;
    string line = "";
    line.resize(300);
    current_address = start_address - 1;
    while (txt[i] != '\0') {
        while (txt[i] != '\n' && txt[i] != '\0') {
            line[j] = txt[i];
            i++;
            j++;
        }
        line[j] = '\0';
        // assemble every instruction
        string fw = get_first_word(line);
        if (fw[0] == '\0') {
            line.clear();
            line.resize(300);
            j = 0;
            if (txt[i] == '\n')
                i++;
            continue;
        } else if (fw == ".BLKW") {
            int d = s_find(line, '#', 1);
            int imm16 = atoi(line.substr(d + 1, 6).c_str());
            current_address += imm16;
        } else if (fw == ".STRINGZ") {
            int d1 = s_find(line, '"', 1) + 1;
            int d2 = s_find(line, '"', 2) - 1;
            current_address += d2 - d1 + 2;
        } else
            current_address++;

        string mach_line;  // this string is what we finally want to get
        mach_line.resize(18);
        mach_line = translate(line, get_first_word(fw));  // translation
        mach_code.append(mach_line);

        line.clear();
        line.resize(300);
        j = 0;
        if (txt[i] == '\n')
            i++;
    }

    return mach_code;
}

int main() {
    string lc3_txt, temp;
    lc3_txt.resize(10000);
    temp.resize(10000);
    int t = 0;
    // read from stdin until meeting EOF
    while (!((lc3_txt[t++] = getchar()) == '9'))
        ;
    lc3_txt[t] = '\0';

    // Pretreat the lc-3 code
    Pretreat(lc3_txt);
    // Assemble the lc-3 code that has been pretreat
    string machine_code = Assemble(lc3_txt);

    // transfer the string to "char*" mode, to use fputs.
    char txt_out[10000];
    int i = 0, j = 0;
    while (i < machine_code.size()) {
        if (machine_code[i] != '\0') {
            txt_out[j] = machine_code[i];
            j++;
        }
        i++;
    }
    txt_out[j] = '\0';

    fputs(txt_out, stdout);  // puts the result into stdout
}
