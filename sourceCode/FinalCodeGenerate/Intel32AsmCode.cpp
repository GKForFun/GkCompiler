#include "Intel32AsmCode.h"
#include "stdafx.h"
//从文件中读取四元式
//PROC,ENDP,START,END不作为四元式存储
string regName[] = { "eax","ebx","ecx","edx" };
void CBaseInfo::ReadElementFromFile()
{
	inFile.open("MIDCODE.txt", ios::in);
	string line;
	while (getline(inFile, line))
	{
		FourElement curElement;
		vector<string> elementVector;
		string tmpElement;
		unsigned int len = 0, i = 0;
		while (i < line.size())
		{
			if (line[i + len] != ','&& line[i + len] != ' '&& line[i + len] != '\0')
			{
				len++;
			}
			else
			{
				tmpElement = line.substr(i, len);
				elementVector.push_back(tmpElement);
				len++;
				i += len;
				len = 0;
			}
		}
		while (elementVector.size() < 4)
		{
			elementVector.push_back("");
		}
		if (elementVector[0] != "PROC" && elementVector[0] != "ENDP" && elementVector[0]!="START:" && elementVector[0] != "END")
		{
			curElement.op = elementVector[0];
			curElement.arg1 = elementVector[1];
			curElement.arg2 = elementVector[2];
			curElement.result = elementVector[3];
			element.push_back(curElement);
		}
		else if (elementVector[0] == "PROC")
		{
			FuncInfo newFunc;
			newFunc.name = elementVector[1];
			newFunc.start = element.size();
			for (unsigned int i = 2; i < elementVector.size(); i++)
			{
				if (elementVector[i] != "")
				{
					newFunc.param.push_back(elementVector[i]);
				}
			}
			function.push_back(newFunc);
			funcNum++;
		}
		else if (elementVector[0] == "ENDP" || elementVector[0] == "END")
		{
			FuncInfo curFunc = function.back();
			function.pop_back();
			curFunc.end = element.size()-1;
			function.push_back(curFunc);
		}
		else if (elementVector[0] == "START:")
		{
			FuncInfo newFunc;
			newFunc.name = "main";
			newFunc.start = element.size();
			function.push_back(newFunc);
			funcNum++;
		}
		else
		{
			cout << "read four element error!" <<endl;
			break;
		}
		elementVector.clear();
	}
	inFile.close();
}
//从文件中获取每个函数的中间变量数量以及esp偏移
//初始化中间变量生命周期
void CBaseInfo::GetTmpVarInfo()
{
	ifstream inFile;
	inFile.open("TEMPINDEX.txt", ios::in);
	string line;
	list<FuncInfo>::iterator iter = function.begin();
	while (getline(inFile, line))
	{
		vector<string> tmp;
		string tmp2;
		unsigned int len = 0, i = 0;
		while (i < line.size())
		{
			if (line[i + len] != ','&& line[i + len] != ' '&& line[i + len] != '\0')
			{
				len++;
			}
			else
			{
				tmp2 = line.substr(i, len);
				tmp.push_back(tmp2);
				len++;
				i += len;
				len = 0;
			}
		}
		iter->tmpNum = atoi(tmp[0].c_str());
		GetTmpVarLifeRange(&(*iter));
		iter->espOffset = atoi(tmp[1].c_str());
		iter++;
	}
}
//根据索引返回四元式
fElement CBaseInfo::GetElement(int index)
{
	return element[index];
}
//根据索引返回函数信息
funcInfo CBaseInfo::GetFuncInfo(int index)
{
	list<FuncInfo>::iterator iter;
	int i;
	for (iter = function.begin(),i = 0; iter != function.end(); iter++, i++)
	{
		if (i == index)
		{
			return *iter;
		}
	}
}
//返回函数数量，包括主函数
int CBaseInfo::GetFuncNum()
{
	return funcNum;
}
//初始化函数中间变量
void CBaseInfo::GetTmpVarLifeRange(FuncInfo* func)
{
	for (int i = 0; i < func->tmpNum; i++)
	{
		TmpVar curVar;
		bool flag = true;
		string tmpVar = "T"+Int2string(i);
		for (unsigned int j = func->start; j <= func->end; j++)
		{
			if (element[j].result == tmpVar && flag)
			{
				curVar.range.first = j;
				flag = false;
				continue;
			}
			if (element[j].arg1 == tmpVar || element[j].arg2 == tmpVar ||element[j].result == tmpVar)
			{
				curVar.range.second = j;
				continue;
			}
		}
		curVar.value = "";
		func->tmpVarList.push_back(curVar);
	}
}
//int转string
string CBaseInfo::Int2string(int num)
{
	stringstream ss;
	string tmp;
	ss << num;
	ss >> tmp;
	return tmp;
}
//注意
//	寄存器分配
//	如果当前操作操作数不为寄存器的话，取一个空闲寄存器做中转，如果没有空闲的，则选择占用寄存器的生命周期长的变量剥夺
//	
void CCodeGen::FuncHandle()
{
	out.open("final.asm", ios::out);
	out << ".386" << endl;
	out << ".model flat, stdcall" << endl;
	out << "include include\\msvcrt.inc" << endl;
	out << "includelib lib\\msvcrt.lib" << endl;
	out << ".data" << endl;
	out << "       szFormat  db    '%c',0" << endl;
	out << ".code" << endl;
	int funcNum = GetFuncNum();
	InitRegister();		//初始化寄存器组
	for (int i = 0; i < funcNum; i++)
	{
		curFunc = GetFuncInfo(i);
		if (curFunc.name != "main")
		{
			out <<  curFunc.name + " proc C ";
			for (unsigned int k = 0; k < curFunc.param.size(); k++)
			{
				if (k != curFunc.param.size() - 1)
				{
					out<< ",";
				}
				out << curFunc.param[k];
			}
			out<< endl;
		}
		else
		{
			out << "start:" << endl;
		}
		for (int j = curFunc.start; j <= curFunc.end; j++)
		{
			curElement = GetElement(j);
			if (j != curFunc.end)
			{
				nextElement = GetElement(j + 1);
			}
			else
			{
				nextElement = GetElement(j);
			}
			iCurElement = j;
			realArg[0] = GetRealArg(curElement.arg1);
			realArg[1] = GetRealArg(curElement.arg2);
			string op = curElement.op;
			//分类：
			//	运算操作 add,sub,mul,div,xor,and,or,not
			//	赋值操作 assign
			//	比较操作 je,jne,jle,jl,jge,jg,jmp
			//	标签操作 label
			//	栈操作	 push,pop
			//	函数操作 call,return
			//	数组操作 index
			if (op == "ADD" || op == "SUB" || op == "IMUL" || op == "IDIV" || op == "XOR" || op == "AND" || op == "OR" || op == "NOT")
			{
				BaseCalcuOp();
			}
			else if (op == "ASSIGN")
			{
				AssignOp();
			}
			else if (op == "JE" || op == "JNE" || op == "JLE" || op == "JL" || op == "JGE" || op == "JG" || op == "JMP")
			{
				CmpAndJmp();
			}
			else if (op == "LABEL")
			{
				LabelPrint();
			}
			else if (op == "PUSH" || op == "POP")
			{
				StackOp();
			}
			else if (op == "CALL" || op == "RETURN")
			{
				FuncOp();
			}
			else if (op == "INDEX")
			{
				ArrIndex();
			}
			if (arrIndexReg != 0)
			{
				if (JudgeMemVar(curElement.arg1) || JudgeTmpVar(curElement.arg1))
				{
					if (JudgeMemVar(curElement.arg1))
					{
						string reg1 = (curElement.arg1).substr(1, (curElement.arg1).size() - 2);
						if (JudgeRegVar(reg1))
						{
							FreeRegister(GetRegIndex(reg1));
						}
					}
					else
					{
						string arg1 = GetRealTmpVarValue(curElement.arg1);
						if (arg1[0] == 'i' && arg1[1] ==':')
						{
							if(realArg[0][0] == 'd')
							{
								string reg1 = realArg[0].substr(14, realArg[0].size() - 15);
								if (JudgeRegVar(reg1))
								{
									FreeRegister(GetRegIndex(reg1));
								}
							}
						}
						else if (JudgeMemVar(arg1))
						{
							string reg1 = arg1.substr(1, arg1.size() - 2);
							if (JudgeRegVar(reg1))
							{
								FreeRegister(GetRegIndex(reg1));
							}
						}
					}
				}
				if (JudgeMemVar(curElement.arg2)|| JudgeTmpVar(curElement.arg2))
				{
					if (JudgeMemVar(curElement.arg2))
					{
						string reg2 = (curElement.arg2).substr(1, (curElement.arg2).size() - 2);
						if (JudgeRegVar(reg2))
						{
							FreeRegister(GetRegIndex(reg2));
						}
					}
					else
					{
						string arg2 = GetRealTmpVarValue(curElement.arg2);
						if (arg2[0] == 'i' && arg2[1] == ':')
						{
							if (realArg[1][0] == 'd')
							{
								string reg1 = realArg[1].substr(14, realArg[1].size() - 15);
								if (JudgeRegVar(reg1))
								{
									FreeRegister(GetRegIndex(reg1));
								}
							}
						}
						else if (JudgeMemVar(arg2))
						{
							string reg2 = arg2.substr(1, arg2.size() - 2);
							if (JudgeRegVar(reg2))
							{
								FreeRegister(GetRegIndex(reg2));
							}
						}
					}
				}
				if (JudgeMemVar(curElement.result) || JudgeTmpVar(curElement.result))
				{
					if (JudgeMemVar(curElement.result))
					{
						string reg2 = (curElement.result).substr(1, (curElement.result).size() - 2);
						if (JudgeRegVar(reg2))
						{
							FreeRegister(GetRegIndex(reg2));
						}
					}
					else
					{
						string arg2 = GetRealTmpVarValue(curElement.result);
						if (arg2[0] == 'i' && arg2[1] == ':')
						{
							if (realResult[0] == 'd')
							{
								string reg1 = realResult.substr(14, realResult.size() - 15);
								if (JudgeRegVar(reg1))
								{
									FreeRegister(GetRegIndex(reg1));
								}
							}
						}
						else if (JudgeMemVar(arg2))
						{
							string reg2 = arg2.substr(1, arg2.size() - 2);
							if (JudgeRegVar(reg2))
							{
								FreeRegister(GetRegIndex(reg2));
							}
						}
					}
				}
				arrIndexReg = 0;
			}
		}
		out << "push ebp" << endl;
		out << "mov ebp,esp" << endl;
		out << "sub esp," << curFunc.espOffset << endl;
		for (unsigned int k = 0; k < curFuncInsList.size(); k++)
		{
			out << curFuncInsList[k] << endl;
		}
		if (curFunc.name != "main")
		{
			out << "add esp," << Int2string(curFunc.espOffset) <<endl;
			out << "mov esp,ebp" << endl;
			out << "pop ebp" << endl;
			out << "ret" << endl;
			out << curFunc.name + " endp" << endl;
		}
		else
		{
			out << "add esp," << curFunc.espOffset << endl;
			out << "mov esp,ebp" << endl;
			out << "pop ebp" << endl;
			out << "ret" << endl;
			out << "end start" << endl;
		}
		curFuncInsList.clear();
	}
}

