#include "Syntax_Analyzer.h"

vector<WORD_TYPE>::iterator currentWord;
void CExp_Analyse::GetGlobalSymbol(CSymbol_Table * gSymbol)
{
	globlSymbol = gSymbol;
}

/*
Purpose:
	analyse var expression

Parameters:
	type	- (WordType) current word type

Return value:
	None
*/
void CExp_Analyse::Var_Analysis(WordType type,int parentNum)
{
	parent = parentNum;
	if (type == TK_ID)				//if variable has been identified
	{
		VarAssign_Analysis();
	}
	else
	{
		currentWord++;
		VarDefin_Analysis(type);
	}
}

/*
Purpose:
	analyse the variable definition statement

Parameters:
	type	- (WordType) var type

Return value:
	None
*/
void CExp_Analyse::VarDefin_Analysis(WordType type)
{
	if (currentWord->type == TK_COMMA)
	{
		ErrorMsgPrint("illegal character ',' at line %s", int2String(currentWord->line));
		return;
	}
	while (currentWord->type != TK_NEWLINE)				//attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{
		if (currentWord->type == TK_ID && !IsVar(currentWord->word))
		{
			SYMBOL_TABLE varInfo;
			if (type == KW_INT || type == KW_CHAR || type == KW_STR || type == KW_BOOL)		//determine basic types
			{
				BASE_TYPE newVar;
				newVar.name = currentWord->word;			//get variable name
				newVar.type = type;
				currentWord++;
				if (currentWord->type == TK_ASSIGN)			// '='
				{
					currentWord++;
					newVar.value = Exp_Analysis();			//get value
					symbolTable->InsertBaseVar(&newVar);
					if (parent == 1)
					{
						CBaseOpGen *baseOp = new CBaseOpGen(newVar.value, "", ASSIGN, newVar.memOffset);
						baseOp->CodeGen();
					}
				}
				else
				{
					newVar.isInit = false;
					symbolTable->InsertBaseVar(&newVar);
				}
				symbolTable->InsertSymbol(newVar.name, type, newVar.offset);
			}
			// array type define
			else if (type == KW_CHAR_ARRAY || type == KW_INT_ARRAY)	
			{
				ARRAY_TYPE newArrayVar;
				newArrayVar.name = currentWord->word;		//get array name
				newArrayVar.type = type;
				currentWord++;
				//get array variable information
				DetectSpecifiedVt(TK_OPENBR);
				if (currentWord->type == TK_INT && (currentWord + 1)->type == TK_CLOSEBR)			//defined array size
				{
					newArrayVar.len = String2int(currentWord->word);
					currentWord = currentWord + 2;
				}
				else if (currentWord->type == TK_CLOSEBR)					//unknown array size
				{
					newArrayVar.len = -1;
					currentWord++;
				}
				else
				{
					ErrorMsgPrint("array define wrong!,at line %s", int2String(currentWord->line));
					return;
				}
				// get array elements
				if (currentWord->type == TK_ASSIGN)				//array initialization
				{
					currentWord++;
					DetectSpecifiedVt(TK_BEGIN);
					// get array elements
					while (currentWord->type != TK_END && currentWord->type != TK_NEWLINE)
					{
						if (currentWord->type != TK_COMMA)			// new element
						{
							newArrayVar.arr.push_back(currentWord->word);
						}
						currentWord++;
					}
					// identify '}'
					DetectSpecifiedVt(TK_END);
					// determine array size
					if (newArrayVar.len == -1)
					{
						newArrayVar.len = newArrayVar.arr.size();
					}
					if (newArrayVar.len != newArrayVar.arr.size())
					{
						ErrorMsgPrint("The array size does not match the number of array elements ：%s ", newArrayVar.name);
						return;
					}
					// insert into symbol table
					symbolTable->InsertArrVar(&newArrayVar);
					symbolTable->InsertSymbol(newArrayVar.name, type, newArrayVar.offset);
					//generate initialization array code
					if (parent == 1)
					{
						CBaseOpGen *baseOp = new CBaseOpGen;
						for (int i = 0; i < newArrayVar.len; i++)
						{
							string arrElement = GetTempVar();
							baseOp->GetParam(newArrayVar.memOffset, int2String(i), INDEX, arrElement);
							baseOp->CodeGen();
							baseOp->GetParam(newArrayVar.arr[i], "", ASSIGN, arrElement);
							baseOp->CodeGen();
						}
					}
				}
				// array doesn't initialize
				else if (newArrayVar.len != -1)
				{
					symbolTable->InsertArrVar(&newArrayVar);
					symbolTable->InsertSymbol(newArrayVar.name, type, newArrayVar.offset);
				}
				else
				{
					ErrorMsgPrint("unknown array size :%s ", newArrayVar.name);
					return;
				}
			}
		}
		else if (currentWord->type == KW_SYSFUNC)
		{

		}
		else if(currentWord->type == TK_COMMA)
		{
			currentWord++;
			if (currentWord->word == "\n")
			{
				ErrorMsgPrint("illegel end");
				return;
			}
		}
	}
	currentWord++;
}

