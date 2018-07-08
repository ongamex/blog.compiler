#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <locale>
#include <assert.h>

enum TokenType : int
{
	tokenType_none,
	tokenType_number,
	tokenType_identifier,
	tokenType_string,

	// operators.
	tokenType_memberAccess,
	tokenType_comma,
	tokenType_assign, // =
	tokenType_less, // <
	tokenType_greater, // >
	//tokenType_lessEquals, // <=
	//tokenType_greaterEquals, // >=
	tokenType_equals, // ==
	tokenType_notEquals, // !=
	tokenType_plus, // +
	tokenType_minus, // -
	tokenType_asterisk, // *
	tokenType_slash, // /
	tokenType_lparen, // (
	tokenType_rparen, // )
	tokenType_blockBegin, // {
	tokenType_blockEnd, // }

	// keywords.
	tokenType_fn,
	tokenType_if,
	tokenType_else,
	tokenType_while,
	tokenType_print,
};

struct Token
{
	Token() = default;
	explicit Token(TokenType type) : 
		type(type)
	{}

	TokenType type = tokenType_none;
	float numberData;
	std::string strData;
};

struct Lexer
{
	Lexer(const char* const code) :
		m_code(code),
		m_ptr(code)
	{}

	void skipSpacesAhead()
	{
		while(isspace(*m_ptr))
			m_ptr++;
	}

	Token getNextToken()
	{
		skipSpacesAhead();

		if(*m_ptr == '\0')
		{
			// No more tokes to process, this should be it!
			Token token;
			return token;
		}
		else if(isalpha(*m_ptr) || *m_ptr == '_')
		{
			// This should be an indentifier or a keyword.
			Token token;
			while(isalpha(*m_ptr) || *m_ptr == '_' || isdigit(*m_ptr))
			{
				token.strData.push_back(*m_ptr);
				m_ptr++;
			}

			if(token.strData == "fn") {
				token.type = tokenType_fn;
				token.strData.clear();
			}
			else if(token.strData == "if") {
				token.type = tokenType_if;
				token.strData.clear();
			}
			else if(token.strData == "else") {
				token.type = tokenType_else;
				token.strData.clear();
			}
			else if(token.strData == "print") {
				token.type = tokenType_print;
				token.strData.clear();
			} 
			else if(token.strData == "while") {
				token.type = tokenType_while;
				token.strData.clear();
			}
			else {
				token.type = tokenType_identifier;
			}

			return token;
		}
		else if(*m_ptr == '"')
		{
			Token token;
			token.type = tokenType_string;

			m_ptr++;
			while(*m_ptr!='"')
			{
				token.strData.push_back(*m_ptr);
				m_ptr++;
			}
			m_ptr++;

			return token;
		}
		else if(*m_ptr == '.') {
			m_ptr++;
			return Token(tokenType_memberAccess);
		}
		else if(*m_ptr == ',') {
			m_ptr++;
			return Token(tokenType_comma);
		}
		else if(*m_ptr == '=') {
			if(m_ptr[1] == '=') {
				m_ptr+=2;
				return Token(tokenType_equals);
			} else {
				m_ptr++;
				return Token(tokenType_assign);
			}
		}
		else if(m_ptr[0] == '!' && m_ptr[1] == '=') {
			m_ptr+=2;
			return Token(tokenType_notEquals);
		}
		else if(*m_ptr == '<') {
			m_ptr++;
			return Token(tokenType_less);
		}
		else if(*m_ptr == '>') {
			m_ptr++;
			return Token(tokenType_greater);
		}
		else if(*m_ptr == '*') {
			m_ptr++;
			return Token(tokenType_asterisk);
		}
		else if(*m_ptr == '/') {
			m_ptr++;
			return Token(tokenType_slash);
		}
		else if(*m_ptr == '+') {
			m_ptr++;
			return Token(tokenType_plus);
		}
		else if(*m_ptr == '-') {
			m_ptr++;
			return Token(tokenType_minus);
		}
		else if(*m_ptr == '(') {
			m_ptr++;
			return Token(tokenType_lparen);
		}
		else if(*m_ptr == ')') {
			m_ptr++;
			return Token(tokenType_rparen);
		}
		else if(*m_ptr == '{') {
			m_ptr++;
			return Token(tokenType_blockBegin);
		}
		else if(*m_ptr == '}') {
			m_ptr++;
			return Token(tokenType_blockEnd);
		}
		else if(isdigit(*m_ptr))
		{
			// This should be a number.
			float numberAccum = 0;
			while(isdigit(*m_ptr)){
				const int digit = *m_ptr - '0';
				numberAccum = numberAccum*10.f + float(digit);
				m_ptr++;
			}

			if(*m_ptr == '.')
			{
				float mult = 0.1f;
				m_ptr++;
				while(isdigit(*m_ptr)){
					const int digit = *m_ptr - '0';
					numberAccum += mult * float(digit);
					m_ptr++;
				}
			}

			Token token;
			token.type = tokenType_number;
			token.numberData = numberAccum;

			return token;
		}

		// Should never happen.
		assert(false);
		return Token();
	}