void CCodeGen::BaseCalcuOp()
{
	if (curElement.op == "ADD")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "add eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			curFuncInsList.push_back( "add eax," + realArg[0] );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "add eax," + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "SUB")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "sub eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			string reg = regName[GetIdleReg()];
			curFuncInsList.push_back( "mov " + reg + "," + realArg[0] );
			curFuncInsList.push_back( "sub " + reg + ",eax" );
			curFuncInsList.push_back( "mov eax," + reg );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "sub eax," + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "IMUL")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "imul eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			curFuncInsList.push_back( "imul eax," + realArg[0] );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "imul eax," + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "IDIV")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "cdq" );
			curFuncInsList.push_back( "idiv eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			string reg = regName[GetIdleReg()];
			curFuncInsList.push_back( "mov " + reg + ",eax" );
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "cdq" );
			curFuncInsList.push_back( "idiv " + reg );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "cdq" );
			curFuncInsList.push_back( "idiv " + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "XOR")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "xor eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			curFuncInsList.push_back( "xor eax," + realArg[0] );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "xor eax," + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "OR")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "or eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			curFuncInsList.push_back( "or eax," + realArg[0] );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "or eax," + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "AND")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "and eax," + realArg[1] );
			FreeRegister(0);
		}
		else if (realArg[1] == "eax")
		{
			curFuncInsList.push_back( "and eax," + realArg[0] );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "and eax," + realArg[1] );
		}
		//---------------------------------------------------------------------------------
		if (arrIndexReg != -1)
		{
			if (arrIndexReg == 1)
			{
				string reg = (curElement.arg1).substr(1, (curElement.arg1).size() - 2);
				FreeRegister(GetRegIndex(reg));
			}
			else if (arrIndexReg == 2)
			{
				string reg = (curElement.arg2).substr(1, (curElement.arg2).size() - 2);
				FreeRegister(GetRegIndex(reg));
			}
			else if (arrIndexReg == 3)
			{
				string reg1 = (curElement.arg1).substr(1, (curElement.arg1).size() - 2);
				FreeRegister(GetRegIndex(reg1));
				string reg2 = (curElement.arg2).substr(1, (curElement.arg2).size() - 2);
				FreeRegister(GetRegIndex(reg2));
			}
		}
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
	else if (curElement.op == "NOT")
	{
		if (realArg[0] == "eax")
		{
			curFuncInsList.push_back( "not eax" );
			FreeRegister(0);
		}
		else
		{
			curFuncInsList.push_back( "mov eax," + realArg[0] );
			curFuncInsList.push_back( "not eax" );
		}
		//---------------------------------------------------------------------------------
		if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result)
		{
			string tmp = GetTmpVar();
			curFuncInsList.push_back( "mov " + tmp + ",eax" );
			SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
		}
		else
		{
			BusyRegister(0);
			SetTmpVarValue(GetTmpVarIndex(curElement.result), "eax");
		}
		//---------------------------------------------------------------------------------------
	}
}

