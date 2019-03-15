#include "GkCompiler_Global.h"

vector<WORD_TYPE> vWord;

CBaseFunction::CBaseFunction()
{
	outFile.open("MIDCODE.txt", ios::out | ios::app);
	outTempIndex.open("TEMPINDEX.txt", ios::out | ios::app);
}

CBaseFunction::~CBaseFunction()
{
	outFile.close();
}

int CBaseFunction::String2int(string num)
{
	return atoi(num.c_str());;
}

string CBaseFunction::int2String(int i)
{
	stringstream ss;
	string num;
	ss << i;
	ss >> num;
	return num;
}

bool CBaseFunction::IsDigit(string var)
{
	for (unsigned int i = 0; i < var.size(); i++)
	{
		if (!isdigit(var[i]))
		{
			return false;
		}
	}
	return true;
}

void CBaseFunction::ErrorMsgPrint(string msg, string word)
{
	char errorMsg[100];
	sprintf_s(errorMsg, msg.c_str(), word.c_str());
	cout << errorMsg << endl;
}



/*
Purpose:
add base type variable

Parameters:
BASE_TYPE*	- variable information

Return value:
None
*/
void CSymbol_Table::InsertBaseVar(BASE_TYPE * var)
{
	baselist.push_back(*var);
	if (var->type == KW_CHAR)
	{
		var->offset = espOffset;
		espOffset += 4;
	}
	else if (var->type == KW_INT)
	{
		var->offset = espOffset;
		espOffset += 4;
	}
	else if (var->type == KW_BOOL)
	{
		var->offset = espOffset;
		espOffset += 4;
	}
	else if (var->type == KW_STR)
	{
		var->offset = espOffset;
		int temp = var->value.size() % 4;
		if (temp != 0)
		{
			espOffset += (var->value.size() - temp) + 4;
		}
		else
		{
			espOffset += var->value.size();
		}
	}
	var->memOffset = "[-" + int2String(var->offset) + "]";
}

/*
Purpose:
add array type variable

Parameters:
ARRAY_TYPE*	- variable information

Return value:
None
*/
void CSymbol_Table::InsertArrVar(ARRAY_TYPE * var)
{
	arrlist.push_back(*var);
	var->offset = espOffset;
	espOffset += var->len * 4;
	/*if (var->type == KW_INT_ARRAY)
	{
		var->offset = espOffset;
		espOffset += var->len * 4;
	}
	else
	{
		var->offset = espOffset;
		int temp;
		temp = var->len % 4;
		if (temp != 0)
		{
			espOffset += (var->len - temp) + 4;
		}
		else
		{
			espOffset += var->len;
		}
	}*/
	var->memOffset = "[-" + int2String(var->offset) + "]";
}

void CSymbol_Table::InsertFuncInfo(FUNC_TYPE * func)
{
	funclist.push_back(*func);
}

/*
Purpose:
add a variable to symbol table

Parameters:
SYMBOL_TABLE*	- variable information

Return value:
None
*/
void CSymbol_Table::InsertSymbol(string name, int type, int offset)
{
	SYMBOL_TABLE varInfo;
	varInfo.name = name;
	varInfo.type = (WordType)type;
	if (varInfo.type != TK_FUNCNAME)
	{
		varInfo.offset = offset;
		if (offset > 0)
		{
			varInfo.memOffset = "[-" + int2String(offset) + "]";
		}
		else
		{
			varInfo.memOffset = "[+" + int2String(-offset) + "]";
		}
	}
	else
	{
		varInfo.offset = 0;
		varInfo.memOffset = "0";
	}
	symbollist.push_back(varInfo);
}

bool CSymbol_Table::IsFunc(string name)
{
	for (const auto i : funclist)
	{
		if (i.name == name)
		{
			return true;
		}
	}
	return false;
}

FUNC_TYPE CSymbol_Table::GetFuncInfo(string name)
{
	for (const auto i : funclist)
	{
		if (i.name == name)
		{
			return i;
		}
	}
}
