#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "stdafx.h"
#include <regex>
#define IN
#define OUT
using namespace std;

enum WordType
{
	TK_INT,				// number
	TK_CHAR,				// ex:'a'
	TK_STR,					// ex:"aaa"
	TK_OPERATOR,			// +, -, *, /, & , |, ^, ~, ++, --
	TK_COMPARE,				// >=, <=,> ,< , ==, !=
	TK_ID,					// token
	TK_ASSIGN,				// =
	TK_OPENPA,				// (
	TK_CLOSEPA,				// )
	TK_OPENBR,				// [
	TK_CLOSEBR,				// ]
	TK_BEGIN,				// {
	TK_END,					// }
	TK_COMMA,				// ,
	TK_SEMICOLON,			// ;
	TK_NEWLINE,				// \n
	TK_BOOL,				// bool
	TK_FUNCNAME,

	KW_FOR,
	KW_IF,					// if
	KW_ELSE,				// else
	KW_WHILE,				// while
	KW_BREAK,				// break
	KW_STR,					// string
	KW_BOOL,				// bool
	KW_CHAR,				// char
	KW_INT,					// int
	KW_CHAR_ARRAY,			// char_array
	KW_INT_ARRAY,			// int_array
	KW_SUBFUNC,				// subfunc
	KW_TEMPVAR,				// temp variable
	KW_SYSFUNC,				// system function
	KW_MAIN,					// main func
	KW_RETURN
};
struct FUNC_TYPE
{
	string name;
	WordType type;
	vector<pair<WordType, string>> param;
	int paramNum = 0;
};

//char int bool string
struct BASE_TYPE
{
	string name;
	string value;
	string memOffset;
	WordType type;
	bool isInit;
	int offset;
};

//single symbol table 
struct WORD_TYPE
{
	WordType type;
	string word;
	int line;
};

struct SYMBOL_TABLE
{
	WordType type;
	string name;
	int offset;
	string memOffset;
};

//array type
struct ARRAY_TYPE
{
	string name;
	vector<string> arr;
	WordType type;
	string memOffset;
	int offset;
	int len;
};

extern vector<WORD_TYPE> vWord;
class CBaseFunction
{
public:
	CBaseFunction();
	~CBaseFunction();
	int String2int(string num);
	string int2String(int i);
	bool IsDigit(string var);
	void ErrorMsgPrint(string msg, string word = "");			//´ý¶¨
	ofstream outFile;
	ofstream outTempIndex;
};

class CSymbol_Table:public CBaseFunction
{
public:
	void InsertBaseVar(BASE_TYPE *var);
	void InsertArrVar(ARRAY_TYPE *var);
	void InsertFuncInfo(FUNC_TYPE *func);
	void InsertSymbol(string name, int type, int offset);
	bool IsFunc(string name);
	FUNC_TYPE GetFuncInfo(string name);
	vector<BASE_TYPE> baselist;
	vector<ARRAY_TYPE> arrlist;
	vector<FUNC_TYPE> funclist;
	vector<SYMBOL_TABLE> symbollist;
	int espOffset = 12;			// int -4,char -1,The 'string' is determined by its size
	int paramOffset = -8;
};

#endif // !_GLOBAL_H