void CCodeGen::AssignOp()
{
	realResult = GetRealArg(curElement.result);
	if (JudgeRegVar(realArg[0]))
	{
		curFuncInsList.push_back( "mov " + realResult + "," + realArg[0] );
		FreeRegister(GetRegIndex(realArg[0]));
	}
	else
	{
		string reg = regName[GetIdleReg()];
		curFuncInsList.push_back( "mov " + reg + "," + realArg[0] );
		curFuncInsList.push_back( "mov " + realResult + "," + reg );
	}
}

void CCodeGen::CmpAndJmp()
{
	if (curElement.op == "JMP")
	{
		curFuncInsList.push_back( "jmp " + curElement.result );
	}
	else
	{
		string arg1 = realArg[0];
		if (!JudgeRegVar(realArg[0]) && !JudgeRegVar(realArg[1]))
		{
			arg1 = regName[GetIdleReg()];
			curFuncInsList.push_back( "mov " + arg1 + "," + realArg[0] );
		}
		curFuncInsList.push_back( "cmp " + arg1 + "," + realArg[1] );
		for (int i = 0; i <curElement.op.size(); i++)
			curElement.op[i] = tolower(curElement.op[i]);
		curFuncInsList.push_back( curElement.op + " " + curElement.result );
		for (int i = 0; i < 2; i++)
		{
			if (JudgeRegVar(realArg[0]))
			{
				FreeRegister(GetRegIndex(realArg[0]));
			}
		}
	}
}