	const char* m_code = nullptr;
	const char* m_ptr = nullptr;
};

enum AstNodeType
{
	astNodeType_invalid,
	astNodeType_number,
	astNodeType_string,
	astNodeType_identifier,
	astNodeType_memberAccess,
	astNodeType_tableMaker,
	astNodeType_binop,
	astNodeType_unop,
	astNodeType_fnCall,
	astNodeType_assign,
	astNodeType_statementList,
	astNodeType_if,
	astNodeType_while,
	astNodeType_print,
	astNodeType_fndecl,

};

struct AstNode
{
	AstNode(AstNodeType const type = astNodeType_invalid) :
		type(type)
	{}
	virtual ~AstNode() = default;

	AstNodeType type;
};

struct AstNumber : public AstNode
{
	AstNumber(float const value = NAN) 
		: AstNode(astNodeType_number)
		, value(value)
	{}

	float value = NAN;
};

struct AstString : public AstNode
{
	AstString(std::string s) 
		: AstNode(astNodeType_string)
		, value(std::move(s))
	{}

	std::string value;
};

struct AstIdentifier : public AstNode
{
	AstIdentifier(std::string identifier) 
		: AstNode(astNodeType_identifier)
		, identifier(identifier)
	{}

	std::string identifier;
};

struct AstMemberAcess : public AstNode
{
	AstMemberAcess(AstNode* const left, const std::string& memberName)
		: left(left)
		, memberName(memberName)
		, AstNode(astNodeType_memberAccess)
	{}

	AstNode* left = nullptr;
	std::string memberName;
};

struct AstTableMaker : public AstNode
{
	AstTableMaker()
		: AstNode(astNodeType_tableMaker)
	{}

	std::unordered_map<std::string, AstNode*> memberToExpression;
};

struct AstFnCall : public AstNode
{
	AstFnCall()
		: AstNode(astNodeType_fnCall)
	{}

	AstNode* theFunction = nullptr;
	std::vector<AstNode*> callArgs;
};

enum BinOp : int
{
	binop_add,
	binop_sub,
	binop_mul,
	binop_div,
	binop_equals,
	binop_notEquals,
	binop_less,
	binop_greater,
};

struct AstBinOp : public AstNode
{
	AstBinOp(const BinOp op, AstNode* const left, AstNode* const right) 
		: AstNode(astNodeType_binop)
		, op(op)
		, left(left)
		, right(right)
	{}

	AstNode* left;
	AstNode* right;
	BinOp op;
};

struct AstUnOp : public AstNode
{
	AstUnOp(char op, AstNode* left) 
		: AstNode(astNodeType_unop)
		, op(op)
		, left(left)
	{}

	AstNode* left = nullptr;
	char op; // + -
};

struct AstAssign : public AstNode
{
	AstAssign(AstNode* left, AstNode* right) 
		: AstNode(astNodeType_assign)
		, left(left)
		, right(right)
	{}