/*
Purpose:
	analyze assignment statements

Parameters:
	None

Return value:
	None
*/
void CExp_Analyse::VarAssign_Analysis()
{
	SYMBOL_TABLE varInfo = GetVarInfo(currentWord->word);
	CBaseOpGen *baseOp = new CBaseOpGen();
	currentWord++;
	while (currentWord->type != TK_NEWLINE)
	{
		// primitive type variable 
		if (varInfo.type == KW_CHAR || varInfo.type == KW_INT || varInfo.type == KW_BOOL || varInfo.type == KW_STR)
		{
			DetectSpecifiedVt(TK_ASSIGN);
			string value = Exp_Analysis();				//eax,mem offset,immediate data
			baseOp->GetParam(value, "", ASSIGN, varInfo.memOffset);
			baseOp->CodeGen();
		}
		// array type variable
		else
		{
			//left expression
			DetectSpecifiedVt(TK_OPENBR);
			//get array index
			string arrIndex = Exp_Analysis();
			DetectSpecifiedVt(TK_CLOSEBR);
			string arrElement = GetTempVar();
			baseOp->GetParam(varInfo.memOffset, arrIndex, INDEX, arrElement);
			baseOp->CodeGen();
			// right expression
			DetectSpecifiedVt(TK_ASSIGN);
			string value = Exp_Analysis();	// eax,mem offset,immediate data
			baseOp->GetParam(value, "", ASSIGN, arrElement);
			baseOp->CodeGen();
		}
	}
	currentWord++;
}

void CExp_Analyse::DetectSpecifiedVt(WordType vt)
{
	if (currentWord->type == vt)
	{
		currentWord++;
	}
	else
	{
		char buffer[100];
		sprintf_s(buffer, 100, "illegal character %s at line %d",currentWord->word.c_str(), currentWord->line);
		ErrorMsgPrint(buffer);
		currentWord++;
		system("Pause");
	}
}

int CExp_Analyse::ReturnTempIndex()
{
	return tempIndex;
}

/*
Purpose:
	get a temp variable

Parameters:
	None

Return value:
	string	- "dword ptr ss:[ebp-'offset']"
*/
string CExp_Analyse::GetTempVar()
{
	//int offset = symbolTable->espOffset;
	//symbolTable->espOffset += 4;
	//return "dword ptr ss:[ebp-" + int2String(offset) + "]";
	return "T" + int2String(tempIndex++);
}


//judge whether current word is a variable or not
bool CExp_Analyse::IsVar(string word)
{
	for (const auto i : symbolTable->symbollist)
	{
		if (i.name == word)
		{
			return true;
		}
	}
	if (globlSymbol != NULL)
	{
		for (const auto i : globlSymbol->symbollist)
		{
			if (i.name == word)
			{
				if (i.type == TK_FUNCNAME)
				{
					return false;
				}
				return true;
			}
		}
	}
	return false;
}

//get array element's index
string CExp_Analyse::GetArrElementIndex(int offset, int i,WordType arrType)
{
	int realOffset;
	if (arrType == KW_INT_ARRAY)
	{
		realOffset = offset + i * 4;
	}
	else
	{
		realOffset = offset + i;
	}
	return "[-" + int2String(realOffset)+"]";
}