void CCodeGen::LabelPrint()
{
	curFuncInsList.push_back( curElement.arg1+ ":" );
}

void CCodeGen::StackOp()
{
	if (curElement.op == "PUSH")
	{
		curFuncInsList.push_back( "push " + realArg[0] );
	}
	else
	{
		if (!JudgeRegVar(realArg[0]))
		{
			string reg = regName[GetIdleReg()];
			curFuncInsList.push_back( "pop " + reg );
			curFuncInsList.push_back( "mov " + realArg[0] + "," + reg );
		}
		else
		{
			curFuncInsList.push_back( "pop " + realArg[0] );
		}
	}
}

void CCodeGen::FuncOp()
{
	if (curElement.op == "CALL")
	{
		if (curElement.arg1 == "print")
		{
			string reg = regName[GetIdleReg()];
			curFuncInsList.push_back("lea " + reg + ",szFormat");
			curFuncInsList.push_back("push " + reg);
			curFuncInsList.push_back("call crt_printf");
		}
		else
		{
			curFuncInsList.push_back("call " + curElement.arg1);
		}
	}
	else if (curElement.op == "RETURN")
	{
		if (realArg[0] != "")
		{
			if (realArg[0] != "eax")
			{
				curFuncInsList.push_back("mov eax," + realArg[0]);
			}
		}
	}
}

