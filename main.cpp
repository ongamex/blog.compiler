#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <locale>
#include <assert.h>

#include <cstdarg>

// Identifies the type of the token (also know as lexeme) matched by the lexer.
// Some tokens store additional data with the token, like a float for number or
// a string for string literals and identifiers.
enum TokenType : int
{
	tokenType_none,
	tokenType_number,
	tokenType_identifier,
	tokenType_string,

	// operators.
	tokenType_dot,
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
	tokenType_lsqBracket, // [
	tokenType_rsqBracket, // ]
	tokenType_blockBegin, // {
	tokenType_blockEnd, // }
	tokenType_semicolon, // ;

	// keywords.
	tokenType_fn,
	tokenType_if,
	tokenType_else,
	tokenType_while,
	tokenType_for,
	tokenType_return,
	tokenType_print,
	tokenType_array,
};

// A location in our source code used primerly for error reporting.
struct Location
{
	int column = -1;
	int line = -1;
};

// A token (also known as lexeme) matched by the lexter.
// Tokens are basically the building blocks of the language.
// Example token types:
// keywords like: if/else/while
// special symbols like: < > = != == . , () [] ;
// identifiers, number and string literals
struct Token
{
	Token() = default;
	explicit Token(TokenType type, int column, int line) : 
		type(type),
		location({column, line})
	{}

	TokenType type = tokenType_none;
	float numberData; // The number asociated with this token (if any).
	std::string strData; // The string asociated with this token (if any). Example usage is with identifiers or string literals.
	Location location; // The location of the token in the source file.
};

// The lexer takes the input text, and converts it to a linear set of tokens(also know as lexemes).
struct Lexer
{
	int column = 0;
	int line = 0;

	Lexer(const char* const code) :
		m_code(code),
		m_ptr(code)
	{}

	void eatChar() {
		if(*m_ptr == '\n') {
			column=0;
			line++;
		} else {
			column++;
		}
		m_ptr++;
	}

	void skipSpacesAhead()
	{
		while(isspace(*m_ptr)) {
			eatChar();
		}
	}

