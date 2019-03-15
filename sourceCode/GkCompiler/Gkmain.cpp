#include "stdafx.h"
#include "Lexical_Analysis.h"
#include "Syntax_Analyzer.h"
int main(int argc,char* argv[])
{
	CLexicalAnalyzer LexAnalyzer;
	if (argc != 2)
	{
		cout << "usage:<input file name>";
		return 0;
	}
	string inFile = argv[1];
	//string outFile = argv[2];
	if (!LexAnalyzer.Init_LexAnalyzer(inFile))
	{
		return false;
	}
	LexAnalyzer.LexicalAnalyzer();
	cout << "start lexical analyse" << endl;
	CSyntaxAnalyzer SyntaxAnalyzer;
	cout << "lexical analyse completed!" << endl;
	cout << endl;
	cout << "start syntax analyse" << endl;
	SyntaxAnalyzer.Syntax_Analyzer();
	cout << "syntax analyse completed!" << endl;
	WinExec("FinalCodeGenerate.exe", SW_NORMAL);
	return 0;
}