//get current function's parameters
void CFunc_Analyse::GetParameters(FUNC_TYPE * func)
{
	varAnalyse->DetectSpecifiedVt(TK_OPENPA);
	while (currentWord->type != TK_CLOSEPA)
	{
		if (currentWord->type == KW_INT || currentWord->type == KW_BOOL || currentWord->type == KW_CHAR || currentWord->type == KW_STR)
		{
			pair<WordType, string> param;
			param.first = currentWord->type;
			currentWord++;
			if (currentWord->type == TK_ID)
			{
				param.second = (currentWord++)->word;
			}
			else
			{
				ErrorMsgPrint("illegal parameter name at line %s", int2String(currentWord->line));
				return;
			}
			func->param.push_back(param);
			func->paramNum++;
			paramList.push_back(param);
			funcSymbol.InsertSymbol(param.second, param.first, funcSymbol.paramOffset);
			funcSymbol.paramOffset -= 4;
		}
		else if (currentWord->type == TK_COMMA)
		{
			currentWord++;
		}
		else
		{
			ErrorMsgPrint("illegal symbol name at line %s", int2String(currentWord->line));
			return;
		}
	}
	currentWord++;
}

//Expression evaluation
string CExp_Analyse::Exp_Analysis()
{
	bool uminus = false;
	string operand1;
	if (currentWord->word == "-")
	{
		uminus = true;
		currentWord++;
	}
	operand1 = Logical_Analysis();
	if (uminus == true)
	{
		string temp = GetTempVar();
		CBaseOpGen *uminusOp = new CBaseOpGen("0", operand1, SUB, temp);
		uminusOp->CodeGen();
		operand1 = temp;
	}
	if (currentWord->type!= TK_OPERATOR)
	{
		return operand1;
	}
	while (currentWord->word == "+" || currentWord->word == "-")
	{
		string operand2;
		string Operator;
		string result = GetTempVar();
		Operator = currentWord->word;
		currentWord++;
		operand2 = Logical_Analysis();
		if (Operator == "+")
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2,  ADD, result);
			newOp->CodeGen();
		}
		else
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2,  SUB, result);
			newOp->CodeGen();
		}
		operand1 = result;
	}
	return operand1;
}
string CExp_Analyse::Logical_Analysis()
{
	string operand1;
	operand1 = Multi_Analysis();
	if (currentWord->type != TK_OPERATOR)
	{
		return operand1;
	}
	while (currentWord->word == "&" || currentWord->word == "|" || currentWord->word == "^")
	{
		string operand2;
		string Operator;
		string result = GetTempVar();
		Operator = currentWord->word;
		currentWord++;
		operand2 = Multi_Analysis();
		if (Operator == "&")
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2, AND, result);
			newOp->CodeGen();
		}
		else if (Operator == "|")
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2, OR, result);
			newOp->CodeGen();
		}
		else if (Operator == "^")
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2, XOR, result);
			newOp->CodeGen();
		}
		else
		{
			ErrorMsgPrint("unknow operator at line %s", int2String(currentWord->line));
			return "";
		}
		operand1 = result;
	}
	return operand1;
}

//Multiplication and division operation expression calculation
string CExp_Analyse::Multi_Analysis()
{
	string operand1;
	operand1 = BitOp_Analysis();
	if (currentWord->type != TK_OPERATOR)
	{
		return operand1;
	}
	while (currentWord->word == "*" || currentWord->word == "/")
	{
		string operand2;
		string Operator;
		string result = GetTempVar();
		Operator = currentWord->word;
		currentWord++;
		operand2 = BitOp_Analysis();
		if (Operator == "*")
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2, MUL, result);
			newOp->CodeGen();
		}
		else
		{
			CBaseOpGen *newOp = new CBaseOpGen(operand1, operand2, DIV, result);
			newOp->CodeGen();
		}
		operand1 = result;
	}
	return operand1;
}

