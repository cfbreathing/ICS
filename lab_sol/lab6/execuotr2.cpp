#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
const int overflow = 65536;

int bstoi(string str){
    int ret = 0;
    for(auto it = str.begin();it != str.end();++it){
        ret <<= 1;
        ret += *it - '0';
    }
    // cerr << "btoi: " << str << " = " << ret << "\n";
    return ret;
}

string ext_bin_str[2] = {"0000000000000000", "1111111111111111"};
int exbstoi(string str){
    int ret = 0;
    string exstr;
    exstr.append( ext_bin_str[ str[0] - '0' ].substr(0, 16-str.length()) );
    exstr.append(str);
    ret = bstoi(exstr);
    // cerr << "exbtoi: " << str << " i.e. " << exstr << " = " << ret << "\n";
    return ret;
}

string itobs(int x){
    string ret = ext_bin_str[0];
    for(int i = 15;i >= 0 && x;--i){
        if(x&1) ret[i] = '1';
        x >>= 1;
    }
    // cerr << "itob: " << x << " = " << ret << "\n";
    return ret;
}

class LC3_PC{
private:
    int memLocation;
    int vecLocation;
public:
    LC3_PC(){
        cerr << "LC3_PC(): No! You shouldn't see this!\n";
    }
    LC3_PC(int ml, int vl = 0): memLocation(ml), vecLocation(vl){}
    ~LC3_PC(){}
    int getMPC(){
        return memLocation;
    }
    int getVPC(){
        return vecLocation;
    }
    void setMPC(int ml){
        vecLocation += ml - memLocation;
        memLocation = ml;
    }
    void setVPC(int vl){
        memLocation += vl - vecLocation;
        vecLocation = vl;
    }
    void inc(){
        ++memLocation, ++vecLocation;
    }
};

class LC3_registers{
private:
    int val;
public:
    LC3_registers(): val(0x7777){}
    ~LC3_registers(){}
    int getVal(){
        return val = (val % overflow + overflow) % overflow;
    }
    void setVal(int val){
        this->val = (val % overflow + overflow) % overflow;
    }
};

struct LC3_env{
    bool CCn, CCz, CCp, alive;
    LC3_PC pc;
    vector< LC3_registers > reg;
    LC3_env(): 
        CCn(false), CCz(true), CCp(false), reg(8), alive(false){}
    LC3_env(int orig): 
        CCn(false), CCz(true), CCp(false), reg(8), pc(orig), alive(false){}
    ~LC3_env(){}
    void setCC(int x){
        CCn = CCz = CCp = false;
        if(x & 0x8000){
            CCn = true;
        } else if(!x) {
            CCz = true;
        } else {
            CCp = true;
        }
    }
};

class LC3_instructions{
protected:
    string content;
    int location;
    int val;
public:
    LC3_instructions(){
        cerr << "LC3_instructions(): No! You shouldn't see this!\n";
    }
    LC3_instructions(string ins, int loc = 0): content(ins), location(loc){
        val = bstoi( content.substr(0,16) );
    }
    ~LC3_instructions(){}
    int getVal(){
        return val;
    }
    string getContent(){
        return content;
    }
    int getLoc(){
        return location;
    }
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){}
};

LC3_instructions * routing(string, int);

class LC3_ins_ADD : public LC3_instructions{
protected:
    int dr, sr1, sr2;
    int imm;
    bool useImm;
public:
    LC3_ins_ADD(){}
    LC3_ins_ADD(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        sr1 = bstoi( ins.substr(7,3) );
        useImm = ins[10] == '1';
        if(useImm){
            imm = exbstoi( ins.substr(11,5) );
        } else {
            sr2 = bstoi( ins.substr(13,3) );
        }
    }
    ~LC3_ins_ADD(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int res = env.reg[sr1].getVal();
        if(useImm){
            res = (res + imm) % overflow;
        } else {
            res = (res + env.reg[sr2].getVal()) % overflow;
        }
        env.reg[dr].setVal(res);
        env.setCC(res);
    }
};
 