	Token getNextToken()
	{
		skipSpacesAhead();

		if(*m_ptr == '\0')
		{
			// No more tokes to process, this should be it!
			return Token(tokenType_none, column, line);
		}
		else if(isalpha(*m_ptr) || *m_ptr == '_')
		{
			// This should be an indentifier or a keyword.
			Token token;
			while(isalpha(*m_ptr) || *m_ptr == '_' || isdigit(*m_ptr)) {
				token.strData.push_back(*m_ptr);
				eatChar();
			}

			// Check if this is not an identifier but a keyword.
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
			else if(token.strData == "return") {
				token.type = tokenType_return;
				token.strData.clear();
			} 
			else if(token.strData == "while") {
				token.type = tokenType_while;
				token.strData.clear();
			}
			else if(token.strData == "for") {
				token.type = tokenType_for;
				token.strData.clear();
			}else if(token.strData == "array") {
				token.type = tokenType_array;
				token.strData.clear();
			}
			else {
				token.type = tokenType_identifier;
			}

			token.location.column = column;
			token.location.line = line;

			return token;
		}
		else if(*m_ptr == '"')
		{
			Token token;
			token.type = tokenType_string;

			eatChar();
			while(*m_ptr!='"')
			{
				token.strData.push_back(*m_ptr);
				eatChar();
			}
			eatChar();

			return token;
		}
		else if(*m_ptr == ';') {
			eatChar();
			return Token(tokenType_semicolon, column, line);
		}
		else if(*m_ptr == '.') {
			eatChar();
			return Token(tokenType_dot, column, line);
		}
		else if(*m_ptr == ',') {
			eatChar();
			return Token(tokenType_comma, column, line);
		}
		else if(*m_ptr == '=') {
			if(m_ptr[1] == '=') {
				eatChar();
				eatChar();
				return Token(tokenType_equals, column, line);
			} else {
				eatChar();
				return Token(tokenType_assign, column, line);
			}
		}
		else if(m_ptr[0] == '!' && m_ptr[1] == '=') {
			eatChar();
			eatChar();
			return Token(tokenType_notEquals, column, line);
		}
		else if(*m_ptr == '<') {
			eatChar();
			return Token(tokenType_less, column, line);
		}
		else if(*m_ptr == '>') {
			eatChar();
			return Token(tokenType_greater, column, line);
		}
		else if(*m_ptr == '*') {
			eatChar();
			return Token(tokenType_asterisk, column, line);
		}
		else if(*m_ptr == '/') {
			eatChar();
			return Token(tokenType_slash, column, line);
		}
		else if(*m_ptr == '+') {
			eatChar();
			return Token(tokenType_plus, column, line);
		}
		else if(*m_ptr == '-') {
			m_ptr++;
			return Token(tokenType_minus, column, line);
		}
		else if(*m_ptr == '(') {
			eatChar();
			return Token(tokenType_lparen, column, line);
		}
		else if(*m_ptr == ')') {
			eatChar();
			return Token(tokenType_rparen, column, line);
		}
		else if(*m_ptr == '{') {
			eatChar();
			return Token(tokenType_blockBegin, column, line);
		}
		else if(*m_ptr == '}') {
			eatChar();
			return Token(tokenType_blockEnd, column, line);
		}
		else if(*m_ptr == '[') {
			eatChar();
			return Token(tokenType_lsqBracket, column, line);
		}
		else if(*m_ptr == ']') {
			eatChar();
			return Token(tokenType_rsqBracket, column, line);
		}
		else if(isdigit(*m_ptr))
		{
			// This should be a number.
			float numberAccum = 0;
			while(isdigit(*m_ptr)){
				const int digit = *m_ptr - '0';
				numberAccum = numberAccum*10.f + float(digit);
				eatChar();
			}

			if(*m_ptr == '.')
			{
				float mult = 0.1f;
				eatChar();
				while(isdigit(*m_ptr)){
					const int digit = *m_ptr - '0';
					numberAccum += mult * float(digit);
					eatChar();
				}
			}

			Token token;
			token.type = tokenType_number;
			token.numberData = numberAccum;
			token.location.column = column;
			token.location.line = line;

			return token;
		}

		// Should never happen.
		assert(false);
		return Token();
	}

	const char* m_code = nullptr;
	const char* m_ptr = nullptr;
};

enum AstNodeType {
	astNodeType_invalid,
	astNodeType_number,
	astNodeType_string,
	astNodeType_identifier,
	astNodeType_memberAccess,
	astNodeType_tableMaker,
	astNodeType_arrayMaker,
	astNodeType_binop,
	astNodeType_unop,
	astNodeType_fnCall,
	astNodeType_builtInFnCall,
	astNodeType_arrayIndexing,
	astNodeType_assign,
	astNodeType_statementList,
	astNodeType_if,
	astNodeType_while,
	astNodeType_for,
	astNodeType_print,
	astNodeType_return,
	astNodeType_fndecl,

};

// AstNode is the base class for our nodes in our Abstract Syntax Tree (AST for short).
// Each node represents a basic operation like:
// addition, substraction, multiplication, assign and many more- All these combined in some way form an expression.
// statements - expression, if, while, return, block of statements and so on.
struct AstNode
{
	AstNode(AstNodeType const type = astNodeType_invalid) :
		type(type)
	{}
	
	virtual ~AstNode() = default;

	AstNodeType type;
};

// AstNode representing a single number literal (basically AstNode representation of the matched token by the lexer).
struct AstNumber : public AstNode
{
	AstNumber(float const value = NAN) 
		: AstNode(astNodeType_number)
		, value(value)
	{}

	float value = NAN;
};