//bitwise operation expression evaluation
string CExp_Analyse::BitOp_Analysis()
{
	bool Not = false;
	string operand1;
	if (currentWord->word == "~")
	{
		Not = true;
		currentWord++;
	}
	operand1 = Get_Terminator();
	if (Not == true)
	{
		string temp = GetTempVar();
		CBaseOpGen *NotOp = new CBaseOpGen(operand1,"",NOT, temp);
		NotOp->CodeGen();
		operand1 = temp;
	}
	return operand1;
}

//get terminator
string CExp_Analyse::Get_Terminator()
{
	if (currentWord->type == TK_ID)				//variable
	{
		SYMBOL_TABLE var;
		if (IsVar(currentWord->word))
		{
			var = GetVarInfo(currentWord->word);
		}
		else
		{
			FUNC_TYPE func;
			func = globlSymbol->GetFuncInfo(currentWord->word);
			currentWord++;
			return Func_Call(&func);
		}
		//array type variable
		if (var.type == KW_INT_ARRAY || var.type == KW_CHAR_ARRAY)
		{
			currentWord++;
			DetectSpecifiedVt(TK_OPENBR);
			string index;
			index = Exp_Analysis();			//get array index
			DetectSpecifiedVt(TK_CLOSEBR);
			string result = GetTempVar();
			CBaseOpGen *baseOp = new CBaseOpGen(var.memOffset, index, INDEX, result);
			baseOp->CodeGen();
			return result;
		}
		//normal variable
		else
		{
			currentWord++;
			//return TransVarToOffset(var.offset);
			return var.memOffset;
		}
	}
	else if (currentWord->type == TK_CHAR || currentWord->type == TK_INT || currentWord->type == TK_STR || currentWord->type == TK_BOOL)			//立即数
	{
		string value = currentWord->word;
		currentWord++;
		return value;
	}
	else if (currentWord->type == TK_OPENPA)		//括号情况
	{
		currentWord++;
		string value = Exp_Analysis();
		DetectSpecifiedVt(TK_CLOSEPA);
		return value;
	}
	else
	{
		ErrorMsgPrint("unknow expression at line %s",int2String(currentWord->line));
		return "";
	}
}

//get variable information from current function's symbol table
SYMBOL_TABLE CExp_Analyse::GetVarInfo(string var)
{
	for (const auto i : symbolTable->symbollist)
	{
		if (i.name == var)
		{
			return i;
		}
	}
	if (globlSymbol != NULL)
	{
		for (auto i : globlSymbol->symbollist)
		{
			if (i.name == var)
			{
				string tmp;
				tmp = "G:" + i.name;
				i.memOffset = tmp;
				return i;
			}
		}
	}
}

/*
Purpose:
	'if' compound statement analyse

Parameters:
	None

Return value:
	None
*/
void CFunc_Analyse::If_Analysis()
{
	CBaseOpGen *baseOp = new CBaseOpGen;
	string ifLabels[3];
	int iOprator;
	int iOppesiteOprator;
	ifLabels[0] = GetLabel();
	ifLabels[1] = GetLabel();
	ifLabels[2] = GetLabel();
	varAnalyse->DetectSpecifiedVt(TK_OPENPA);
	while (currentWord->type != TK_CLOSEPA)
	{
		string leftExp, rightExp;
		//string result = varAnalyse->GetTempVar();
		leftExp = varAnalyse->Exp_Analysis();			// left expression
		if (currentWord->type == TK_COMPARE)
		{
			iOprator = baseOp->JudgeOperator(currentWord->word);	// get compare operator
			iOppesiteOprator = baseOp->GetOppositeOp(currentWord->word);
			currentWord++;
		}
		else
		{
			ErrorMsgPrint("expect comparsion operator at if expression at line %s",int2String(currentWord->line));
			return;
		}
		rightExp = varAnalyse->Exp_Analysis();			// right expression

		//generate code here!!
		//baseOp->GetParam(leftExp, rightExp,(OP_TYPE)iOprator, result);
		//baseOp->CodeGen();
		if (currentWord->word == "||")
		{
			currentWord++;
			baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOprator, ifLabels[0]);
			baseOp->CodeGen();
		}
		else
		{
			if (currentWord->word == "&&")
			{
				currentWord++;
			}
			baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOppesiteOprator, ifLabels[1]);
			baseOp->CodeGen();
		}
	}
	currentWord++;
	IgnoreLineBreak();
	baseOp->GetParam(ifLabels[0], "", LABEL, "");
	baseOp->CodeGen();
	CompoundStat_Analysis();			// if compound statement
	IgnoreLineBreak();
	if (currentWord->type == KW_ELSE)
	{
		baseOp->GetParam("", "", JMP, ifLabels[2]);
		baseOp->CodeGen();
		baseOp->GetParam(ifLabels[1], "", LABEL, "");
		baseOp->CodeGen();
		currentWord++;
		IgnoreLineBreak();
		CompoundStat_Analysis();
		IgnoreLineBreak();
		baseOp->GetParam(ifLabels[2], "", LABEL, "");
		baseOp->CodeGen();
	}
	else
	{
		baseOp->GetParam(ifLabels[1], "", LABEL, "");
		baseOp->CodeGen();
	}
}