class LC3_ins_AND : public LC3_instructions{
protected:
    int dr, sr1, sr2;
    int imm;
    bool useImm;
public:
    LC3_ins_AND(){}
    LC3_ins_AND(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        sr1 = bstoi( ins.substr(7,3) );
        useImm = ins[10] == '1';
        if(useImm){
            imm = exbstoi( ins.substr(11,5) );
        } else {
            sr2 = bstoi( ins.substr(13,3) );
        }
    }
    ~LC3_ins_AND(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int res = env.reg[sr1].getVal();
        if(useImm){
            res &= imm;
        } else {
            res &= env.reg[sr2].getVal();
        }
        env.reg[dr].setVal(res);
        env.setCC(res);
    }
};

class LC3_ins_BR : public LC3_instructions{
protected:
    bool n, z, p;
    int offset;
public:
    LC3_ins_BR(){}
    LC3_ins_BR(string & ins, int loc = 0): LC3_instructions(ins, loc){
        n = ins[4] - '0';
        z = ins[5] - '0';
        p = ins[6] - '0';
        offset = exbstoi( ins.substr(7,9) );
    }
    ~LC3_ins_BR(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        if( (env.CCn && n) ||
            (env.CCz && z) ||
            (env.CCp && p) ){
            int newPC = (env.pc.getMPC() + offset) % overflow;
            env.pc.setMPC( newPC );
        }
    }
};

class LC3_ins_JMP : public LC3_instructions{
protected:
    int base;
public:
    LC3_ins_JMP(){}
    LC3_ins_JMP(string & ins, int loc = 0): LC3_instructions(ins, loc){
        base = bstoi( ins.substr(7,3) );
    }
    ~LC3_ins_JMP(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int newPC = env.reg[base].getVal();
        env.pc.setMPC( newPC );
    }
};

class LC3_ins_JSR : public LC3_instructions{
protected:
    int offset, base;
    bool useOffset;
public:
    LC3_ins_JSR(){}
    LC3_ins_JSR(string & ins, int loc = 0): LC3_instructions(ins, loc){
        useOffset = ins[4] - '0';
        if(useOffset){
            offset = exbstoi( ins.substr(5,11) );
        } else {
            base = bstoi( ins.substr(7,3) );
        }
    }
    ~LC3_ins_JSR(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        env.reg[7].setVal( env.pc.getMPC() );
        int newPC;
        if(useOffset){
            newPC = (env.pc.getMPC() + offset) % overflow;
        } else {
            newPC = env.reg[base].getVal();
        }
        env.pc.setMPC( newPC );
    }
};

class LC3_ins_LD : public LC3_instructions{
protected:
    int dr, offset;
public:
    LC3_ins_LD(){}
    LC3_ins_LD(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        offset = exbstoi( ins.substr(7,9) );
    }
    ~LC3_ins_LD(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int dataLoc = (env.pc.getMPC() + offset) % overflow;
        auto dpc = env.pc;
        dpc.setMPC(dataLoc);
        int newVal = insList[ dpc.getVPC() ]->getVal();
        env.reg[dr].setVal( newVal );
        env.setCC(newVal);
    }
};

class LC3_ins_LDI : public LC3_instructions{
protected:
    int dr, offset;
public:
    LC3_ins_LDI(){}
    LC3_ins_LDI(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        offset = exbstoi( ins.substr(7,9) );
    }
    ~LC3_ins_LDI(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int dataLoc = (env.pc.getMPC() + offset) % overflow;
        auto dpc = env.pc;
        dpc.setMPC(dataLoc);
        dataLoc = insList[ dpc.getVPC() ]->getVal();
        dpc.setMPC(dataLoc);
        int newVal = insList[ dpc.getVPC() ]->getVal();
        env.reg[dr].setVal( newVal );
        env.setCC(newVal);
    }
};

