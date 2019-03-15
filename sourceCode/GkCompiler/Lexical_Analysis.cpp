#include "Lexical_Analysis.h"
#include "GkCompiler_Global.h"

//not used
string sReservedWord[] = {
	"if","else","while","break","string","bool","char","int","char_array","int_array","zhp","true","false","main","return"
};

string sSysFunc[] = {
	"print","messagebox"
};
int iReservedWordSize = sizeof(sReservedWord) / sizeof(sReservedWord[0]);
int Line = 1;

/*
Purpose:
	Open source file and create output file

Parameters:
	None

Return value:
	bool	- true	:success
			- false	:failed
*/
bool CLexicalAnalyzer::Init_LexAnalyzer(string inFile)
{
	//cout << "Please input source file name:" << endl;
	sInFile = inFile;
	//sOutFile = outFile;
	ifstream hOpenFile;
	try
	{
		hOpenFile.open(sInFile);
		if (!hOpenFile)
		{
			throw "Open file failed,Please input again";
		}
		hOpenFile.close();
	}
	catch (const char* msg)
	{
		cout << msg << endl;
		while (!hOpenFile)
		{
			cin >> sInFile;
			hOpenFile.open(sInFile);
		}
		hOpenFile.close();
	}
	return true;
}

/*
Purpose:
	judge whether word is int type

Parameters:
	word	- the string to be judged

Return value:
	bool	- true	:number
			- false	:not number
*/
bool CLexicalAnalyzer::IsNum(string word)
{
	stringstream sin(word);
	double d;
	char c;
	if (!(sin >> d))
	{
		return false;
	}
	if (sin >> c)
	{
		return false;
	}
	return true;
}

bool CLexicalAnalyzer::OpenFile()
{
	hInputFile.open(sInFile);
	if (!hInputFile)
	{
		cout << "Open source file failed!" << endl;
		return false;
	}
	return true;
}