// AstNode representing a single string literal (basically AstNode representation of the matched token by the lexer).
struct AstString : public AstNode
{
	AstString(std::string s) 
		: AstNode(astNodeType_string)
		, value(std::move(s))
	{}

	std::string value;
};

// AstNode representing a single identifer (basically AstNode representation of the matched token by the lexer).
struct AstIdentifier : public AstNode
{
	AstIdentifier(std::string identifier) 
		: AstNode(astNodeType_identifier)
		, identifier(identifier)
	{}

	std::string identifier;
};

// AstNode representing the operation of acessing a member variable in a table(known as dictionaly or map in some languages).
// Exmple: <expression>.<member> like point.x
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

// AstNode representing a set of nodes used to create a table.
// Example: { x = 5; y = 10; }
struct AstTableMaker : public AstNode
{
	AstTableMaker()
		: AstNode(astNodeType_tableMaker)
	{}

	std::unordered_map<std::string, AstNode*> memberToExpression;
};

// AstNode representing a set of nodes used to create an array.
// Example: array{ 5, 10, 15, 20 }
struct AstArrayMaker : public AstNode
{
	AstArrayMaker()
		: AstNode(astNodeType_arrayMaker)
	{}

	std::vector<AstNode*> arrayElements;
};

// AstNode representing a function call.
// The function is obtained by evaluating the expresion on the left of the '('
// everything else, until the matching ')' is concidered a function argument.
struct AstFnCall : public AstNode
{
	AstFnCall()
		: AstNode(astNodeType_fnCall)
	{}

	AstNode* theFunction = nullptr; // The node that we are going to evaluate to obtain the function that we're going to call.
	std::vector<AstNode*> callArgs; // The node that we are going to evalute in order to pass the parameters to the function.
};

// AstNode representing an array indexing.
// The array is obtained by evaluating the expresion on the left of the '['
// everything else, until the matching ']' is going to be used as index.
struct AstArrayIndexing : public AstNode
{
	AstArrayIndexing()
		: AstNode(astNodeType_arrayIndexing)
	{}

	AstNode* theArray = nullptr; // The node that we are going to evaluate to obtain the array variable.
	AstNode* index = nullptr; // The node that we are going to evaluate to obtain the index.
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

struct AstFor : public AstNode
{
	AstFor() :
		AstNode(astNodeType_for)
	{}

	AstNode* initExpression = nullptr;
	AstNode* expression = nullptr;
	AstNode* postIterationExpression = nullptr;
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

struct AstReturn : public AstNode
{
	AstReturn() :
		AstNode(astNodeType_return),
		expression(nullptr)
	{}

	AstNode* expression;
};

struct Error
{
	Location location;
	std::string message;
};

struct Parser
{
	AstNode* root = nullptr;
	const Token* m_token = nullptr;
	std::unordered_map<int, AstFnDecl*> m_fnIdx2fn;

	std::vector<Error> m_errors;

	bool hasErrors() const {
		return !m_errors.empty();
	}

	void reportError(Error e) {
		m_errors.emplace_back(std::move(e));
	}

	void reportError(const Location& loc, const char* const fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char msg[1024] = { 0 };
		snprintf(msg, sizeof(msg), fmt, args);
		va_end(args);
		reportError({loc, msg});
	}

	void registerFunction(AstFnDecl* const fnDecl) {
		fnDecl->fnIdx = m_fnIdx2fn.size();
		m_fnIdx2fn[m_fnIdx2fn.size()] = fnDecl;
	}

	void parse()
	{
		root = parse_programRoot();
	}

	// A block of statements or a single statement.
	AstNode* parse_statement_block() {
		if(m_token->type == tokenType_blockBegin) {
			match(tokenType_blockBegin);

			AstStatementList* const stmntList = new AstStatementList();
			while(m_token->type != tokenType_blockEnd)
			{
				AstNode* node = parse_statement();
				if(node) {
					stmntList->m_statements.push_back(node);
				} else {
					reportError(m_token->location, "Failed to parse a statement");
					assert(false);
					break;
				}
			}
			match(tokenType_blockEnd);
			return stmntList;
		} else {
			return nullptr;
		}
	}

