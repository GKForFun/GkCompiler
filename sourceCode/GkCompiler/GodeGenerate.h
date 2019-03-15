#ifndef _CODEGEN_H
#define _CODEGEN_H
#include "GkCompiler_Global.h"
#include "Syntax_Analyzer.h"

enum OP_TYPE
{
	ADD,
	SUB,
	MUL,
	DIV,
	XOR,
	AND,
	OR,
	ASSIGN,
	UMINUS,
	JE,
	JNE,
	JLE,
	JL,
	JGE,
	JG,
	JMP,
	LABEL,
	PUSH,
	POP,
	CALL,
	INDEX,
	NOT,
	RETURN
};

//basic operation
class CBaseOpGen:public CBaseFunction
{
public:
	void CodeGen();
	void GetParam(string op1, string op2, OP_TYPE Operator, string Result);
	string GetOperator();
	int JudgeOperator(string Operator);
	int GetOppositeOp(string Operator);
	CBaseOpGen();
	CBaseOpGen(string op1, string op2, OP_TYPE Operator,string Result) :arg1(op1), arg2(op2), op(Operator), result(Result) {}
private:
	string arg1;
	string arg2;
	string result;
	OP_TYPE op;
};


/*
mov mem,judge
jmp LABEL2
LABEL1:
while code.....
jmp LABEL3(break)
LABEL2:
cmp mem,out_condition
jX LABEL1
LABEL3:

*/
// 'subfunc' structure assemble code generation
class CFuncOpGen :public CBaseFunction			//function assemble code generate
{
public:
	void StartCodeGen();
	void EndCodeGen();
	void MainStartCodeGen();
	void MainEndCodeGen();
	void GetFunName(string name);
	void GetParameter(string param);
	void GetOffset(int espOffset);
private:
	string funcName;
	vector<string> paramList;				//from right to left
	int offset;
	int charNum;
	int intNum;
};

class CGlobalVarGen 
{
public:
	void CodeGen();
	void GetParam(WordType type,string name,string value);
private:
	string varName;
	WordType varType;
	vector<string> varValue;
	ofstream globalFile;
};

class FinalCodeGenerate:public CBaseFunction
{
public:
	void ReadMidFileAndWrite();
	//void WriteFinalFile(string name);
private:
	ifstream inFile;
	ofstream outFile;
	int espOffset;
};
#endif // !_CODEGEN_H