/*
Purpose:
	Divide source file into different kinds of parts

Parameters:
	None

Return value:
	None
*/
void CLexicalAnalyzer::LexicalAnalyzer()
{
	string sLine;
	if (!OpenFile())
	{
		ErrorMsgPrint("can't find or open file %s", sInFile);
		return;
	}
	//get single line from source file
	while (getline(hInputFile, sLine))
	{
		string sWord;
		for (unsigned int i = 0; i < sLine.length();)
		{
			sWord = "";
			//number
			if (sLine[i] >= '0' && sLine[i] <= '9')
			{
				sWord += sLine[i++];
				while (sLine[i] >= '0'&&sLine[i] <= '9')
				{
					sWord += sLine[i++];
				}
				Insert_WordTable(TK_INT, sWord);
			}
			//identifier
			else if ((sLine[i] >= 'a'&&sLine[i] <= 'z') || (sLine[i] >= 'A'&&sLine[i] <= 'Z') || (sLine[i] == '_'))		
			{
				bool flag = false;
				sWord += sLine[i++];
				while ((sLine[i] >= 'a'&&sLine[i] <= 'z') || (sLine[i] >= 'A'&&sLine[i] <= 'Z') || (sLine[i] >= '0'&&sLine[i] <= '9') || sLine[i] == '_')
				{
					sWord += sLine[i++];
				}
				//recognize KW types
				int keyWordIndex = JudgeKeyWord(sWord);
				if (keyWordIndex != -1)
				{
					Insert_WordTable((WordType)keyWordIndex, sWord);
				}
				else
				{
					Insert_WordTable(TK_ID, sWord);
				}
			}
			//symbol
			else if(sLine[i]==',')
			{
				sWord += sLine[i++];
				Insert_WordTable(TK_COMMA, sWord);
			}
			//operator
			else if(sLine[i]=='+'||sLine[i]=='-'||sLine[i]=='*'||sLine[i]=='/'||sLine[i]=='&'||sLine[i]=='|'||sLine[i]=='^'||sLine[i]=='~')
			{
				sWord += sLine[i++];
				if (sWord[0] == '+' && sLine[i] == '+')
				{
					sWord += sLine[i++];
				}
				else if (sWord[0] == '-' && sLine[i] == '-')
				{
					sWord += sLine[i++];
				}
				if (sWord[0] == '|' && sLine[i] == '|')
				{
					sWord += sLine[i++];
				}
				else if (sWord[0] == '&' && sLine[i] == '&')
				{
					sWord += sLine[i++];
				}
				Insert_WordTable(TK_OPERATOR, sWord);
			}
			//comparison operator and assign operator
			else if (sLine[i] == '=' || sLine[i] == '<' || sLine[i] == '>' || sLine[i]=='!')
			{
				sWord += sLine[i++];
				if (sWord[0] == '='&&sLine[i] != '=')
				{
					Insert_WordTable(TK_ASSIGN, sWord);
				}
				else
				{
					if (sLine[i] == '=')
					{
						sWord += sLine[i++];
					}
					Insert_WordTable(TK_COMPARE, sWord);
				}
			}
			//brcket
			else if (sLine[i] == '{' || sLine[i] == '}' || sLine[i] == '(' || sLine[i] == ')' || sLine[i] == '[' || sLine[i] == ']')
			{
				sWord += sLine[i++];
				switch (sWord[0])
				{
				case '(':
					Insert_WordTable(TK_OPENPA, sWord);
					break;
				case ')':
					Insert_WordTable(TK_CLOSEPA, sWord);
					break;
				case '[':
					Insert_WordTable(TK_OPENBR, sWord);
					break;
				case ']':
					Insert_WordTable(TK_CLOSEBR, sWord);
					break;
				case '{':
					Insert_WordTable(TK_BEGIN, sWord);
					break;
				case '}':
					Insert_WordTable(TK_END, sWord);
					break;
				default:
					break;
				}
			}
			//char
			else if (sLine[i] == '\'' && (sLine[i + 2] == '\''))
			{
				int iChar = sLine[i + 1];
				sWord += int2String(iChar);
				Insert_WordTable(TK_CHAR, sWord);
				i = i + 3;
			}
			else if (sLine[i] == ';')
			{
				sWord += sLine[i++];
				Insert_WordTable(TK_SEMICOLON, sWord);
			}
			//string
			else if (sLine[i] == '\"')
			{
				i++;
				while (sLine[i] != '\n')
				{
					sWord += sLine[i++];
					if (sLine[i] == '\"')
					{
						i++;
						break;
					}
				}
				Insert_WordTable(TK_STR, sWord);
			}
			//comments
			else if (sLine[i] == '#')
			{
				i++;
				while (i < sLine.length()) {
					i++;
				}
			}
			//space
			else if (sLine[i] == '\t' || sLine[i] == '\n' || sLine[i] == ' ')
			{
				i++;
			}
			else
			{
				ErrorMsgPrint("An unrecognized character at line %s", int2String(i));
				return;
			}
		}
		Insert_WordTable(TK_NEWLINE, "\n");
	}
	//---------------------------------------------------------
	// test code
	/*ofstream tempOut;
	tempOut.open("tempOut.txt", ios::out | ios::app);
	for (int i = 0; i < vWord.size(); i++)
	{
		tempOut << i;
		tempOut << " " << vWord[i].type << " " << vWord[i].word << endl;
	}
	tempOut.close();*/
	//---------------------------------------------------------
}

/*
Purpose:
	insert identified word into word table

Parameters:
	type	- word type
	word	- current word

Return value:
	None
*/
void CLexicalAnalyzer::Insert_WordTable(WordType type, string word)
{
	WORD_TYPE currentWord;
	currentWord.type = type;
	currentWord.word = word;
	currentWord.line = Line;
	if (currentWord.type == TK_NEWLINE)
	{
		Line++;
	}
	vWord.push_back(currentWord);
}

/*
功能：
	识别保留字

参数：
	要识别的string

返回值：
	string对应的WordType值
*/
int CLexicalAnalyzer::JudgeKeyWord(string word)
{
	if (word == "if")
		return KW_IF;
	else if (word == "else")
		return KW_ELSE;
	else if (word == "while")
		return KW_WHILE;
	else if (word == "break")
		return KW_BREAK;
	else if (word == "string")
		return KW_STR;
	else if (word == "zhp")
		return KW_SUBFUNC;
	else if (word == "bool")
		return KW_BOOL;
	else if (word == "char")
		return KW_CHAR;
	else if (word == "int")
		return KW_INT;
	else if (word == "int_array")
		return KW_INT_ARRAY;
	else if (word == "char_array")
		return KW_CHAR_ARRAY;
	else if (word == "for")
		return KW_FOR;
	else if (word == "true" || word == "false")
		return TK_BOOL;
	else if (word == "main")
		return KW_MAIN;
	else if (word == "return")
		return KW_RETURN;
	else if (word == "print")
		return KW_SYSFUNC;
	else if (word == "messagebox")
		return KW_SYSFUNC;
	else
	{
		return -1;
	}
}