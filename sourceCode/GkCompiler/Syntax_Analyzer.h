#ifndef _SYNTAX_H
#define _SYNTAX_H
#include "GkCompiler_Global.h"
#include "GodeGenerate.h"
class CFunc_Analyse;
class CExp_Analyse:public CBaseFunction
{
public:
	CExp_Analyse(CSymbol_Table *symbol) :symbolTable(symbol){}
	void GetGlobalSymbol(CSymbol_Table * gSymbol);
	void Var_Analysis(WordType type,int parentNum);		// 1 subfunction 2 global
	void VarDefin_Analysis(WordType type);
	void VarAssign_Analysis();
	void DetectSpecifiedVt(WordType vt);
	int ReturnTempIndex();
	string Func_Call(FUNC_TYPE *funcInfo);
	string GetTempVar();						//get new temp variable
	string Exp_Analysis();						//addition expression
	string Multi_Analysis();					//multiply expression
	string BitOp_Analysis();					//bit operate expression
	string Logical_Analysis();
	string Get_Terminator();
	SYMBOL_TABLE GetVarInfo(string var);
	bool IsVar(string word);
	string GetArrElementIndex(int offset,int i, WordType arrType);
private:
	CSymbol_Table * symbolTable;
	CSymbol_Table * globlSymbol = NULL;
	int tempIndex = 0;
	int parent = 0;
};

class CSyntaxAnalyzer
{
public:
	CExp_Analyse * varAnalyse = new CExp_Analyse(&globalSymbol);
	bool Init_SyntaxAnalyer();
	void GlobalVarAnalyse();
	void Syntax_Analyzer();
	void InsertSystemFuncInfo();
private:
	CSymbol_Table globalSymbol;
};

class CFunc_Analyse:public CBaseFunction
{
public:
	CFunc_Analyse(CSymbol_Table *symbol) :globalSymbol(symbol) {}
	~CFunc_Analyse();
	CExp_Analyse * varAnalyse = new CExp_Analyse(&funcSymbol);
	string GetLabel();
	void GetFuncName(string name);
	void Subfunc_Analysis();
	void Mainfunc_Analysis();
	void GetParameters(FUNC_TYPE * func);
	void CompoundStat_Analysis(string cycleBreakLabel="");
	void If_Analysis();
	void While_Analysis();
	void For_Analysis();
	void Break_Tag(string breakTag);
	void IgnoreLineBreak();
	bool IsSystemFunc(string curWord);
private:
	int labelIndex = 0;
	vector<pair<WordType, string>> paramList;
	string funcName;
	CSymbol_Table *globalSymbol;
	CSymbol_Table funcSymbol;
};

inline bool CSyntaxAnalyzer::Init_SyntaxAnalyer()
{
	if (vWord.empty())
	{
		return false;
	}
	return true;
}

#endif // !_SYNTAX_H