void CCodeGen::ArrIndex()
{
	int base = GetArrBase(curElement.arg1);
	string realOffset;
	string reg = regName[GetIdleReg()];
	BusyRegister(GetRegIndex(reg));
	string reg2 = regName[GetIdleReg()];
	curFuncInsList.push_back( "mov "+reg + "," + realArg[1] );
	curFuncInsList.push_back( "shl "+ reg +",2" );
	if (base < 0)
	{
		curFuncInsList.push_back( "add "+reg + "," + Int2string(-base));
		curFuncInsList.push_back( "mov " + reg2 + ",ebp" );
		curFuncInsList.push_back( "sub " + reg2 + "," + reg );
		realOffset = "[" + reg2 + "]";
		FreeRegister(GetRegIndex(reg));
	}
	else
	{
		curFuncInsList.push_back( "error" );
	}
	//---------------------------------------------------------------------------------
	if (nextElement.arg1 != curElement.result && nextElement.arg2 != curElement.result && nextElement.result != curElement.result)
	{
		string tmp = GetTmpVar();
		curFuncInsList.push_back( "mov " + tmp + "," + reg2 );
		tmp = "i:" + tmp;
		SetTmpVarValue(GetTmpVarIndex(curElement.result), tmp);
	}
	else
	{
		BusyRegister(GetRegIndex(reg2));
		SetTmpVarValue(GetTmpVarIndex(curElement.result), realOffset);
	}
	//---------------------------------------------------------------------------------------
}

//获取空闲的寄存器
//如果没有空闲的寄存器则将以当前指令为基准生命周期最长的中间变量交换入内存
int CCodeGen::GetIdleReg()
{
	for (int i = 0; i < 4; i++)
	{
		if (optionalReg[i].first)
		{
			return i;
		}
	}
	return -1;
}

int CCodeGen::GetArrBase(string base)
{
	base = base.substr(1, base.size() - 2);
	return atoi(base.c_str());
}

string CCodeGen::GetTmpVar()
{
	string espOffset = Int2string(curFunc.espOffset);
	curFunc.espOffset += 4;
	return "dword ptr ss:[ebp-" + espOffset + "]";
}

string CCodeGen::GetRealArg(string arg)
{
	if (JudgeMemVar(arg))
	{
		return GetRealMemVarValue(arg);
	}
	else if (JudgeTmpVar(arg))
	{
		string realArg = GetRealTmpVarValue(arg);
		string offset = realArg.substr(1, realArg.size() - 2);
		if (JudgeRegVar(offset))
		{
			arrIndexReg++;
			return "dword ptr ss:[" + offset + "]";
		}
		else if (JudgeIndexVar(realArg))
		{
			string memPos = realArg.substr(2, realArg.size() - 2);
			int reg = GetIdleReg();
			string idleReg = regName[reg];
			BusyRegister(reg);
			arrIndexReg++;
			curFuncInsList.push_back( "mov " + idleReg + "," + memPos );
			return "dword ptr ss:[" + idleReg + "]";
		}
		if (realArg == "")
		{
			cout << "get temp variable value failed. GetRealTmpVarValue()";
		}
		else
		{
			return realArg;
		}
	}
	else if (JudgeGlobalVar(arg))
	{
		return GetRealGlobalVarValue(arg);
	}
	else
	{
		return arg;
	}
}