/*
Purpose:
	'while' compound statement analyse

Parameters:
	None

Return value:
	None
*/
void CFunc_Analyse::While_Analysis()			//此处需要对while结构做实际的逆向
{
	CBaseOpGen *baseOp = new CBaseOpGen;
	string leftExp, rightExp,countVar;
	string whileLables[3];
	int iOprator;
	int iOppesiteOprator;
	whileLables[0] = GetLabel();
	whileLables[1] = GetLabel();
	whileLables[2] = GetLabel();
	varAnalyse->DetectSpecifiedVt(TK_OPENPA);
	baseOp->GetParam(whileLables[0], "", LABEL, "");
	baseOp->CodeGen();
	while (currentWord->type != TK_CLOSEPA)
	{
		//string result = varAnalyse->GetTempVar();
		leftExp = varAnalyse->Exp_Analysis();
		if (currentWord->type == TK_COMPARE)
		{
			iOprator = baseOp->JudgeOperator(currentWord->word);
			iOppesiteOprator = baseOp->GetOppositeOp(currentWord->word);
			currentWord++;
		}
		else
		{
			ErrorMsgPrint("expect compare operator at while expression");
			return;
		}
		rightExp = varAnalyse->Exp_Analysis();
		//baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOprator, result);
		//baseOp->CodeGen();
		if (currentWord->word == "||")
		{
			currentWord++;
			baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOprator, whileLables[1]);
			baseOp->CodeGen();
		}
		else
		{
			if (currentWord->word == "&&")
			{
				currentWord++;
			}
			baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOppesiteOprator, whileLables[2]);
			baseOp->CodeGen();
		}
	}
	currentWord++;
	//newWhile->GetWhileInfo(countVar, tmpVar2, tmpVar, (COMPARE)iComOp, whileLabel,whileLable2,whileLable3);
	varAnalyse->DetectSpecifiedVt(TK_NEWLINE);
	baseOp->GetParam(whileLables[1], "", LABEL, "");
	baseOp->CodeGen();
	CompoundStat_Analysis(whileLables[2]);
	baseOp->GetParam("", "", JMP, whileLables[0]);
	baseOp->CodeGen();
	baseOp->GetParam(whileLables[2], "", LABEL, "");
	baseOp->CodeGen();
	IgnoreLineBreak();
}