class LC3_ins_LDR : public LC3_instructions{
protected:
    int dr, base, offset;
public:
    LC3_ins_LDR(){}
    LC3_ins_LDR(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        base = bstoi( ins.substr(7,3) );
        offset = exbstoi( ins.substr(10,6) );
    }
    ~LC3_ins_LDR(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int dataLoc = (env.reg[base].getVal() + offset) % overflow;
        auto dpc = env.pc;
        dpc.setMPC(dataLoc);
        int newVal = insList[ dpc.getVPC() ]->getVal();
        env.reg[dr].setVal( newVal );
        env.setCC(newVal);
    }
};

class LC3_ins_LEA : public LC3_instructions{
protected:
    int dr, offset;
public:
    LC3_ins_LEA(){}
    LC3_ins_LEA(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        offset = exbstoi( ins.substr(7,9) );
    }
    ~LC3_ins_LEA(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int newVal = (env.pc.getMPC() + offset) % overflow;
        env.reg[dr].setVal( newVal );
        // env.setCC(newVal); LEA doesn't setCC
    }
};

class LC3_ins_NOT : public LC3_instructions{
protected:
    int dr, sr;
public:
    LC3_ins_NOT(){}
    LC3_ins_NOT(string & ins, int loc = 0): LC3_instructions(ins, loc){
        dr = bstoi( ins.substr(4,3) );
        sr = bstoi( ins.substr(7,3) );
    }
    ~LC3_ins_NOT(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int res = ( ~env.reg[sr].getVal() ) % overflow;
        env.reg[dr].setVal(res);
        env.setCC(res);
    }
};

class LC3_ins_ST : public LC3_instructions{
protected:
    int sr, offset;
public:
    LC3_ins_ST(){}
    LC3_ins_ST(string & ins, int loc = 0): LC3_instructions(ins, loc){
        sr = bstoi( ins.substr(4,3) );
        offset = exbstoi( ins.substr(7,9) );
    }
    ~LC3_ins_ST(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int dataLoc = (env.pc.getMPC() + offset) % overflow;
        auto dpc = env.pc;
        dpc.setMPC(dataLoc);
        int newVal = env.reg[sr].getVal();
        LC3_instructions * newInsPtr = new LC3_instructions(itobs(newVal), dpc.getMPC());
        insList[ dpc.getVPC() ] = newInsPtr;
    }
};

class LC3_ins_STI : public LC3_instructions{
protected:
    int sr, offset;
public:
    LC3_ins_STI(){}
    LC3_ins_STI(string & ins, int loc = 0): LC3_instructions(ins, loc){
        sr = bstoi( ins.substr(4,3) );
        offset = exbstoi( ins.substr(7,9) );
    }
    ~LC3_ins_STI(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int dataLoc = (env.pc.getMPC() + offset) % overflow;
        auto dpc = env.pc;
        dpc.setMPC(dataLoc);
        dataLoc = insList[ dpc.getVPC() ]->getVal();
        dpc.setMPC(dataLoc);
        int newVal = env.reg[sr].getVal();
        LC3_instructions * newInsPtr = new LC3_instructions(itobs(newVal), dpc.getMPC());
        insList[ dpc.getVPC() ] = newInsPtr;
    }
};

class LC3_ins_STR : public LC3_instructions{
protected:
    int sr, base, offset;
public:
    LC3_ins_STR(){}
    LC3_ins_STR(string & ins, int loc = 0): LC3_instructions(ins, loc){
        sr = bstoi( ins.substr(4,3) );
        base = bstoi( ins.substr(7,3) );
        offset = exbstoi( ins.substr(10,6) );
    }
    ~LC3_ins_STR(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        int dataLoc = (env.reg[base].getVal() + offset) % overflow;
        auto dpc = env.pc;
        dpc.setMPC(dataLoc);
        int newVal = env.reg[sr].getVal();
        delete insList[ dpc.getVPC() ];
        insList[ dpc.getVPC() ] = new LC3_instructions(itobs(newVal), dpc.getMPC());
    }
};

