#include "GodeGenerate.h"

void CBaseOpGen::CodeGen()
{
	string Operator = GetOperator();
	outFile << Operator << "," << arg1 << "," << arg2 << "," << result << endl;
}

void CBaseOpGen::GetParam(string op1, string op2, OP_TYPE Operator, string Result)
{
	arg1 = op1;
	arg2 = op2;
	op = Operator;
	result = Result;
}

string CBaseOpGen::GetOperator()
{
	string Operator;
	switch (op)
	{
	case ADD:
		Operator = "ADD";
		break;
	case SUB:
		Operator = "SUB";
		break;
	case MUL:
		Operator = "IMUL";
		break;
	case DIV:
		Operator = "IDIV";
		break;
	case XOR:
		Operator = "XOR";
		break;
	case AND:
		Operator = "AND";
		break;
	case OR:
		Operator = "OR";
		break;
	case ASSIGN:
		Operator = "ASSIGN";
		break;
	case UMINUS:
		Operator = "UMINUS";
		break;
	case JE:
		Operator = "JE";
		break;
	case JNE:
		Operator = "JNE";
		break;
	case JLE:
		Operator = "JLE";
		break;
	case JL:
		Operator = "JL";
		break;
	case JGE:
		Operator = "JGE";
		break;
	case JG:
		Operator = "JG";
		break;
	case JMP:
		Operator = "JMP";
		break;
	case LABEL:
		Operator = "LABEL";
		break;
	case PUSH:
		Operator = "PUSH";
		break;
	case POP:
		Operator = "POP";
		break;
	case CALL:
		Operator = "CALL";
		break;
	case INDEX:
		Operator = "INDEX";
		break;
	case NOT:
		Operator = "NOT";
		break;
	case RETURN:
		Operator = "RETURN";
		break;
	default:
		break;
	}
	return Operator;
}

int CBaseOpGen::JudgeOperator(string Operator)
{
	if (Operator == ">=")
		return JGE;
	else if (Operator == ">")
		return JG;
	else if (Operator == "<=")
		return JLE;
	else if (Operator == "<")
		return JL;
	else if (Operator == "==")
		return JE;
	else if (Operator == "!=")
		return JNE;
	else
	{
		return -1;
	}
}

int CBaseOpGen::GetOppositeOp(string Operator)
{
	if (Operator == ">=")
		return JL;
	else if (Operator == ">")
		return JLE;
	else if (Operator == "<=")
		return JG;
	else if (Operator == "<")
		return JGE;
	else if (Operator == "==")
		return JNE;
	else if (Operator == "!=")
		return JE;
	else
	{
		return -1;
	}
}

CBaseOpGen::CBaseOpGen()
{
}


void CFuncOpGen::StartCodeGen()
{
	outFile << "PROC " << funcName <<" ";
	for (int i=0;i<paramList.size();i++)
	{
		outFile << paramList[i];
		if ((i + 1) != paramList.size())
		{
			outFile << ",";
		}
	}
	outFile << endl;
}

void CFuncOpGen::EndCodeGen()
{
	outFile << "ENDP " << funcName << endl;
}

void CFuncOpGen::MainStartCodeGen()
{
	outFile << "START:" << endl;
}

void CFuncOpGen::MainEndCodeGen()
{
	outFile << "END START" << endl;
}

void CFuncOpGen::GetFunName(string name)
{
	funcName = name;
}

void CFuncOpGen::GetParameter(string param)
{
	paramList.push_back(param);
}

void CFuncOpGen::GetOffset(int espOffset)
{
	offset = espOffset;
}


void FinalCodeGenerate::ReadMidFileAndWrite()
{
	string line;
	inFile.open("medicateCode.txt", ios::in);
	ErrorMsgPrint("can't find or read file %s", "medicateCode.txt");
	while (getline(inFile, line))
	{
		outFile << line << endl;
	}
}

/*void FinalCodeGenerate::WriteFinalFile(string name="finalcode.asm")
{
	vector<string> asmBegin = {".486",
		".model flat, stdcall",
		"option casemap:none",
		"includelib \\masm32\\lib\\kernel32.lib",
		"includelib \\masm32\\lib\\user32.lib",
		"includelib \\masm32\\lib\\gdi32.lib",
		"includelib \\masm32\\lib\\msvcrt.lib",
		"includelib \\masm32\\lib\\masm32.lib",
		"\n",
		"include \\masm32\\include\\kernel32.inc",
		"include \\masm32\\include\\user32.inc",
		"include \\masm32\\include\\gdi32.inc",
		"include \\masm32\\include\\windows.inc",
		"include \\masm32\\include\\msvcrt.inc",
		"include \\masm32\\include\\masm32.inc",
		"\n"
	};
	outFile.open(name, ios::out);
	for (const auto i : asmBegin)
	{
		outFile << i << endl;
	}
	outFile << ".data" << endl;
	outFile << ".stack" << endl;
	outFile << ".code" << endl;
	outFile << "gkfunc proc" << endl;
	outFile << "push ebp" << endl;
	outFile << "mov ebp,esp" << endl;
	outFile << "sub esp," << espOffset << endl;
	ReadMidFileAndWrite();
	outFile << "add esp," << espOffset << endl;
	outFile << "mov esp,ebp" << endl;
	outFile << "pop ebp" << endl;
	outFile << "ret" << endl;
	outFile << "gkfunc endp" << endl;
	outFile << "start:" << endl;
	outFile << "call gkfunc" << endl;
	outFile << "ret" << endl;
	outFile << "end start" << endl;
}
*/
void CGlobalVarGen::CodeGen()
{
	globalFile.open("GLOBALVAR.txt", ios::out|ios::app);
	if (varType == KW_BOOL || varType == KW_STR || varType == KW_INT || varType == KW_CHAR)
	{
		if (varType != KW_STR)
		{
			globalFile << varName << " dd " << varValue.front() << endl;
		}
		else
		{
			globalFile << varName << " db \'" << varValue.front() << "\',0" << endl;
		}
	}
	// KW_INT_ARRAY , KW_CHAR_ARRAY
	else
	{
		if (varType == KW_INT_ARRAY)
		{
			globalFile << varName << " dd ";
			for (const auto i : varValue)
			{
				globalFile << i << ",";
			}
			globalFile << "0" << endl;
		}
		else
		{
			globalFile << varName << " db ";
			for (const auto i : varValue)
			{
				globalFile << i << ",";
			}
			globalFile << "0" << endl;
		}
	}
	globalFile.close();
	varValue.clear();
}

void CGlobalVarGen::GetParam(WordType type, string name, string value)
{
	varType = type;
	varName = name;
	if (varType == KW_BOOL)
	{
		if (value == "true")
		{
			value = "1";
		}
		else
		{
			value = "0";
		}
	}
	if (value == "")
	{
		varValue.push_back("?");
	}
	else
	{
		varValue.push_back(value);
	}
}