void CFunc_Analyse::For_Analysis()
{
	SYMBOL_TABLE countVarInfo, forVar;
	CBaseOpGen *baseOp = new CBaseOpGen;
	CBaseOpGen *forVarOp = new CBaseOpGen;
	bool lastExp = false;
	string countVarInit;
	string forLables[3];
	forLables[0] = GetLabel();
	forLables[1] = GetLabel();
	forLables[2] = GetLabel();
	varAnalyse->DetectSpecifiedVt(TK_OPENPA);
	// first expression
	if (currentWord->type == TK_ID)
	{
		countVarInfo = varAnalyse->GetVarInfo(currentWord->word);
		currentWord++;
		varAnalyse->DetectSpecifiedVt(TK_ASSIGN);
		countVarInit = varAnalyse->Exp_Analysis();
		baseOp->GetParam(countVarInit, "", ASSIGN, countVarInfo.memOffset);
		baseOp->CodeGen();
	}

	varAnalyse->DetectSpecifiedVt(TK_SEMICOLON);
	// judge expression
	baseOp->GetParam(forLables[0], "", LABEL, "");
	baseOp->CodeGen();
	while (currentWord->type != TK_SEMICOLON)
	{
		//string result = varAnalyse->GetTempVar();
		string leftExp, rightExp;
		int iOprator;
		int iOppesiteOprator;
		leftExp = varAnalyse->Exp_Analysis();
		if (currentWord->type == TK_COMPARE)
		{
			iOprator = baseOp->JudgeOperator(currentWord->word);
			iOppesiteOprator = baseOp->GetOppositeOp(currentWord->word);
			currentWord++;
		}
		else
		{
			ErrorMsgPrint("expect comparsion operator at if expression");
			return;
		}
		rightExp = varAnalyse->Exp_Analysis();
		//baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOprator, result);
		//baseOp->CodeGen();
		if (currentWord->word == "||")
		{
			currentWord++;
			baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOprator, forLables[1]);
			baseOp->CodeGen();
		}
		else
		{
			if (currentWord->word == "&&")
			{
				currentWord++;
			}
			baseOp->GetParam(leftExp, rightExp, (OP_TYPE)iOppesiteOprator, forLables[2]);
			baseOp->CodeGen();
		}
	}
	varAnalyse->DetectSpecifiedVt(TK_SEMICOLON);
	// last expression
	if (currentWord->type == TK_ID)
	{
		string rightExp2;
		forVar = varAnalyse->GetVarInfo(currentWord->word);
		currentWord++;
		varAnalyse->DetectSpecifiedVt(TK_ASSIGN);
		rightExp2 = varAnalyse->Exp_Analysis();
		forVarOp->GetParam(rightExp2, "", ASSIGN, forVar.memOffset);
		lastExp = true;
	}
	varAnalyse->DetectSpecifiedVt(TK_CLOSEPA);
	baseOp->GetParam(forLables[1], "", LABEL, "");
	baseOp->CodeGen();
	CompoundStat_Analysis(forLables[1]);
	if (lastExp)
	{
		forVarOp->CodeGen();
	}
	baseOp->GetParam("", "", JMP, forLables[0]);
	baseOp->CodeGen();
	baseOp->GetParam(forLables[2], "", LABEL, "");
	baseOp->CodeGen();
}

void CFunc_Analyse::Break_Tag(string breakTag)
{
	if (breakTag != "")
	{
		cout << "JMP " << breakTag << endl;
	}
	else
	{
		ErrorMsgPrint("this is not a cycle! can't use 'break' at line %s",int2String(currentWord->line));
		return;
	}
}

// ignore line break
void CFunc_Analyse::IgnoreLineBreak()
{
	while (currentWord->type == TK_NEWLINE)
	{
		currentWord++;
	}
}

bool CFunc_Analyse::IsSystemFunc(string curWord)
{
	if (curWord == "print")
	{
		return true;
	}
	return false;
}
/*
Purpose:
	syntax analysis and generate target code(x86 assemble language)

Parameters:
	None

Return value:
	None
*/
void CSyntaxAnalyzer::Syntax_Analyzer()
{
	currentWord = vWord.begin();
	InsertSystemFuncInfo();
	while (currentWord != vWord.end())
	{
		if (currentWord->type == KW_SUBFUNC)
		{
			CFunc_Analyse *newFunc = new CFunc_Analyse(&globalSymbol);
			currentWord++;
			newFunc->Subfunc_Analysis();			//main function have to be handled idividually
		}
		else if (currentWord->type == KW_MAIN)
		{
			CFunc_Analyse *newFunc = new CFunc_Analyse(&globalSymbol);
			currentWord++;
			newFunc->Mainfunc_Analysis();
		}
		else if (currentWord->type == KW_INT || currentWord->type == KW_CHAR || currentWord->type == KW_BOOL || currentWord->type == KW_STR
			|| currentWord->type == KW_INT_ARRAY || currentWord->type == KW_CHAR_ARRAY
			|| varAnalyse->IsVar(currentWord->word))
		{
			varAnalyse->Var_Analysis(currentWord->type,2);
		}
		else if (currentWord->type == TK_NEWLINE)
		{
			currentWord++;
		}
		else
		{
			varAnalyse->ErrorMsgPrint("expected unqualified-id \'%s\'", currentWord->word);
			break;
		}
	}
	GlobalVarAnalyse();
}