	AstNode* parse_statement() {
		if(m_token->type == tokenType_blockBegin)
		{
			return parse_statement_block();
		}

		return parse_single_statement();
	}

	AstNode* parse_single_statement()
	{
		if(m_token->type == tokenType_print)
		{
			match(tokenType_print);
			AstPrint* const astPrint = new AstPrint(parse_expression());
			match(tokenType_semicolon);
			return astPrint;
		}
		else if(m_token->type == tokenType_if)
		{
			// Add if as a statement so we don't have to add a semicolon after it, when we don't use it in an expression.
			AstNode* ifNode = parse_expression_if();

			if(ifNode == nullptr) {
				reportError(m_token->location, "failed to parse if expression");
			}

			return ifNode;
		}
		else if(m_token->type == tokenType_while)
		{
			match(tokenType_while);
			AstWhile* const astWhile = new AstWhile();
			astWhile->expression = parse_expression();
			astWhile->trueBranchStatement = parse_statement_block();

			return astWhile;
		}
		else if(m_token->type == tokenType_for)
		{
			match(tokenType_for);
			AstFor* const astFor = new AstFor();

			astFor->initExpression = parse_expression();
			match(tokenType_semicolon);
			
			astFor->expression = parse_expression();
			match(tokenType_semicolon);

			astFor->postIterationExpression = parse_expression();
			match(tokenType_semicolon);

			astFor->trueBranchStatement = parse_statement_block();

			return astFor;
		}
		else if(m_token->type == tokenType_return)
		{
			match(tokenType_return);
			AstReturn* const astReturn = new AstReturn;

			if(m_token->type != tokenType_semicolon) {
				astReturn->expression = parse_expression();
			}

			match(tokenType_semicolon);
			return astReturn;
		}
		else {
			AstNode* expr = parse_expression();
			match(tokenType_semicolon);
			return expr;
		}

		assert(false);
		return nullptr;
	}

	AstNode* parse_programRoot()
	{
		AstStatementList* progRoot = new AstStatementList();
		progRoot->needsOwnScope = false;
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
		AstNode* left = parse_expression6();
		
		return left;
	}

	AstNode* parse_expression_if()
	{
		if(m_token->type == tokenType_if)
		{
			match(tokenType_if);
			AstIf* const astIf = new AstIf();
			astIf->expression = parse_expression();
			if(astIf->expression == nullptr) {
				reportError(m_token->location, "Failed to parse if condition expression");
			}

			astIf->trueBranchStatement = parse_statement_block();

			if(m_token->type == tokenType_else) {
				match(tokenType_else);
				astIf->falseBranchStatement = parse_statement_block();
			}

			return astIf;
		}

		reportError(m_token->location, "Expected if token");

		return nullptr;
	}

	AstNode* parse_expression_fndecl()
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

		fnDecl->fnBodyBlock = parse_statement_block();
		assert(fnDecl->fnBodyBlock != nullptr);

		// If this is a block statement list, then force disable the specific scope for it, as we are going to use the scope of the function.
		if(fnDecl->fnBodyBlock && fnDecl->fnBodyBlock->type == astNodeType_statementList) {
			AstStatementList* const bodyStatementList = static_cast<AstStatementList*>(fnDecl->fnBodyBlock);
			bodyStatementList->needsOwnScope = false;
		}

		return fnDecl;
	}

