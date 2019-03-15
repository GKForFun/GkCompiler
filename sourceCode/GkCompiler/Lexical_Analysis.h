#ifndef _LEX_H
#define _LEX_H
#include "GkCompiler_Global.h"
static string sysFuncName[] = {
	"print","input","messageBox"
};
struct ReservWord
{
	WordType type;
	string name;
};
static ReservWord revWordList[] = {
	{KW_IF,"if"},
	{KW_ELSE,"else"},
	{KW_WHILE,"while"},
	{KW_BREAK,"break"},
	{KW_STR,"string"},
	{KW_SUBFUNC,"zhp"},
	{KW_SUBFUNC,"main"},
	{KW_BOOL,"bool"},
	{KW_CHAR,"char"},
	{KW_INT,"int"},
	{KW_INT_ARRAY,"int_array"},
	{KW_CHAR_ARRAY,"char_array"},
	{KW_FOR,"for"},
	{KW_RETURN,"return"},
};

class CLexicalAnalyzer:public CBaseFunction
{
public:
	bool Init_LexAnalyzer(string inFile);
	void LexicalAnalyzer();
	bool IsNum(string word);
	bool OpenFile();
	void Insert_WordTable(WordType type,string name);
	int JudgeKeyWord(string word);
private:
	ifstream hInputFile;
	string sInFile;
	//string sOutFile;
};
#endif // !_LEX_H