	AstNode* left = nullptr;
	AstNode* right = nullptr;
};

struct AstStatementList : public AstNode
{
	AstStatementList() :
		AstNode(astNodeType_statementList)
	{}

	bool needsOwnScope = true; // if specified when executing a new scope will be generated for the statement list.
	std::vector<AstNode*> m_statements;
};

struct AstIf : public AstNode
{
	AstIf() :
		AstNode(astNodeType_if)
	{}

	AstNode* expression = nullptr;
	AstNode* trueBranchStatement = nullptr;
	AstNode* falseBranchStatement = nullptr;
};

struct AstFnDecl : public AstNode
{
	AstFnDecl() :
		AstNode(astNodeType_fndecl)
	{}

	AstNode* fnBodyBlock = nullptr; // THe code of the function.
	std::vector<std::string> argsNames;
	int fnIdx = -1;
};

struct AstWhile : public AstNode
{
	AstWhile() :
		AstNode(astNodeType_while)
	{}

	AstNode* expression = nullptr;
	AstNode* trueBranchStatement = nullptr;
};

struct AstPrint : public AstNode
{
	AstPrint(AstNode* expression) :
		AstNode(astNodeType_print),
		expression(expression)
	{}

	AstNode* expression;
};

struct Parser
{
	AstNode* root = nullptr;
	const Token* m_token = nullptr;
	std::unordered_map<int, AstFnDecl*> m_fnIdx2fn;

	void registerFunction(AstFnDecl* const fnDecl) {
		fnDecl->fnIdx = m_fnIdx2fn.size();
		m_fnIdx2fn[m_fnIdx2fn.size()] = fnDecl;
	}

	void parse()
	{
		root = parse_programRoot();
	}

	// A block of statements or a single statement.
	AstNode* parse_statement()
	{
		if(m_token->type == tokenType_blockBegin) {
			matchAny();

			AstStatementList* const stmntList = new AstStatementList();
			while(m_token->type != tokenType_blockEnd)
			{
				AstNode* node = parse_statement();
				if(node) {
					stmntList->m_statements.push_back(node);
				} else {
					assert(false);
					break;
				}
			}

			match(tokenType_blockEnd);
			return stmntList;
		} 
		else if(m_token->type == tokenType_print)
		{
			matchAny();
			AstPrint* const astPrint = new AstPrint(parse_expression());
			return astPrint;
		}
		//else if(m_token->type == tokenType_if)
		//{
		//	return parse_if();
		//}
		else if(m_token->type == tokenType_while)
		{
			match(tokenType_while);
			AstWhile* const astWhile = new AstWhile();
			astWhile->expression = parse_expression();
			astWhile->trueBranchStatement = parse_statement();

			return astWhile;
		}
		else {
			return parse_expression();
		}

		assert(false);
		return nullptr;
	}

	AstNode* parse_programRoot()
	{
		AstStatementList* progRoot = new AstStatementList();

		while(m_token->type != tokenType_none) {
			AstNode* node = parse_statement();
			if(node) {
				progRoot->m_statements.push_back(node);
			}
			else {
				assert(false);
				break;
			}
		}

		return progRoot;
	}

	AstNode* parse_expression()
	{
		AstNode* left =  parse_expression6();

		// This whole expression could be a function call
		if(m_token->type == tokenType_lparen) {
			matchAny();

			AstFnCall* fnCall = new AstFnCall();
			fnCall->theFunction = left;

			while(m_token->type != tokenType_rparen)
			{
				AstNode* const arg = parse_expression();
				if(arg)
				{
					fnCall->callArgs.push_back(arg);
				}
				else
				{
					assert(false);
					return nullptr;
				}
			}
			match(tokenType_rparen);

			return fnCall;
		}
		
		return left;
	}