	AstNode* parse_expression_tableMaker()
	{
		match(tokenType_blockBegin);
		AstTableMaker* const result = new AstTableMaker();

		while(m_token->type != tokenType_blockEnd)
		{
			// { identifer = expression; ... }
			if(m_token->type == tokenType_identifier) {
				AstNode*& memberInitExpr = result->memberToExpression[m_token->strData];
				match(tokenType_identifier);
				match(tokenType_assign);
				memberInitExpr = parse_expression();

				if(!memberInitExpr) {
					assert(false);
					return nullptr;
				}	

				match(tokenType_semicolon);
			} else {
				reportError(m_token->location, "Expected an identifier for member initialization when creating a table");
				assert(false);
				return nullptr;
			}
		}
		match(tokenType_blockEnd);

		return result;
	}

	AstNode* parse_expression_arrayMaker()
	{
		match(tokenType_array);
		match(tokenType_blockBegin);

		AstArrayMaker* result = new AstArrayMaker();

		while(m_token->type != tokenType_blockEnd) {
			result->arrayElements.push_back(parse_expression());

			if(m_token->type == tokenType_comma) {
				match(tokenType_comma);
			} else if(m_token->type == tokenType_blockEnd) {
				match(tokenType_blockEnd);
				break;
			} else {
				reportError(m_token->location, "Expected }");
				return nullptr;
			}
		}

		return result;
	}

	AstNode* parse_expression0()
	{
		AstNode* left = nullptr;
		if(m_token->type == tokenType_number)
		{
			left = new AstNumber(m_token->numberData);
			matchAny();
		}
		else if(m_token->type == tokenType_string)
		{
			left = new AstString(m_token->strData);
			matchAny();
		}
		else if(m_token->type == tokenType_identifier)
		{
			left = new AstIdentifier(m_token->strData);
			matchAny();
		}
		else if(m_token->type == tokenType_lparen)
		{
			matchAny();
			left = parse_expression();
			match(tokenType_rparen);
		}
		else if(m_token->type == tokenType_blockBegin)
		{
			left = parse_expression_tableMaker();
		}
		else if(m_token->type == tokenType_array)
		{
			left = parse_expression_arrayMaker();
		}
		else if(m_token->type == tokenType_if)
		{
			left = parse_expression_if();
		}
		else if(m_token->type == tokenType_fn)
		{
			left = parse_expression_fndecl();
		}

		//
		if(left == nullptr) {
			reportError(m_token->location, "Unknown expression");
			assert(false);
			return nullptr;
		}

		// In Addition, this thing could be a function call for an array indexing.
		while(m_token->type == tokenType_lparen || m_token->type == tokenType_lsqBracket || m_token->type == tokenType_dot)
		{
			if(m_token->type == tokenType_lparen)
			{
				match(tokenType_lparen);

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

				left = fnCall;
			}
			else if(m_token->type == tokenType_lsqBracket)
			{
				match(tokenType_lsqBracket);

				AstArrayIndexing* arrayIndexing = new AstArrayIndexing();
				arrayIndexing->theArray = left;
				arrayIndexing->index = parse_expression();

				match(tokenType_rsqBracket);

				left = arrayIndexing;
			}
			else if(m_token->type == tokenType_dot)
			{
				match(tokenType_dot);
				assert(m_token->strData.size() > 0);
				AstMemberAcess* const memberAcess = new AstMemberAcess(left, m_token->strData);
				match(tokenType_identifier);
				left = memberAcess;
			}
		}
	
		return left;
	}

	AstNode* parse_expression1()
	{
		if(m_token->type == tokenType_minus)
		{
			matchAny();
			AstUnOp* const result =  new AstUnOp('-', parse_expression0());
			return result;
		}
		else if(m_token->type == tokenType_plus)
		{
			matchAny();
			AstUnOp* const result =  new AstUnOp('+', parse_expression0());
			return result;
		}
		else
		{
			return parse_expression0();
		}

		assert(false);
		return nullptr;
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
			match(tokenType_assign);
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
			reportError(m_token->location, "Unexpected token");
			return;
		}
		++m_token;
	}