void CSyntaxAnalyzer::InsertSystemFuncInfo()
{
	FUNC_TYPE* sysfunc = new FUNC_TYPE;
	pair<WordType, string> param;
	param.first = KW_INT;
	param.second = "aa";
	sysfunc->name = "print";
	sysfunc->param.push_back(param);
	sysfunc->paramNum = 1;
	sysfunc->type = KW_INT;
	globalSymbol.funclist.push_back(*sysfunc);
}

/*
Purpose:
	check subfunction struct
	generate symbol table
	generate target code

Parameters:
	None

Return value:
	None
*/
void CFunc_Analyse::Subfunc_Analysis()
{
	CFuncOpGen *geneFunc = new CFuncOpGen;
	FUNC_TYPE newFunc;
	varAnalyse->GetGlobalSymbol(globalSymbol);
	//identify function type
	if (currentWord->type == KW_CHAR || currentWord->type == KW_INT || currentWord->type == KW_BOOL)
	{
		newFunc.type = currentWord->type;
		currentWord++;
		//get function name
		if (currentWord->type == TK_ID)
		{
			geneFunc->GetFunName(currentWord->word);
			globalSymbol->InsertSymbol(currentWord->word, TK_FUNCNAME, 0);
			GetFuncName(currentWord->word);
			newFunc.name = currentWord->word;
			currentWord++;
		}
		else
		{
			varAnalyse->ErrorMsgPrint("unknown function name at line %s",int2String(currentWord->line));
			return;
		}
		GetParameters(&newFunc);
		for (const auto i : newFunc.param)
		{
			geneFunc->GetParameter(i.second);
		}
		geneFunc->StartCodeGen();
		globalSymbol->InsertFuncInfo(&newFunc);
		CompoundStat_Analysis();				//compound statement  function body analysis
		geneFunc->EndCodeGen();					//generate function end assemble code
	}
	else
	{
		varAnalyse->ErrorMsgPrint("subfunction type error at line %s", int2String(currentWord->line));
		return;
	}
	outTempIndex << varAnalyse->ReturnTempIndex()<<"," << funcSymbol.espOffset << endl;
}

string CExp_Analyse::Func_Call(FUNC_TYPE * funcInfo)
{
	CBaseOpGen *funCall = new CBaseOpGen;
	string retValue = GetTempVar();
	int i = 0;
	DetectSpecifiedVt(TK_OPENPA);
	if (currentWord->type == TK_COMMA)
	{
		ErrorMsgPrint("illegal character ',' at line %s ", int2String(currentWord->line));
		return "";
	}
	while (currentWord->type != TK_CLOSEPA)
	{
		if (currentWord->type == TK_COMMA)
		{
			currentWord++;
			continue;
		}
		if (i > funcInfo->paramNum)
		{
			ErrorMsgPrint("too much parameters at line %s", int2String(currentWord->line));
			return "";
		}
		SYMBOL_TABLE paramInfo;
		if ( currentWord->type != TK_ID)
		{
			ErrorMsgPrint("can't find defination of %s", currentWord->word);
			return "";
		}
		paramInfo = GetVarInfo(currentWord->word);
		if (funcInfo->name != "print")
		{
			if (funcInfo->param[i].first != paramInfo.type)
			{
				ErrorMsgPrint("parameter type error!");
				return "";
			}
		}
		currentWord++;
		funCall->GetParam(paramInfo.memOffset, "", PUSH, "");
		funCall->CodeGen();
		i++;
	}
	currentWord++;
	funCall->GetParam(funcInfo->name, "", CALL, retValue);
	funCall->CodeGen();
	return retValue;
}