	AstNode* parse_expression_tableMaker()
	{
		match(tokenType_blockBegin);

		AstTableMaker* result = new AstTableMaker();

		while(m_token->type != tokenType_blockEnd)
		{
			if(m_token->type == tokenType_identifier)
			{
				AstNode*  &memberInitExpr = result->memberToExpression[m_token->strData];
				matchAny();
				memberInitExpr = parse_expression();
			}
			else
			{
				assert(false);
				return nullptr;
			}
		}
		match(tokenType_blockEnd);

		return result;
	}

	AstNode* parse_expression0()
	{
		if(m_token->type == tokenType_number)
		{
			AstNode* const result =  new AstNumber(m_token->numberData);
			matchAny();
			return result;
		}
		else if(m_token->type == tokenType_string)
		{
			AstNode* const result =  new AstString(m_token->strData);
			matchAny();
			return result;
		}
		else if(m_token->type == tokenType_identifier)
		{
			AstNode* const result =  new AstIdentifier(m_token->strData);
			matchAny();
			return result;
		}
		else if(m_token->type == tokenType_lparen)
		{
			matchAny();
			AstNode* const result =  parse_expression();
			match(tokenType_rparen);
			return result;
		}
		else if(m_token->type == tokenType_blockBegin)
		{
			return parse_expression_tableMaker();
		}
		else if(m_token->type == tokenType_if)
		{
			return parse_if();
		}
		else if(m_token->type == tokenType_fn)
		{
			return parse_fndecl();
		}

		assert(false);
		return nullptr;
	}

	AstNode* parse_expressionMeberAccess()
	{
		AstNode* const left = parse_expression0();

		if(m_token->type == tokenType_memberAccess)
		{
			matchAny();
			if(m_token->type == tokenType_identifier)
			{
				AstNode* const result = new AstMemberAcess(left, m_token->strData);
				match(tokenType_identifier);
				return result;
			}
			else
			{
				assert(false);
				return nullptr;
			}
		}
		
		return left;
	}

	AstNode* parse_if()
	{
		if(m_token->type == tokenType_if)
		{
			match(tokenType_if);
			AstIf* const astIf = new AstIf();
			astIf->expression = parse_expression();
			astIf->trueBranchStatement = parse_statement();

			if(m_token->type == tokenType_else) {
				matchAny();
				astIf->falseBranchStatement = parse_statement();
			}

			return astIf;
		}

		return nullptr;
	}

	AstNode* parse_fndecl()
	{
		AstFnDecl* fnDecl = nullptr;

		if(m_token->type == tokenType_fn)
		{
			fnDecl = new AstFnDecl();
			registerFunction(fnDecl);

			match(tokenType_fn);

			match(tokenType_lparen);
			while(m_token->type == tokenType_identifier) {
				fnDecl->argsNames.push_back(m_token->strData);
				match(tokenType_identifier);
			}
			match(tokenType_rparen);
		}

		fnDecl->fnBodyBlock = parse_statement();

		// If this is a block statement list, then force disable the specific scope for it, as we are going to use the scope of the function.
		if(fnDecl->fnBodyBlock && fnDecl->fnBodyBlock->type == astNodeType_statementList)
		{
			AstStatementList* const bodyStatementList = static_cast<AstStatementList*>(fnDecl->fnBodyBlock);
			bodyStatementList->needsOwnScope = false;
		}

		return fnDecl;
	}

	AstNode* parse_expression1()
	{
		if(m_token->type == tokenType_minus)
		{
			matchAny();
			AstUnOp* const result =  new AstUnOp('-', parse_expression());
			return result;
		}
		else if(m_token->type == tokenType_plus)
		{
			matchAny();
			AstUnOp* const result =  new AstUnOp('+', parse_expression());
			return result;
		}

		return parse_expressionMeberAccess();
	}

	AstNode* parse_expression2()
	{
		AstNode* left = parse_expression1();

		if(m_token->type == tokenType_asterisk)
		{
			matchAny();
			AstNode* retval = new AstBinOp(binop_mul, left, parse_expression2());
			return retval;
		}
		else if(m_token->type == tokenType_slash)
		{
			matchAny();
			AstNode* retval = new AstBinOp(binop_div, left, parse_expression2());
			return retval;
		}

		return left;
	}

