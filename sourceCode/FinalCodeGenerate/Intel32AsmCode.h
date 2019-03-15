#ifndef  X86ASM_H
#define X86ASM_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <list>
using namespace std;
enum ArgType
{
	REG,
	MEM,
	IMME
};
typedef struct FourElement
{
	string op;
	string arg1;
	string arg2;
	string result;
}fElement;

typedef struct TmpVar
{
	pair<int, int> range;
	string value;
};

typedef struct FuncInfo
{
	string name;
	int tmpNum;
	int espOffset;
	unsigned int start;
	unsigned int end;
	vector<string> param;
	vector<TmpVar> tmpVarList;
}funcInfo;

typedef struct GlobalVar
{
	string name;
	string type;
	vector<string> value;
}globalVar;
class CBaseInfo
{
public:
	__in  void ReadElementFromFile();
	__in  void GetTmpVarInfo();
	__out fElement GetElement(int index);
	__out funcInfo GetFuncInfo(int index);
	__out globalVar SearchGlobalVar(string name);
	__out int GetFuncNum();			//返回函数数量，包括主函数
	void GetTmpVarLifeRange(FuncInfo* func);		//初始化函数中的中间变量的信息
	string Int2string(int num);
private:
	vector<fElement> element;
	list<FuncInfo> function;
	vector<GlobalVar> globalVar;
	ifstream inFile;
	int funcNum = 0;
};


class CCodeGen :public CBaseInfo
{
public:
	void FuncHandle();		//函数处理
	void InitRegister();	//初始化寄存器
	void BaseCalcuOp();		//运算操作
	void AssignOp();
	void CmpAndJmp();
	void LabelPrint();
	void StackOp();
	void FuncOp();
	void ArrIndex();
	int GetIdleReg();		//获取空闲寄存器
	int GetArrBase(string base);
	string GetTmpVar();		//临时变量
	string GetRealArg(string arg);		//获取真实参数
	bool JudgeIndexVar(string arg);
	bool JudgeRegVar(string arg);		//判断是否为寄存器
	int GetRegIndex(string regName);	//获取寄存器索引
	bool JudgeMemVar(string arg);		//判断是否为内存变量
	string GetRealMemVarValue(string arg);	//获取内存变量真实值
	bool JudgeTmpVar(string arg);		//判断是否为中间变量
	string GetRealTmpVarValue(string arg);	//获取中间变量真实值
	int GetTmpVarIndex(string arg);
	bool JudgeGlobalVar(string arg);		//判断是否为全局变量
	string GetRealGlobalVarValue(string arg);	//获取全局变量真实值
	void FreeRegister(int index);
	void JudgeArgType();				//判断参数类型
	int JudgeArgLifeEnd();				//
	void BusyRegister(int index);
	void SetTmpVarValue(int index, string value);

	//fElement lastElement;
	fElement curElement;
	fElement nextElement;
	string realArg[2];			//注意用完后收尾，不要影响下一个四元式
	ArgType curArg[2];
	string realResult;
	vector<string> curFuncInsList;
	int arrIndexReg = 0;
	int regIndex[2] = { -1,-1 };
	int argLifeEnd[2] = { -1,-1 };
	int iCurElement;
private:
	funcInfo curFunc;
	vector<pair<bool, int>> optionalReg;		//int为正为中间变量，int为负为参数
	ofstream out;
};

#endif // ! X86ASM_H