void CFunc_Analyse::Mainfunc_Analysis()
{
	CFuncOpGen *geneFunc = new CFuncOpGen;
	varAnalyse->GetGlobalSymbol(globalSymbol);
	varAnalyse->DetectSpecifiedVt(TK_OPENPA);
	varAnalyse->DetectSpecifiedVt(TK_CLOSEPA);
	geneFunc->MainStartCodeGen();
	CompoundStat_Analysis();
	geneFunc->MainEndCodeGen();
	outTempIndex << varAnalyse->ReturnTempIndex() << "," << funcSymbol.espOffset<< endl;
}

/*
Purpose:
	analyse function body

Parameters:
	None

Return value:
	None
*/
void CFunc_Analyse::CompoundStat_Analysis(string cycleBreakLabel)
{
	IgnoreLineBreak();
	varAnalyse->DetectSpecifiedVt(TK_BEGIN);
	IgnoreLineBreak();
	while (currentWord->type != TK_END)
	{
		if (currentWord->type == KW_IF)		//if compound statement analyse
		{
			currentWord++;
			If_Analysis();
		}
		else if (currentWord->type == KW_WHILE)				//while compound statement analyse
		{
			currentWord++;
			While_Analysis();
		}
		else if (currentWord->type == KW_INT		//variable analyse
			|| currentWord->type == KW_CHAR
			|| currentWord->type == KW_INT_ARRAY
			|| currentWord->type == KW_CHAR_ARRAY
			|| currentWord->type == KW_BOOL
			|| currentWord->type == KW_STR
			|| varAnalyse->IsVar(currentWord->word))
		{
			varAnalyse->Var_Analysis(currentWord->type,1);
		}
		else if (currentWord->type == KW_FOR)		//for analyse
		{
			currentWord++;
			For_Analysis();
		}
		else if (globalSymbol->IsFunc(currentWord->word)||IsSystemFunc(currentWord->word))				//function call(include system function)
		{
			FUNC_TYPE funcInfo = globalSymbol->GetFuncInfo(currentWord->word);
			currentWord++;
			varAnalyse->Func_Call(&funcInfo);
		}
		else if (currentWord->type == KW_BREAK)
		{
			currentWord++;
			Break_Tag(cycleBreakLabel);
		}
		else if (currentWord->type == KW_RETURN)
		{
			currentWord++;
			if (currentWord->word == " ")
			{
				currentWord++;
			}
			if (currentWord->type == TK_ID || currentWord->type == TK_INT || currentWord->type == TK_CHAR || currentWord->type == TK_BOOL)
			{
				SYMBOL_TABLE varInfo = varAnalyse->GetVarInfo(currentWord->word);
				CBaseOpGen *retValue = new CBaseOpGen;
				retValue->GetParam(varInfo.memOffset, "", RETURN, "");
				retValue->CodeGen();
			}
		}
		IgnoreLineBreak();
	}
	currentWord++;
}

/*
Purpose:
	gets the assembly instruction label

Parameters:
	None

Return value:
	string	- label
*/
string CFunc_Analyse::GetLabel()
{
	stringstream ss;
	string index;
	string label = "LABEL";
	ss << labelIndex;
	ss >> index;
	labelIndex++;
	return label+index;
}

CFunc_Analyse::~CFunc_Analyse()
{
}

/*
Purpose:
	get the function name

Parameters:
	string	- function name

Return value:
	None
*/
void CFunc_Analyse::GetFuncName(string name)
{
	funcName = name;
}

void CSyntaxAnalyzer::GlobalVarAnalyse()
{
	CGlobalVarGen * globalVar = new CGlobalVarGen;
	for (const auto i : globalSymbol.baselist)
	{
		globalVar->GetParam(i.type,i.name,i.value);
		globalVar->CodeGen();
	}
	for (const auto i : globalSymbol.arrlist)
	{
		for (int j = 0; j < i.arr.size(); j++)
		{
			globalVar->GetParam(i.type, i.name, i.arr[j]);
		}
		globalVar->CodeGen();
	}
}