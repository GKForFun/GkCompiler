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
	__out int GetFuncNum();			//���غ�������������������
	void GetTmpVarLifeRange(FuncInfo* func);		//��ʼ�������е��м��������Ϣ
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
	void FuncHandle();		//��������
	void InitRegister();	//��ʼ���Ĵ���
	void BaseCalcuOp();		//�������
	void AssignOp();
	void CmpAndJmp();
	void LabelPrint();
	void StackOp();
	void FuncOp();
	void ArrIndex();
	int GetIdleReg();		//��ȡ���мĴ���
	int GetArrBase(string base);
	string GetTmpVar();		//��ʱ����
	string GetRealArg(string arg);		//��ȡ��ʵ����
	bool JudgeIndexVar(string arg);
	bool JudgeRegVar(string arg);		//�ж��Ƿ�Ϊ�Ĵ���
	int GetRegIndex(string regName);	//��ȡ�Ĵ�������
	bool JudgeMemVar(string arg);		//�ж��Ƿ�Ϊ�ڴ����
	string GetRealMemVarValue(string arg);	//��ȡ�ڴ������ʵֵ
	bool JudgeTmpVar(string arg);		//�ж��Ƿ�Ϊ�м����
	string GetRealTmpVarValue(string arg);	//��ȡ�м������ʵֵ
	int GetTmpVarIndex(string arg);
	bool JudgeGlobalVar(string arg);		//�ж��Ƿ�Ϊȫ�ֱ���
	string GetRealGlobalVarValue(string arg);	//��ȡȫ�ֱ�����ʵֵ
	void FreeRegister(int index);
	void JudgeArgType();				//�жϲ�������
	int JudgeArgLifeEnd();				//
	void BusyRegister(int index);
	void SetTmpVarValue(int index, string value);

	//fElement lastElement;
	fElement curElement;
	fElement nextElement;
	string realArg[2];			//ע���������β����ҪӰ����һ����Ԫʽ
	ArgType curArg[2];
	string realResult;
	vector<string> curFuncInsList;
	int arrIndexReg = 0;
	int regIndex[2] = { -1,-1 };
	int argLifeEnd[2] = { -1,-1 };
	int iCurElement;
private:
	funcInfo curFunc;
	vector<pair<bool, int>> optionalReg;		//intΪ��Ϊ�м������intΪ��Ϊ����
	ofstream out;
};

#endif // ! X86ASM_H