	AstNode* parse_expression3()
	{
		AstNode* left = parse_expression2();

		if(m_token->type == tokenType_plus)
		{
			matchAny();
			AstNode* retval = new AstBinOp(binop_add, left, parse_expression());
			return retval;
		}
		else if(m_token->type == tokenType_minus)
		{
			matchAny();
			AstNode* retval = new AstBinOp(binop_sub, left, parse_expression());
			return retval;
		}

		return left;
	}

	AstNode* parse_expression4()
	{
		AstNode* left = parse_expression3();

		if(m_token->type == tokenType_equals)
		{
			matchAny();
			AstBinOp* equals = new AstBinOp(binop_equals, left, parse_expression());
			return equals;
		} 
		else if(m_token->type == tokenType_notEquals)
		{
			matchAny();
			AstBinOp* equals = new AstBinOp(binop_notEquals, left, parse_expression());
			return equals;
		}

		return left;
	}

	AstNode* parse_expression5()
	{
		AstNode* left = parse_expression4();

		if(m_token->type == tokenType_less)
		{
			matchAny();
			AstBinOp* equals = new AstBinOp(binop_less, left, parse_expression());
			return equals;
		}
		else if(m_token->type == tokenType_greater)
		{
			matchAny();
			AstBinOp* equals = new AstBinOp(binop_greater, left, parse_expression());
			return equals;
		}

		return left;
	}

	AstNode* parse_expression6()
	{

		AstNode* left = parse_expression5();

		if(m_token->type == tokenType_assign)
		{
			matchAny();
			AstAssign* assign = new AstAssign(left, parse_expression());
			return assign;
		}

		return left;	
	}

	void matchAny()
	{
		++m_token;
	}

	void match(TokenType const type)
	{
		if(m_token->type != type)
		{
			// A compilation error here!
			assert(false);
			return;
		}
		++m_token;
	}
};

enum VarType : int
{
	varType_undefined,
	varType_table,
	varType_f32,
	varType_string,
	varType_fn,
};

struct Var
{
	Var(VarType const varType)
		: m_varType(varType)
	{}

	void makeFloat32(const float value)
	{
		*this = Var(varType_f32);
		m_value_f32 = value;
	}

	void makeString(std::string s)
	{
		*this = Var(varType_string);
		m_value_string = std::move(s);
	}

	void makeTable()
	{
		*this = Var(varType_table);
	}

	void makeFunction(int functionIndex)
	{
		*this = Var(varType_fn);
		m_fnIdx = functionIndex;
	}

	//std::string m_name; // if applicable.
	VarType m_varType = varType_undefined; // Flags of enum VarFlag

	float m_value_f32 = 0.f;
	std::string m_value_string;
	std::unordered_map<std::string, Var*> m_tableLUT; // member name ot variable LUT
	int m_fnIdx = -1;
};

void printVariable(const Var* const expr)
{
	if(expr->m_varType == varType_f32)
		printf("%f\n", expr->m_value_f32);
	else if(expr->m_varType == varType_string)
		printf("%s\n", expr->m_value_string.c_str());
	else if(expr->m_varType == varType_fn)
		printf("<function %i>\n", expr->m_fnIdx);
	else if(expr->m_varType == varType_table)
	{
		printf("{ \n");
		for(auto& pair : expr->m_tableLUT)
		{
			printf("%s = ", pair.first.c_str());
			printVariable(pair.second);
			
		}
		printf(" }\n");
	}
	else
		printf("<undefined>\n");
};

struct Scope
{
	std::string scope;
};

struct Executor
{
	void pushScope(const AstNode* const node, const char* const postfix)
	{
		std::stringstream uniqueIdSS;
		uniqueIdSS << (size_t)(AstNode*)node;
		if(postfix) {
			uniqueIdSS << postfix;
		}

		std::string fullScope = !m_scopeStack.empty() 
			? m_scopeStack.back() + " " + uniqueIdSS.str()
			: uniqueIdSS.str();

		m_scopeStack.emplace_back(std::move(fullScope));
	}