	bool is_matched(TokenType const type) {
		if(m_token->type == type) {
			match(type);
			return true;
		}

		return false;
	}
};

enum VarType : int
{
	varType_undefined,
	varType_table,
	varType_array,
	varType_f32,
	varType_string,
	varType_fn,
	varType_fnNative, // A C++ function basically.
};

struct Var;
struct Executor;
typedef int (*NativeFnPtr)(int argc, Var* argv[], Executor* exec, Var** ppResultVariable);

struct Var
{
	Var(VarType const varType)
		: m_varType(varType)
	{
		if(varType == varType_table) {
			m_tableLUT = std::make_shared<std::unordered_map<std::string, Var*>>();
		}

		if(varType == varType_array) {
			m_arrayValues = std::make_shared<std::vector<Var*>>();
		}
	}

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

	void makeArray()
	{
		*this = Var(varType_array);
	}

	void makeFunction(int functionIndex)
	{
		*this = Var(varType_fn);
		m_fnIdx = functionIndex;
	}

	void makeNativeFunction(NativeFnPtr const nativeFn) {
		*this = Var(varType_fnNative);
		m_fnNative = nativeFn;
	}

public :

	VarType m_varType = varType_undefined; // Flags of enum VarFlag

	float m_value_f32 = 0.f;
	int m_fnIdx = -1;
	NativeFnPtr m_fnNative = nullptr;
	std::string m_value_string;
	std::shared_ptr<std::unordered_map<std::string, Var*>> m_tableLUT; // member name ot variable
	std::shared_ptr<std::vector<Var*>> m_arrayValues; // member name ot variable
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
		if(expr->m_tableLUT)
		for(auto& pair : *expr->m_tableLUT)
		{
			printf("%s = ", pair.first.c_str());
			printVariable(pair.second);
			
		}
		printf(" }\n");
	}
	else if(expr->m_varType == varType_array)
	{
		printf("[ \n");
		if(expr->m_arrayValues)
			for(const Var* const var : *expr->m_arrayValues)
			{
				printVariable(var);
			}
		printf(" ]\n");
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
	Executor()
	{
		addStnadardLibFunctions();
	}

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

	Var* newVariableNativeFunction(const char* name, NativeFnPtr fnPtr) {
		Var* var = newVariableRaw(name, (VarType)0);
		var->makeNativeFunction(fnPtr);
		return var;
	}

	Var* findVariableInScope(const std::string& baseName, bool createUndefinedIfMissing, bool shouldGoUpwardsIfMIssing) {

		for(int t = (int)(m_scopeStack.size()) - 1; t != -2; --t)
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

	struct EvalCtx
	{
		Var* forcedResult = nullptr; // used by return statements to pass the result.
	};

	Var* evaluate(const AstNode* const root, EvalCtx& ctx)
	{
		if(ctx.forcedResult != nullptr) {
			return ctx.forcedResult;
		}

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
				Var* const left = evaluate(n->left, ctx);

				if(left->m_varType != varType_table || !left->m_tableLUT) {
					assert(false);
					return nullptr;
				}

				Var* member = nullptr;

				auto itr = (*left->m_tableLUT).find(n->memberName);
				if(itr == std::end(*left->m_tableLUT))
				{
					member = newVariableRaw(nullptr, varType_undefined);
					(*left->m_tableLUT)[n->memberName] = member;
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
					(*result->m_tableLUT)[pair.first] = evaluate(pair.second, ctx);
				}

				return result;
			}break;
			case astNodeType_arrayMaker:
			{
				const AstArrayMaker* const n = (AstArrayMaker*)root;
				Var* result = newVariableRaw(nullptr, varType_array);
				for(const AstNode* const expr : n->arrayElements)
				{
					(*result->m_arrayValues).push_back(evaluate(expr, ctx));
				}

				return result;
			}break;
			case astNodeType_binop:
			{
				const AstBinOp* const n = (AstBinOp*)root;
				const Var* const left = evaluate(n->left, ctx);
				const Var* const right = evaluate(n->right, ctx);

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
				else if(right->m_varType == varType_string && n->op == binop_add)
				{
					// string + string
					if(left->m_varType == varType_string) {
						return newVariableString(left->m_value_string + right->m_value_string);
					}
					else if(left->m_varType == varType_f32) {
						std::stringstream ss;
						ss << left->m_value_f32;
						std::string reult = ss.str() + right->m_value_string;
						return newVariableString(reult);
					}
				}

				// Unknown operation.
				assert(false);
				return nullptr;
			}break;
			case astNodeType_unop:
			{
				const AstUnOp* const n = (AstUnOp*)root;
				const Var* const left = evaluate(n->left, ctx);

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
				Var* const left = evaluate(n->left, ctx);
				const Var* const right = evaluate(n->right, ctx);
				*left = *right;
				return left;
			}break;
			case astNodeType_fnCall:
			{
				const AstFnCall* const n = (AstFnCall*)root;

				Var* const fn = evaluate(n->theFunction, ctx);
				if(fn && fn->m_varType == varType_fn)
				{
					auto itr = parser->m_fnIdx2fn.find(fn->m_fnIdx);
					if(itr != parser->m_fnIdx2fn.end())
					{
						const AstFnDecl* const fnToCallDecl = (AstFnDecl*)itr->second;

						// Evalute the argument values.
						std::vector<Var*> argValues;
						for(int iArg = 0; iArg < n->callArgs.size(); ++iArg) {
							Var* const val =  evaluate(n->callArgs[iArg], ctx);
							argValues.push_back(val);
						}

						// Validate that the number of arguments is correct.
						if(argValues.size() != fnToCallDecl->argsNames.size()) {
							assert(false);
							return nullptr;
						}

						// Set the function arguments variable and call the function.
						pushScope(fnToCallDecl, nullptr);

						for(int iArg = 0; iArg < n->callArgs.size(); ++iArg) {
							Var* const arg = findVariableInScope(fnToCallDecl->argsNames[iArg], true, false);
							*arg = *argValues[iArg];
						}

						EvalCtx fnCtx;
						evaluate(fnToCallDecl->fnBodyBlock, fnCtx);
						Var* const result = fnCtx.forcedResult;

						popScope();

						if(result == nullptr) {
							return new Var(varType_undefined);
						}

						return result;
					}
				}
				else if(fn && fn->m_varType == varType_fnNative)
				{
					if(fn->m_fnNative != nullptr)
					{
						// Evaluate argument values.
						std::vector<Var*> arguments;
						arguments.reserve(n->callArgs.size());
						for(AstNode* argExpression : n->callArgs){
							arguments.push_back( evaluate(argExpression, ctx) );
						}

						// Perform the function call itself.
						Var* result = nullptr;
						if(fn->m_fnNative(arguments.size(), arguments.data(), this, &result)) {
							return result;
						} else {
							assert(false);
							return nullptr;
						}
					}
				}
				

				assert(false);
				return nullptr;
			}break;
			case astNodeType_arrayIndexing:
			{
				const AstArrayIndexing* const n = (AstArrayIndexing*)root;
				Var* const array = evaluate(n->theArray, ctx);
				if(array && array->m_varType == varType_array)
				{
					Var* const varIndex = evaluate(n->index, ctx);

					if(varIndex && varIndex->m_varType == varType_f32) {
						const int idx = (int)varIndex->m_value_f32;
						if(idx < 0 || idx >= (*array->m_arrayValues).size()) {
							assert(false);
							return nullptr;
						}
						return (*array->m_arrayValues)[idx];
					} else {
						assert(false);
						return nullptr;
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
					evaluate(node, ctx);
				}

				if(n->needsOwnScope) {
					popScope();
				}

				return nullptr;
			}break;
			case astNodeType_if:
			{
				const AstIf* const n = (AstIf*)root;
				const Var* const expr = evaluate(n->expression, ctx);

				if(expr->m_value_f32 != 0.f) {
					pushScope(n, "true");
					Var* const expr = evaluate(n->trueBranchStatement, ctx);
					popScope();
					return expr;
				}
				else if(n->falseBranchStatement) {
					pushScope(n, "false");
					Var* const expr = evaluate(n->falseBranchStatement, ctx);
					popScope();
					return expr;
				}

				return nullptr;
			}break;
			case astNodeType_while:
			{
				const AstWhile* const n = (AstWhile*)root;

				pushScope(n, nullptr);
				const Var* expr = evaluate(n->expression, ctx);
				while(expr->m_value_f32 != 0.f) {
					evaluate(n->trueBranchStatement, ctx);
					expr = evaluate(n->expression, ctx);
				}
				popScope();
	
				return nullptr;
			}break;
			case astNodeType_for:
			{
				const AstFor* const n = (AstFor*)root;
				pushScope(n, nullptr);
				evaluate(n->initExpression, ctx);
				const Var* expr = evaluate(n->expression, ctx);
				while(expr->m_value_f32 != 0.f) {
					evaluate(n->trueBranchStatement, ctx);
					evaluate(n->postIterationExpression, ctx);
					expr = evaluate(n->expression, ctx);
				}
				popScope();

				return nullptr;
			}break;
			case astNodeType_return:
			{
				const AstReturn* const n = (AstReturn*)root;
				if (n->expression!=nullptr) {
					ctx.forcedResult = evaluate(n->expression, ctx);
				} else {
					ctx.forcedResult = nullptr;
				}
				return ctx.forcedResult;
			}break;
			case astNodeType_print:
			{
				const AstPrint* const n = (AstPrint*)root;
				const Var* const expr = evaluate(n->expression, ctx);

				printVariable(expr);

				return nullptr;
			}break;
		}

		assert(false);
		return nullptr;
	}

private :

	void addStnadardLibFunctions() {

		NativeFnPtr const array_size = [](int argc, Var* argv[], Executor* exec, Var** ppResultVariable) -> int {
			if(argc != 1 || argv[0] == nullptr|| argv[0]->m_varType != varType_array) {
				return 0;
			}

			float fSize = 0.f;
			if(argv[0]->m_arrayValues) {
				fSize = argv[0]->m_arrayValues->size();
			}else{
				assert(false); // Should never happen.
			}

			*ppResultVariable = exec->newVariableFloat(fSize);
			return 1;
		};

		newVariableNativeFunction("array_size", array_size);
	}
	
public :

	Parser* parser = nullptr;
	std::unordered_map<std::string, Var*> m_variablesLut;
	std::vector<Var*> m_allocatedVariables;
	std::vector<std::string> m_scopeStack;
}; 

int main(int argc, const char* argv[])
{
	if(argc <= 1) {
		return 0;
	}

	std::vector<char> fileContents;
	{
		FILE* f = fopen(argv[1], "rb");
		if (f != nullptr) {
			fseek(f, 0, SEEK_END);
			const size_t fsize = ftell(f);
			fseek(f, 0, SEEK_SET);
			fileContents.resize(fsize);
			fread(fileContents.data(), 1, fsize, f);
			fclose(f);

			fileContents.push_back('\0');
		}
	}


	Lexer lexer(fileContents.data());
	std ::vector<Token> tokens;

	while(true) {
		const Token tok = lexer.getNextToken();
		tokens.push_back(tok);
	
		if(tok.type == tokenType_none) { 
			break;
		}
	}

	Parser p;
	p.m_token = tokens.data();

	p.parse();
	AstNode* root = p.root;

	Executor e;
	e.parser = &p;
	Executor::EvalCtx ctx;
	e.evaluate(root, ctx);

	system("pause");

	return 0;
}