class LC3_ins_HALT : public LC3_instructions{
public:
    LC3_ins_HALT(){}
    LC3_ins_HALT(int loc): LC3_instructions("1111000000100101", loc){}
    ~LC3_ins_HALT(){}
    virtual void exec(LC3_env &env, vector< LC3_instructions * > &insList){
        env.alive = false;
    }
};

LC3_instructions * routing(string ins, int loc){
    LC3_instructions * ret;
    string opcode = ins.substr(0,4);
    if(opcode == "0001"){
        ret = new LC3_ins_ADD(ins, loc);
    } else if(opcode == "0101"){
        ret = new LC3_ins_AND(ins, loc);
    } else if(opcode == "0000"){
        ret = new LC3_ins_BR(ins, loc);
    } else if(opcode == "1100"){
        ret = new LC3_ins_JMP(ins, loc);
    } else if(opcode == "0100"){
        ret = new LC3_ins_JSR(ins, loc);
    } else if(opcode == "0010"){
        ret = new LC3_ins_LD(ins, loc);
    } else if(opcode == "1010"){
        ret = new LC3_ins_LDI(ins, loc);
    } else if(opcode == "0110"){
        ret = new LC3_ins_LDR(ins, loc);
    } else if(opcode == "1110"){
        ret = new LC3_ins_LEA(ins, loc);
    } else if(opcode == "1001"){
        ret = new LC3_ins_NOT(ins, loc);
    } else if(opcode == "1000"){
        // Don't require.
        // ret = new LC3_ins_RTI(ins, loc);
    } else if(opcode == "0011"){
        ret = new LC3_ins_ST(ins, loc);
    } else if(opcode == "1011"){
        ret = new LC3_ins_STI(ins, loc);
    } else if(opcode == "0111"){
        ret = new LC3_ins_STR(ins, loc);
    } else if(ins.substr(0,16) == "1111000000100101"){
        ret = new LC3_ins_HALT(loc);
    } else {
        ret = new LC3_instructions(ins, loc);
    }
    return ret;
}

class LC3{
private:
    LC3_env env;
    vector< LC3_instructions * > ins;
public:
    LC3(){
        cerr << "LC3(): No! You shouldn't see this!\n";
    }
    LC3(vector< string > vs): env( bstoi(vs[0].substr(0,16)) ){
        int locPtr = env.pc.getMPC();
        for(auto it = vs.begin()+1; it != vs.end();++it){
            auto insTmp = new LC3_instructions(*it, locPtr++);
            this->ins.push_back(insTmp);
        }
        // Maybe it can be dynamic.
        ins.resize(1000000);
    }
    ~LC3(){}
    void run(){
        env.alive = true;
        while(env.alive){
            auto ins_to_load = ins[ env.pc.getVPC() ];
            auto ins_to_exec = routing(ins_to_load->getContent(), ins_to_load->getLoc());
            env.pc.inc();
            cerr << "Now running [ " << ins_to_exec->getContent() << " ]\n";
            ins_to_exec->exec(env, ins);
            delete ins_to_exec;
        }
    }
    void regMonitor(){
        for(int i = 0;i < 8;++i){
            cout << "R" << i << " = x" << setiosflags(ios::uppercase) << setw(4) << setfill('0') << hex << env.reg[i].getVal() << dec << "\n";
        }
    }
};

int main(){
    vector< string > instructionsStrings(0);
    string str;
    while(cin >> str){
        instructionsStrings.push_back(str);
    }
    LC3 lc(instructionsStrings);
    // cerr << "LC3 init finished.\n";
    lc.run();
    // cerr << "LC3 running finished.\n";
    lc.regMonitor();
    return 0;
}