	void popScope()
	{
		assert(m_scopeStack.empty() == false);
		m_scopeStack.pop_back();
	}

	Var* newVariableRaw(const char* nameCStr, const VarType varType) {
		
		Var* const result = new Var(varType);
		
		if(nameCStr!=nullptr) {
			//result->m_name = name;
			m_variablesLut[nameCStr] = result;
		}
		return result;
	}

	Var* newVariableFloat(float v) {
		Var* var = newVariableRaw(nullptr, (VarType)0); // HACK
		var->makeFloat32(v);
		return var;
	}

	Var* newVariableString(std::string v) {
		Var* var = newVariableRaw(nullptr, (VarType)0);
		var->makeString(std::move(v));
		return var;
	}

	Var* newVariableFunction(int fnIdx) {
		Var* var = newVariableRaw(nullptr, (VarType)0);
		var->makeFunction(fnIdx);
		return var;
	}

	Var* findVariableInScope(const std::string& baseName, bool createUndefinedIfMissing, bool shouldGoUpwardsIfMIssing) {

		for(int t = (int)(m_scopeStack.size()) - 1; t != -1; --t)
		{
			std::string name = (t == -1) ? baseName : m_scopeStack[t] + " " + baseName;;
		
			auto itr = m_variablesLut.find(name);
			if(itr == std::end(m_variablesLut)) {

				if(createUndefinedIfMissing) {
					return newVariableRaw(name.c_str(), varType_undefined);
				}
			} else {
				return itr->second;
			}

			if(shouldGoUpwardsIfMIssing == false)
				return nullptr;
		}

		return nullptr;
	}

