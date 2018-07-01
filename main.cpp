#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <locale>
#include <assert.h>

enum TokenType : int
{
	tokenType_none,
	tokenType_number,
	tokenType_identifier,
	tokenType_string,

	// operators.
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

			if(token.strData == "if") {
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
			// TODO: Floating point numbers.
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
	astNodeType_binop,
	astNodeType_unop,
	astNodeType_assign,
	astNodeType_statementList,
	astNodeType_if,
	astNodeType_while,
	astNodeType_print,

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
		else if(m_token->type == tokenType_if)
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
		return parse_expression6();
	}

	AstNode* parse_expression0()
	{
		if(m_token->type == tokenType_number)
		{
			AstNode* const result =  new AstNumber(m_token->numberData);
			matchAny();
			return result;
		}
		if(m_token->type == tokenType_string)
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

		assert(false);
		return nullptr;
	}

	AstNode* parse_expression1()
	{
		AstNode* left = parse_expression0();

		if(m_token->type == tokenType_asterisk)
		{
			matchAny();
			AstNode* retval = new AstBinOp(binop_mul, left, parse_expression0());
			return retval;
		}
		else if(m_token->type == tokenType_slash)
		{
			matchAny();
			AstNode* retval = new AstBinOp(binop_div, left, parse_expression0());
			return retval;
		}

		return left;
	}

	AstNode* parse_expression2()
	{
		AstNode* left = parse_expression1();

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


	AstNode* parse_expression3()
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
		
		return parse_expression2();
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
	varType_f32,
	varType_string,
};

struct Var
{
	std::string n_name; // if applicable.
	VarType m_varType = varType_undefined; // Flags of enum VarFlag

	float m_value_f32 = 0.f;
	std::string m_value_string;

	Var(VarType const varType)
		: m_varType(varType)
	{}

	void makeFloat32(const float value)
	{
		m_varType = varType_f32;
		m_value_f32 = value;
	}

	void makeString(std::string s)
	{
		m_varType = varType_string;
		m_value_string = std::move(s);
	}
};

struct Executor
{
	Var* newVariable(const char* const name, const VarType varType) {
		Var* const result = new Var(varType);
		if(name != nullptr) {
			result->n_name = name;
			m_variablesLut[name] = result;
		}
		return result;
	}

	Var* newVariable(float v) {
		Var* var = newVariable(nullptr, (VarType)0); // HACK
		var->makeFloat32(v);
		return var;
	}

	Var* newVariableString(std::string v) {
		Var* var = newVariable(0);
		var->makeString(std::move(v));
		return var;
	}

	Var* findVariable(const std::string name) {
		auto itr = m_variablesLut.find(name);
		if(itr == std::end(m_variablesLut)) {
			return nullptr;
		}

		return itr->second;
	}

	Var* evaluate(const AstNode* const root)
	{
		switch(root->type)
		{
			case astNodeType_number:
			{   
				return newVariable(((AstNumber*)(root))->value);
			}break;
			case astNodeType_string:
			{   
				return newVariableString(((AstString*)(root))->value); // string
			}break;
			case astNodeType_identifier:
			{
				const AstIdentifier* const n = (AstIdentifier*)root;
				Var* const result = findVariable(n->identifier);

				if(result == nullptr) {
					return newVariable(n->identifier.c_str(), varType_undefined);
				}

				return result;
			}break;
			case astNodeType_binop:
			{
				const AstBinOp* const n = (AstBinOp*)root;
				const Var* const left = evaluate(n->left);
				const Var* const right = evaluate(n->right);

				if(left->m_varType != varType_f32 || right->m_varType != varType_f32) {
					assert(false);
					return nullptr;
				}

				if(n->op == binop_add) return newVariable(left->m_value_f32 + right->m_value_f32);
				else if(n->op == binop_sub) return newVariable(left->m_value_f32 - right->m_value_f32);
				else if(n->op == binop_mul) return newVariable(left->m_value_f32 * right->m_value_f32);
				else if(n->op == binop_div) return newVariable(left->m_value_f32 / right->m_value_f32);
				else if(n->op == binop_equals) return newVariable(left->m_value_f32 == right->m_value_f32);
				else if(n->op == binop_notEquals) return newVariable(left->m_value_f32 != right->m_value_f32);
				else if(n->op == binop_less) return newVariable(left->m_value_f32 < right->m_value_f32);
				else if(n->op == binop_greater) return newVariable(left->m_value_f32 > right->m_value_f32);

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
				return newVariable(v);

			}break;
			case astNodeType_assign:
			{
				const AstAssign* const n = (AstAssign*)root;
				Var* const left = evaluate(n->left);
				const Var* const right = evaluate(n->right);
				left->m_value_f32 = right->m_value_f32;
				left->m_varType = right->m_varType;
				return left;
			}break;
			case astNodeType_statementList:
			{
				const AstStatementList* const n = (AstStatementList*)root;
				for(AstNode* node : n->m_statements)
				{
					evaluate(node);
				}
				return nullptr;
			}break;
			case astNodeType_if:
			{
				const AstIf* const n = (AstIf*)root;
				const Var* const expr = evaluate(n->expression);

				if(expr->m_value_f32 != 0.f) {
					return evaluate(n->trueBranchStatement);
				}
				else if(n->falseBranchStatement) {
					return evaluate(n->falseBranchStatement);
				}

				return nullptr;
			}break;
			case astNodeType_while:
			{
				const AstWhile* const n = (AstWhile*)root;
				const Var* expr = evaluate(n->expression);

				while(expr->m_value_f32 != 0.f) {
					evaluate(n->trueBranchStatement);
					expr = evaluate(n->expression);
				}
	
				return nullptr;
			}break;
			case astNodeType_print:
			{
				const AstPrint* const n = (AstPrint*)root;
				const Var* const expr = evaluate(n->expression);

				if(expr->m_varType == varType_f32)
					printf("%f\n", expr->m_value_f32);
				else if(expr->m_varType == varType_string)
					printf("%s\n", expr->m_value_string.c_str());
				else
					printf("<undefined>\n");

				return nullptr;
			}break;
		}

		assert(false);
		return nullptr;
	}

public :

	std::unordered_map<std::string, Var*> m_variablesLut;
	std::vector<Var*> m_allocatedVariables;
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
x = 5
x = x + 5
if x != 10 {
	x = x + 1
	print "true"
} else if x == 10 {
	x = x - 1
	z = 999
	print "false"
}

t = 0
while t != 10 {
	print t
	t = t + 1
}
y = x * (1 + 1)
print "y = "
print y
print "loktar ogar"
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
	e.evaluate(root);

	printf("-------------------------------------Variables:\n");

	for(auto p : e.m_variablesLut)
	{
		if(p.second->m_varType == varType_f32)
			printf("%s = %f \n", p.first.c_str(), p.second->m_value_f32);
		else if(p.second->m_varType == varType_string)
			printf("%s = %s \n", p.first.c_str(), p.second->m_value_string.c_str());
		else
			printf("%s = <undefined> \n", p.first.c_str());
	}

	system("pause");
	return 0;
}