bool CCodeGen::JudgeIndexVar(string arg)
{
	if (arg[0] == 'i' && arg[1] == ':')
	{
		return true;
	}
	return false;
}

bool CCodeGen::JudgeRegVar(string arg)
{
	for (int i = 0; i < 4; i++)
	{
		if (arg == regName[i])
		{
			return true;
		}
	}
	return false;
}

int CCodeGen::GetRegIndex(string name)
{
	for (int i = 0; i < 4; i++)
	{
		if (name == regName[i])
		{
			return i;
		}
	}
	return -1;
}

bool CCodeGen::JudgeMemVar(string arg)
{
	if (arg[0] == '[')
	{
		return true;
	}
	return false;
}

//获取内存变量真实值
string CCodeGen::GetRealMemVarValue(string arg)
{
	string offset = arg.substr(1, arg.size() - 2);
	if (JudgeRegVar(offset))
	{
		arrIndexReg++;
		return "dword ptr ss:[" + offset + "]";
	}
	return "dword ptr ss:[ebp"+offset+"]";
}

bool CCodeGen::JudgeTmpVar(string arg)
{
	if (arg[0] == 'T')
	{
		return true;
	}
	return false;
}

string CCodeGen::GetRealTmpVarValue(string arg)
{
	int index = -1;
	string sIndex = arg.substr(1, arg.size() - 1);
	index = atoi(sIndex.c_str());
	return curFunc.tmpVarList[index].value;
}

int CCodeGen::GetTmpVarIndex(string arg)
{
	string sIndex = arg.substr(1, arg.size() - 1);
	int index = atoi(sIndex.c_str());
	return index;
}

bool CCodeGen::JudgeGlobalVar(string arg)
{
	return false;
}

string CCodeGen::GetRealGlobalVarValue(string arg)
{
	return string();
}

void CCodeGen::FreeRegister(int index)
{
	optionalReg[index].first = true;
}

void CCodeGen::JudgeArgType()
{
	for (int i = 0; i < 2; i++)
	{
		if (JudgeRegVar(realArg[i]))
		{
			curArg[i] = REG;
			regIndex[i] = GetRegIndex(realArg[i]);
			argLifeEnd[i] = curFunc.tmpVarList[optionalReg[regIndex[i]].second].range.second;		//获取寄存器对应中间变量的生存周期的结束值
		}
		else if (JudgeMemVar(realArg[i]))
		{
			curArg[i] = MEM;
		}
		else
		{
			curArg[i] = IMME;
		}
	}
}

// -1:none 1:arg1 2:arg2 3:arg1&arg2
int CCodeGen::JudgeArgLifeEnd()
{
	int i = -1;
	if (argLifeEnd[0] == iCurElement)	//arg1 到达end
	{
		i = 1;
		if (argLifeEnd[1] == iCurElement)		//arg1 且 arg2 到达end
		{
			i = 3;
		}
	}
	else if (argLifeEnd[1] == iCurElement)	//仅arg2 到达end
	{
		i = 2;
	}
	return i;
}

void CCodeGen::BusyRegister(int index)
{
	optionalReg[index].first = false;
}

void CCodeGen::SetTmpVarValue(int index, string value)
{
	curFunc.tmpVarList[index].value = value;
}

void CCodeGen::InitRegister()
{
	for (int i = 0; i < 4; i++)
	{
		pair<bool, int> curReg;
		curReg.first = true;
		curReg.second = -1;
		optionalReg.push_back(curReg);
	}
}

int main()
{
	CCodeGen test;
	test.ReadElementFromFile();
	test.GetTmpVarInfo();
	test.FuncHandle();
	if (remove("MIDCODE.txt") != 0)
	{
		cout << "del midcode.txt failed" << endl;
	}
	if (remove("TEMPINDEX.txt") != 0)
	{
		cout << "del tempindex.txt failed" << endl;
	}
	WinExec("ml /c /coff final.asm",SW_HIDE);
	WinExec("link /subsystem:console final.obj", SW_HIDE);
	return 0;
}