	Var* evaluate(const AstNode* const root)
	{
		switch(root->type)
		{
			case astNodeType_number:
			{   
				return newVariableFloat(((AstNumber*)(root))->value);
			}break;
			case astNodeType_string:
			{   
				return newVariableString(((AstString*)(root))->value); // string
			}break;
			case astNodeType_identifier:
			{
				const AstIdentifier* const n = (AstIdentifier*)root;
				Var* result = findVariableInScope(n->identifier, false, true);
				if(!result) {
					result = findVariableInScope(n->identifier, true, false);
				}
				return result;
			}break;
			case astNodeType_fndecl:
			{
				const AstFnDecl* const n = (AstFnDecl*)root;
				Var* const result = newVariableFunction(n->fnIdx);
				return result;
			}break;
			case astNodeType_memberAccess:
			{
				const AstMemberAcess* const n = (AstMemberAcess*)root;
				Var* const left = evaluate(n->left);

				if(left->m_varType != varType_table) {
					assert(false);
					return nullptr;
				}

				Var* member = nullptr;

				auto itr = left->m_tableLUT.find(n->memberName);
				if(itr == std::end(left->m_tableLUT))
				{
					member = newVariableRaw(nullptr, varType_undefined);
					left->m_tableLUT[n->memberName] = member;
				}
				else
				{
					member = itr->second;
				}
				
				assert(member != nullptr);
				return member;
			}break;
			case astNodeType_tableMaker:
			{
				const AstTableMaker* const n = (AstTableMaker*)root;
				Var* result = newVariableRaw(nullptr, varType_table);
				for(const auto& pair : n->memberToExpression)
				{
					result->m_tableLUT[pair.first] = evaluate(pair.second);
				}

				return result;
			}
			case astNodeType_binop:
			{
				const AstBinOp* const n = (AstBinOp*)root;
				const Var* const left = evaluate(n->left);
				const Var* const right = evaluate(n->right);

				if(left->m_varType == varType_f32 && right->m_varType == varType_f32)
				{
					if(n->op == binop_add) return newVariableFloat(left->m_value_f32 + right->m_value_f32);
					else if(n->op == binop_sub) return newVariableFloat(left->m_value_f32 - right->m_value_f32);
					else if(n->op == binop_mul) return newVariableFloat(left->m_value_f32 * right->m_value_f32);
					else if(n->op == binop_div) return newVariableFloat(left->m_value_f32 / right->m_value_f32);
					else if(n->op == binop_equals) return newVariableFloat(left->m_value_f32 == right->m_value_f32);
					else if(n->op == binop_notEquals) return newVariableFloat(left->m_value_f32 != right->m_value_f32);
					else if(n->op == binop_less) return newVariableFloat(left->m_value_f32 < right->m_value_f32);
					else if(n->op == binop_greater) return newVariableFloat(left->m_value_f32 > right->m_value_f32);
					
				}
				if(left->m_varType == varType_string && n->op == binop_add)
				{
					// string + string
					if(right->m_varType == varType_string) {
						return newVariableString(left->m_value_string + right->m_value_string);
					}
					else if(right->m_varType == varType_f32) {
						std::stringstream ss;
						ss << right->m_value_f32;
						return newVariableString(left->m_value_string + ss.str());
					}
				}

				// Unknown operation.
				assert(false);
				return nullptr;
			}break;
			case astNodeType_unop:
			{
				const AstUnOp* const n = (AstUnOp*)root;
				const Var* const left = evaluate(n->left);

				if(left->m_varType != varType_f32) {
					assert(false);
					return nullptr;
				}

				const float v = n->op == '-' ?  -left->m_value_f32 : left->m_value_f32;
				return newVariableFloat(v);

			}break;
			case astNodeType_assign:
			{
				const AstAssign* const n = (AstAssign*)root;
				Var* const left = evaluate(n->left);
				const Var* const right = evaluate(n->right);
				*left = *right;
				return left;
			}break;
			case astNodeType_fnCall:
			{
				const AstFnCall* const n = (AstFnCall*)root;

				// TODO: deeply incomplete!
				Var* fn = evaluate(n->theFunction);
				if(fn && fn->m_varType == varType_fn)
				{
					auto itr = parser->m_fnIdx2fn.find(fn->m_fnIdx);
					if(itr != parser->m_fnIdx2fn.end())
					{
						const AstFnDecl* const fnToCallDecl = (AstFnDecl*)itr->second;
						pushScope(fnToCallDecl, nullptr);

						// Set the arguments
						if(fnToCallDecl->argsNames.size() == n->callArgs.size())
						{
							for(int iArg = 0; iArg < n->callArgs.size(); ++iArg)
							{
								Var* const arg = findVariableInScope(fnToCallDecl->argsNames[iArg], true, false);
								*arg = *evaluate(n->callArgs[iArg]);
							}
						}
						else
						{
							assert(false);
							popScope();
							return nullptr;
						}

						Var* result = evaluate(fnToCallDecl->fnBodyBlock);
						popScope();
						return result;
					}
				}

				assert(false);
				return nullptr;
			}break;
			case astNodeType_statementList:
			{
				const AstStatementList* const n = (AstStatementList*)root;

				if(n->needsOwnScope) {
					pushScope(n, nullptr);
				}

				for(AstNode* node : n->m_statements) {
					evaluate(node);
				}

				if(n->needsOwnScope) {
					popScope();
				}

				return nullptr;
			}break;
			case astNodeType_if:
			{
				const AstIf* const n = (AstIf*)root;
				const Var* const expr = evaluate(n->expression);

				if(expr->m_value_f32 != 0.f) {
					pushScope(n, "true");
					Var* const expr = evaluate(n->trueBranchStatement);
					popScope();
					return expr;
				}
				else if(n->falseBranchStatement) {
					pushScope(n, "false");
					Var* const expr = evaluate(n->falseBranchStatement);
					popScope();
					return expr;
				}

				return nullptr;
			}break;
			case astNodeType_while:
			{
				const AstWhile* const n = (AstWhile*)root;
				const Var* expr = evaluate(n->expression);

				pushScope(n, nullptr);
				while(expr->m_value_f32 != 0.f) {
					evaluate(n->trueBranchStatement);
					expr = evaluate(n->expression);
				}
				popScope();
	
				return nullptr;
			}break;
			case astNodeType_print:
			{
				const AstPrint* const n = (AstPrint*)root;
				const Var* const expr = evaluate(n->expression);

				printVariable(expr);

				return nullptr;
			}break;
		}

		assert(false);
		return nullptr;
	}

public :

	Parser* parser = nullptr;
	std::unordered_map<std::string, Var*> m_variablesLut;
	std::vector<Var*> m_allocatedVariables;
	std::vector<std::string> m_scopeStack;
};

void printNode(const AstNode* const n, const int tab)
{
	auto const ident = [](int cnt) {
		while(cnt) {
			printf("  ");
			cnt--;
		}
	};

	ident(tab);

	if(n->type == astNodeType_statementList)
	{
		printf("Statement List:\n");
		const AstStatementList* const progRoot = (AstStatementList*)n;
		for(AstNode* node : progRoot->m_statements) {
			printNode(node, tab + 1);
		}
	}
	else if(n->type == astNodeType_identifier)
	{
		const AstIdentifier* const ident = (AstIdentifier*)n;
		printf("ident(%s)", ident->identifier.c_str());
	}
	else if(n->type == astNodeType_number)
	{
		const AstNumber* const num = (AstNumber*)n;
		printf("num(%f)", num->value);
	}
	else if(n->type == astNodeType_binop)
	{
		const AstBinOp* const op = (AstBinOp*)n;
		printf(" (%c ", op->op);
		printNode(op->left, tab);
		printf(" ");
		printNode(op->right, tab);
		printf(") ");
	}
	else if(n->type == astNodeType_assign)
	{
		const AstAssign* const assign = (AstAssign*)n;
		printNode(assign->left, tab);
		printf(" = ");
		printNode(assign->right, tab);
		printf("\n");
	}
	else if(n->type == astNodeType_unop)
	{
		const AstUnOp* const op = (AstUnOp*)n;
		printf("u%c", op->op);
		printNode(op->left, tab);
	}
	else
	{
		printf("???\n");
	}
}

int main()
{
	const char* const testCode = R"(
(fn() print "Dummy Called") ()
foo = fn(f) print "foo called with arg f = " + f
table = {
	x 1
	y 2
}

foo = fn(f) print "foo called with arg f = " + f
f = 1
print "f="+f
foo(123)
print "f="+f


x = 3 * 2 * 4 * -(3 * 5) == -1
print x
x = 0
x = 1 + (x + 5) * 3 
print x
if x != 10 {
	print "true"
	x = x + 1
} else if x == 10 {
	print "false"
	x = x - 1
}

t = 0
while t != 10 {
	print "t = " + (t = t + 1)
}
q = if if t == 0 x else y 66 else 77
print "Zdrasti!"
)";

	Lexer lexer(testCode);

	std ::vector<Token> tokens;

	while(true)
	{
		const Token tok = lexer.getNextToken();
		tokens.push_back(tok);
	
		if(tok.type == tokenType_number) printf("%f ", tok.numberData);
		if(tok.type == tokenType_identifier) printf("IDENT(%s) ", tok.strData.c_str());
		if(tok.type == tokenType_assign) printf("= ");
		if(tok.type == tokenType_plus) printf("+ ");
		if(tok.type == tokenType_minus) printf("- ");
		if(tok.type == tokenType_asterisk) printf("* ");
		if(tok.type == tokenType_slash) printf("/ ");
		if(tok.type == tokenType_lparen) printf("(");
		if(tok.type == tokenType_rparen) printf(") ");
		if(tok.type == tokenType_none) { printf("\nNONE;\n "); break; }
	}

	Parser p;
	p.m_token = tokens.data();

	p.parse();
	AstNode* root = p.root;

	printNode(root, 0);

	printf("-------------------------------------Execution:\n");
	Executor e;
	e.parser = &p;
	e.evaluate(root);

	printf("-------------------------------------Variables:\n");

	

	for(auto p : e.m_variablesLut)
	{
		printf("%s = ", p.first.c_str());
		printVariable(p.second);
	}

	system("pause");
	return 0;
}