#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <cstdarg>
#include <cassert>

// A location in our source code used primerly for error reporting.
struct Location
{
	Location() = default;
	Location(int column, int line)
		: column(column)
		, line(line)
	{}

	int column = -1;
	int line = -1;
};

// A structure used to describe all type of error exceptions that we use.
struct Error
{
	Error(const Location location, std::string message)
		: location(location)
		, message(std::move(message))
	{}

	Location location;
	std::string message;
};

// Set to 1 when debugging.
#if 0
	#define ThrowError(location, msg) do {assert(false); throw Error(location, msg); } while(false)
#else
	#define ThrowError(location, msg) do { throw Error(location, msg); } while(false)
#endif

// Identifies the type of the token (also know as lexeme) matched by the lexer.
// Some tokens store additional data with the token, like a float for number or
// a string for string literals and identifiers.
enum TokenType : int
{
	tokenType_endToken,
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
	tokenType_lessEquals, // <=
	tokenType_greaterEquals, // >=
	tokenType_not, // !
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

	TokenType type = tokenType_endToken;
	float numberData; // The number asociated with this token (if any).
	std::string strData; // The string asociated with this token (if any). Example usage is with identifiers or string literals.
	Location location; // The location of the token in the source file.
};

// The lexer takes the input text, and converts it to a linear set of tokens(also know as lexemes).
struct Lexer
{
	Lexer() = default;

	void getAllTokens(const char* const codeToTokenize, std::vector<Token>& resultTokens) {

		if(codeToTokenize == nullptr) {
			return;
		}

		*this = Lexer(); // Reset to default.

		m_code = codeToTokenize;
		m_ptr = m_code;

		while(true) {
			const Token tok = getNextToken();
			resultTokens.push_back(tok);

			if(tok.type == tokenType_endToken) { 
				break;
			}
		}
	}

private :

	void eatChar() {
		if(*m_ptr == '\n') {
			m_column=0;
			m_line++;
		} else {
			m_column++;
		}
		m_ptr++;
	}

	void skipSpacesAhead() {
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
			return Token(tokenType_endToken, m_column, m_line);
		}
		else if(m_ptr[0] == '/' && m_ptr[1] == '/')
		{
			eatChar(); eatChar();
			while(*m_ptr != '\0' && *m_ptr != '\n') {
				eatChar();
			}
			return getNextToken();
		}
		else if(isalpha(*m_ptr) || *m_ptr == '_')
		{
			// This should be an indentifier or a keyword.
			Token token;
			while(*m_ptr != '\0' && isalpha(*m_ptr) || *m_ptr == '_' || isdigit(*m_ptr)) {
				token.strData.push_back(*m_ptr);
				eatChar();
			}

			// Check if this is not an identifier but a keyword.
			if(token.strData == "fn") { token.type = tokenType_fn; token.strData.clear(); }
			else if(token.strData == "if")   { token.type = tokenType_if; token.strData.clear(); }
			else if(token.strData == "else") { token.type = tokenType_else; token.strData.clear(); }
			else if(token.strData == "print") { token.type = tokenType_print; token.strData.clear(); }
			else if(token.strData == "return") { token.type = tokenType_return; token.strData.clear(); } 
			else if(token.strData == "while") { token.type = tokenType_while; token.strData.clear(); }
			else if(token.strData == "for") { token.type = tokenType_for; token.strData.clear(); }
			else if(token.strData == "array") { token.type = tokenType_array; token.strData.clear(); }
			else { token.type = tokenType_identifier; }

			token.location.column = m_column;
			token.location.line = m_line;

			return token;
		}
		else if(*m_ptr == '"')
		{
			// This is a string ligeral.
			Token token;
			token.type = tokenType_string;

			eatChar();
			while(*m_ptr != '\0' && *m_ptr!='"') {
				token.strData.push_back(*m_ptr);
				eatChar();
			}
			eatChar();

			return token;
		}
		else if(m_ptr[0] == '=' && m_ptr[1] == '=') { eatChar(); eatChar(); return Token(tokenType_equals, m_column, m_line); }
		else if(m_ptr[0] == '!' && m_ptr[1] == '=') { eatChar(); eatChar(); return Token(tokenType_notEquals, m_column, m_line); }
		else if(m_ptr[0] == '<' && m_ptr[1] == '=') { eatChar(); eatChar(); return Token(tokenType_lessEquals, m_column, m_line); }
		else if(m_ptr[0] == '>' && m_ptr[1] == '=') { eatChar(); eatChar(); return Token(tokenType_greaterEquals, m_column, m_line); }
		else if(*m_ptr == '!') { eatChar(); return Token(tokenType_not, m_column, m_line); }
		else if(*m_ptr == '=') { eatChar(); return Token(tokenType_assign, m_column, m_line); }
		else if(*m_ptr == '<') { eatChar(); return Token(tokenType_less, m_column, m_line); }
		else if(*m_ptr == '>') { eatChar(); return Token(tokenType_greater, m_column, m_line); }
		else if(*m_ptr == '*') { eatChar(); return Token(tokenType_asterisk, m_column, m_line); }
		else if(*m_ptr == '/') { eatChar(); return Token(tokenType_slash, m_column, m_line); }
		else if(*m_ptr == '+') { eatChar(); return Token(tokenType_plus, m_column, m_line); }
		else if(*m_ptr == '-') { eatChar(); return Token(tokenType_minus, m_column, m_line); }
		else if(*m_ptr == '(') { eatChar(); return Token(tokenType_lparen, m_column, m_line); }
		else if(*m_ptr == ')') { eatChar(); return Token(tokenType_rparen, m_column, m_line); }
		else if(*m_ptr == '{') { eatChar(); return Token(tokenType_blockBegin, m_column, m_line); }
		else if(*m_ptr == '}') { eatChar(); return Token(tokenType_blockEnd, m_column, m_line); }
		else if(*m_ptr == '[') { eatChar(); return Token(tokenType_lsqBracket, m_column, m_line); }
		else if(*m_ptr == ']') { eatChar(); return Token(tokenType_rsqBracket, m_column, m_line); }
		else if(*m_ptr == ';') { eatChar(); return Token(tokenType_semicolon, m_column, m_line); }
		else if(*m_ptr == '.') { eatChar(); return Token(tokenType_dot, m_column, m_line); }
		else if(*m_ptr == ',') { eatChar(); return Token(tokenType_comma, m_column, m_line); }
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
					mult *= 0.1f;
					eatChar();
				}
			}

			Token token;
			token.type = tokenType_number;
			token.numberData = numberAccum;
			token.location.column = m_column;
			token.location.line = m_line;

			return token;
		}

		// Unable to recognize any token.
		ThrowError(Location(m_column, m_line), "Unable to recognize any token");
	}

	// Internal state used to perform the tokenization.
	int m_column = 0;
	int m_line = 1;

	const char* m_code = nullptr;
	const char* m_ptr = nullptr;
};

// An id for each node type of the Abstract Syntax Tree.
// Each node represens one "constriction" in the language.
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
	AstNode(AstNodeType const type, Location const location)
		: type(type)
		, location(location)
	{}
	
	virtual ~AstNode() = default;

	Location location; // the location of the expression in the code, ot at least where the expression starts.
	AstNodeType type;
};

// AstNode representing a single number literal (basically AstNode representation of the matched token by the lexer).
struct AstNumber : public AstNode
{
	AstNumber(float const value, Location location) 
		: AstNode(astNodeType_number, location)
		, value(value)
	{}

	float value;
};

// AstNode representing a single string literal (basically AstNode representation of the matched token by the lexer).
struct AstString : public AstNode
{
	AstString(std::string s, Location location) 
		: AstNode(astNodeType_string, location)
		, value(std::move(s))
	{}

	std::string value;
};

// AstNode representing a single identifer (basically AstNode representation of the matched token by the lexer).
struct AstIdentifier : public AstNode
{
	AstIdentifier(std::string identifier, Location location) 
		: AstNode(astNodeType_identifier, location)
		, identifier(identifier)
	{}

	std::string identifier;
};

// AstNode representing the operation of acessing a member variable in a table(known as dictionaly or map in some languages).
// Exmple: <expression>.<member> like point.x
struct AstMemberAcess : public AstNode
{
	AstMemberAcess(AstNode* const left, const std::string& memberName, Location location)
		: left(left)
		, memberName(memberName)
		, AstNode(astNodeType_memberAccess, location)
	{}

	AstNode* left = nullptr;
	std::string memberName;
};

// AstNode representing a set of nodes used to create a table.
// Example: { x = 5; y = 10; }
struct AstTableMaker : public AstNode
{
	AstTableMaker(Location location)
		: AstNode(astNodeType_tableMaker, location)
	{}

	std::unordered_map<std::string, AstNode*> memberToExpression;
};

// AstNode representing a set of nodes used to create an array.
// Example: array{ 5, 10, 15, 20 }
struct AstArrayMaker : public AstNode
{
	AstArrayMaker(Location location)
		: AstNode(astNodeType_arrayMaker, location)
	{}

	std::vector<AstNode*> arrayElements;
};

// AstNode representing a function call.
// The function is obtained by evaluating the expresion on the left of the '('
// everything else, until the matching ')' is concidered a function argument.
struct AstFnCall : public AstNode
{
	AstFnCall(Location location)
		: AstNode(astNodeType_fnCall, location)
	{}

	AstNode* theFunction = nullptr; // The node that we are going to evaluate to obtain the function that we're going to call.
	std::vector<AstNode*> callArgs; // The node that we are going to evalute in order to pass the parameters to the function.
};

// AstNode representing an array indexing.
// The array is obtained by evaluating the expresion on the left of the '['
// everything else, until the matching ']' is going to be used as index.
struct AstArrayIndexing : public AstNode
{
	AstArrayIndexing(Location location)
		: AstNode(astNodeType_arrayIndexing, location)
	{}

	AstNode* theArray = nullptr; // The node that we are going to evaluate to obtain the array variable.
	AstNode* index = nullptr; // The node that we are going to evaluate to obtain the index.
};

// A binary operation line  x + y, x * y and so on.
struct AstBinOp : public AstNode
{
	AstBinOp(const TokenType op, AstNode* const left, AstNode* const right, Location location) 
		: AstNode(astNodeType_binop, location)
		, op(op)
		, left(left)
		, right(right)
	{}

	AstNode* left;
	AstNode* right;
	TokenType op; // The token type of the operation.
};

// Unary operation like !x, -x, +x,
struct AstUnOp : public AstNode
{
	AstUnOp(TokenType op, AstNode* left, Location location) 
		: AstNode(astNodeType_unop, location)
		, op(op)
		, left(left)
	{}

	AstNode* left = nullptr;
	TokenType op; // The type of the token for the speciified operation. ! + - are allowed.
};

struct AstAssign : public AstNode
{
	AstAssign(AstNode* left, AstNode* right, Location location) 
		: AstNode(astNodeType_assign, location)
		, left(left)
		, right(right)
	{}

	AstNode* left = nullptr;
	AstNode* right = nullptr;
};

struct AstStatementList : public AstNode
{
	AstStatementList(Location location) :
		AstNode(astNodeType_statementList, location)
	{}

	bool needsOwnScope = true; // if specified when executing a new scope will be generated for the statement list.
	std::vector<AstNode*> m_statements;
};

struct AstIf : public AstNode
{
	AstIf(Location location) :
		AstNode(astNodeType_if, location)
	{}

	AstNode* expression = nullptr;
	AstNode* trueBranchStatement = nullptr;
	AstNode* falseBranchStatement = nullptr;
};

struct AstFnDecl : public AstNode
{
	AstFnDecl(Location location) :
		AstNode(astNodeType_fndecl, location)
	{}

	AstNode* fnBodyBlock = nullptr; // THe code of the function.
	std::vector<std::string> argsNames;
	int fnIdx = -1;
};

struct AstWhile : public AstNode
{
	AstWhile(Location location) :
		AstNode(astNodeType_while, location)
	{}

	AstNode* expression = nullptr;
	AstNode* trueBranchStatement = nullptr;
};

struct AstFor : public AstNode
{
	AstFor(Location location) :
		AstNode(astNodeType_for, location)
	{}

	AstNode* initExpression = nullptr;
	AstNode* expression = nullptr;
	AstNode* postIterationExpression = nullptr;
	AstNode* trueBranchStatement = nullptr;
};

struct AstPrint : public AstNode
{
	AstPrint(AstNode* expression, Location location)
		: AstNode(astNodeType_print, location)
		, expression(expression)
	{}

	AstNode* expression;
};

struct AstReturn : public AstNode
{
	AstReturn(Location location) :
		AstNode(astNodeType_return, location),
		expression(nullptr)
	{}

	AstNode* expression;
};

// The parser itself.
// Takes a list of tokens and produces an AST.
struct Parser
{
	const Token* m_token = nullptr;
	std::unordered_map<int, AstFnDecl*> m_fnIdx2fn;

	// Registers the specified AstFnDecl, and gives the function specified by it a unique id(in that case just an index in a Look-Up-Table).
	// This id is used to identify the function and to perform function calls.
	void registerFunction(AstFnDecl* const fnDecl) {
		fnDecl->fnIdx = m_fnIdx2fn.size();
		m_fnIdx2fn[m_fnIdx2fn.size()] = fnDecl;
	}

	// Parses the specified list of token stored in m_token array (assums that the last token is tokenType_endToken).
	AstNode* parse() {
		return parse_programRoot();
	}

	// A block of statements or a single statement.
	AstNode* parse_statement_block() {
		if(m_token->type == tokenType_blockBegin) {
			AstStatementList* const stmntList = new AstStatementList(match(tokenType_blockBegin)->location);
			while(m_token->type != tokenType_blockEnd)
			{
				AstNode* node = parse_statement();
				if(node) {
					stmntList->m_statements.push_back(node);
				} else {
					ThrowError(m_token->location, "Failed to parse a statement");
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
			const Token* const printToken = match(tokenType_print);
			AstPrint* const astPrint = new AstPrint(parse_expression(), printToken->location);
			match(tokenType_semicolon);
			return astPrint;
		}
		else if(m_token->type == tokenType_if)
		{
			// Add if as a statement so we don't have to add a semicolon after it, when we don't use it in an expression.
			AstNode* ifNode = parse_expression_if();

			if(ifNode == nullptr) {
				ThrowError(m_token->location, "Failed to parse if expression");
				return nullptr;
			}

			return ifNode;
		}
		else if(m_token->type == tokenType_while)
		{
			AstWhile* const astWhile = new AstWhile(match(tokenType_while)->location);
			astWhile->expression = parse_expression();
			astWhile->trueBranchStatement = parse_statement_block();

			return astWhile;
		}
		else if(m_token->type == tokenType_for)
		{
			AstFor* const astFor = new AstFor(match(tokenType_for)->location);

			astFor->initExpression = parse_expression();
			match(tokenType_semicolon);
			
			astFor->expression = parse_expression();
			match(tokenType_semicolon);

			astFor->postIterationExpression = parse_expression();

			astFor->trueBranchStatement = parse_statement_block();

			return astFor;
		}
		else if(m_token->type == tokenType_return)
		{
			AstReturn* const astReturn = new AstReturn(match(tokenType_return)->location);

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

		ThrowError(m_token->location, "Failed to parse single statement");
		return nullptr;
	}

	AstNode* parse_programRoot()
	{
		AstStatementList* progRoot = new AstStatementList(Location(0,0)); // TODO: proper location of the 1st statement.
		progRoot->needsOwnScope = false;
		while(m_token->type != tokenType_endToken) {
			AstNode* node = parse_statement();
			if(node) {
				progRoot->m_statements.push_back(node);
			}
			else {
				ThrowError(Location(), "Unknown error while parsing the program");
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
			AstIf* const astIf = new AstIf(match(tokenType_if)->location);
			astIf->expression = parse_expression();
			if(astIf->expression == nullptr) {
				ThrowError(m_token->location, "Failed to parse if condition expression");
				return nullptr;
			}

			astIf->trueBranchStatement = parse_statement_block();

			if(m_token->type == tokenType_else) {
				match(tokenType_else);
				astIf->falseBranchStatement = parse_statement_block();
			}

			return astIf;
		}

		ThrowError(m_token->location, "Expected if token");
		return nullptr;
	}

	AstNode* parse_expression_fndecl()
	{
		AstFnDecl* fnDecl = nullptr;

		if(m_token->type == tokenType_fn) {
			fnDecl = new AstFnDecl(match(tokenType_fn)->location);
			registerFunction(fnDecl);

			match(tokenType_lparen);
			while(m_token->type == tokenType_identifier) {
				fnDecl->argsNames.push_back(m_token->strData);
				match(tokenType_identifier);

				if(m_token->type == tokenType_comma) {
					match(tokenType_comma);
				}
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
		AstTableMaker* const result = new AstTableMaker(match(tokenType_blockBegin)->location);

		while(m_token->type != tokenType_blockEnd)
		{
			// { identifer = expression; ... }
			if(m_token->type == tokenType_identifier) {
				AstNode*& memberInitExpr = result->memberToExpression[m_token->strData];
				match(tokenType_identifier);
				match(tokenType_assign);
				memberInitExpr = parse_expression();

				if(!memberInitExpr) {
					ThrowError(m_token->location, "Failed to parse for loop init expression");
					return nullptr;
				}	

				match(tokenType_semicolon);
			} else {
				ThrowError(m_token->location, "Expected an identifier for member initialization when creating a table");
				return nullptr;
			}
		}
		match(tokenType_blockEnd);

		return result;
	}

	AstNode* parse_expression_arrayMaker()
	{
		const Token* const tokenArray = match(tokenType_array);
		match(tokenType_blockBegin);

		AstArrayMaker* result = new AstArrayMaker(tokenArray->location);

		while(m_token->type != tokenType_blockEnd) {
			result->arrayElements.push_back(parse_expression());

			if(m_token->type == tokenType_comma) {
				match(tokenType_comma);
			} 
		}
		match(tokenType_blockEnd);

		return result;
	}

	AstNode* parse_expression0()
	{
		AstNode* left = nullptr;
		if(m_token->type == tokenType_number)
		{
			left = new AstNumber(m_token->numberData, m_token->location);
			match(tokenType_number);
		}
		else if(m_token->type == tokenType_string)
		{
			left = new AstString(m_token->strData, m_token->location);
			match(tokenType_string);
		}
		else if(m_token->type == tokenType_identifier)
		{
			left = new AstIdentifier(m_token->strData, m_token->location);
			match(tokenType_identifier);
		}
		else if(m_token->type == tokenType_lparen)
		{
			match(tokenType_lparen);
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

		if(left == nullptr) {
			ThrowError(m_token->location, "Unknown expression");
			return nullptr;
		}

		// In Addition, this thing could be a function call for an array indexing.
		while(m_token->type == tokenType_lparen || m_token->type == tokenType_lsqBracket || m_token->type == tokenType_dot)
		{
			if(m_token->type == tokenType_lparen)
			{
				AstFnCall* fnCall = new AstFnCall(match(tokenType_lparen)->location);
				fnCall->theFunction = left;

				while(m_token->type != tokenType_rparen)
				{
					// Gather the function call arguments.
					AstNode* const arg = parse_expression();
					if(arg) {
						fnCall->callArgs.push_back(arg);
					} else {
						ThrowError(m_token->location, "Failed to parse function call argument");
						return nullptr;
					}

					if(m_token->type == tokenType_comma) {
						match(tokenType_comma);
					}
				}
				match(tokenType_rparen);

				left = fnCall;
			}
			else if(m_token->type == tokenType_lsqBracket)
			{
				AstArrayIndexing* arrayIndexing = new AstArrayIndexing(match(tokenType_lsqBracket)->location);
				arrayIndexing->theArray = left;
				arrayIndexing->index = parse_expression();

				match(tokenType_rsqBracket);

				left = arrayIndexing;
			}
			else if(m_token->type == tokenType_dot)
			{
				match(tokenType_dot);
				assert(m_token->strData.size() > 0);
				AstMemberAcess* const memberAcess = new AstMemberAcess(left, m_token->strData, m_token->location);
				match(tokenType_identifier);
				left = memberAcess;
			}
		}
	
		return left;
	}

	AstNode* parse_expression1()
	{
		const Location tokenLoc = m_token->location;

		if(m_token->type == tokenType_minus)
		{
			match(tokenType_minus);
			AstUnOp* const result =  new AstUnOp(tokenType_minus, parse_expression0(), tokenLoc);
			return result;
		}
		else if(m_token->type == tokenType_plus)
		{
			match(tokenType_plus);
			AstUnOp* const result =  new AstUnOp(tokenType_plus, parse_expression0(), tokenLoc);
			return result;
		}
		else if(m_token->type == tokenType_not)
		{
			match(tokenType_not);
			AstUnOp* const result =  new AstUnOp(tokenType_not, parse_expression0(), tokenLoc);
			return result;
		}

		return parse_expression0();
	}

	AstNode* parse_expression2()
	{
		AstNode* const left = parse_expression1();

		const Location tokenLoc = m_token->location;
		if(m_token->type == tokenType_asterisk)
		{
			match(tokenType_asterisk);
			AstNode* retval = new AstBinOp(tokenType_asterisk, left, parse_expression2(), tokenLoc);
			return retval;
		}
		else if(m_token->type == tokenType_slash)
		{
			match(tokenType_slash);
			AstNode* retval = new AstBinOp(tokenType_slash, left, parse_expression2(), tokenLoc);
			return retval;
		}

		return left;
	}

	AstNode* parse_expression3()
	{
		AstNode* left = parse_expression2();

		const Location tokenLoc = m_token->location;
		if(m_token->type == tokenType_plus)
		{
			match(tokenType_plus);
			AstNode* retval = new AstBinOp(tokenType_plus, left, parse_expression(), tokenLoc);
			return retval;
		}
		else if(m_token->type == tokenType_minus)
		{
			match(tokenType_minus);
			AstNode* retval = new AstBinOp(tokenType_minus, left, parse_expression(), tokenLoc);
			return retval;
		}

		return left;
	}

	AstNode* parse_expression4()
	{
		AstNode* const left = parse_expression3();

		const Location tokenLoc = m_token->location;
		if(m_token->type == tokenType_equals)
		{
			match(tokenType_equals);
			AstBinOp* equals = new AstBinOp(tokenType_equals, left, parse_expression(), tokenLoc);
			return equals;
		} 
		else if(m_token->type == tokenType_notEquals)
		{
			match(tokenType_notEquals);
			AstBinOp* equals = new AstBinOp(tokenType_notEquals, left, parse_expression(), tokenLoc);
			return equals;
		}
		else if(m_token->type == tokenType_lessEquals)
		{
			match(tokenType_lessEquals);
			AstBinOp* equals = new AstBinOp(tokenType_lessEquals, left, parse_expression(), tokenLoc);
			return equals;
		}
		else if(m_token->type == tokenType_greaterEquals)
		{
			match(tokenType_greaterEquals);
			AstBinOp* equals = new AstBinOp(tokenType_greaterEquals, left, parse_expression(), tokenLoc);
			return equals;
		}

		return left;
	}

	AstNode* parse_expression5()
	{
		AstNode* const left = parse_expression4();

		const Location tokenLoc = m_token->location;
		if(m_token->type == tokenType_less)
		{
			match(tokenType_less);
			AstBinOp* equals = new AstBinOp(tokenType_less, left, parse_expression(), tokenLoc);
			return equals;
		}
		else if(m_token->type == tokenType_greater)
		{
			match(tokenType_greater);
			AstBinOp* equals = new AstBinOp(tokenType_greater, left, parse_expression(), tokenLoc);
			return equals;
		}

		return left;
	}

	AstNode* parse_expression6()
	{
		AstNode* const left = parse_expression5();

		const Location tokenLoc = m_token->location;
		if(m_token->type == tokenType_assign)
		{
			match(tokenType_assign);
			AstAssign* assign = new AstAssign(left, parse_expression(), tokenLoc);
			return assign;
		}

		return left;	
	}

	const Token* match(TokenType const type) {
		if(m_token->type != type) {
			// TODO: a better message based on the expected token .
			ThrowError(m_token->location, "Unexpected token");
			return nullptr;
		}
		return m_token++;
	}
};

//-----------------------------------------------------------------------------------------------------
// The Executor (also know as Interpreter or Virtual Machine) and all the data that is needed to
// execute our script.
//-----------------------------------------------------------------------------------------------------

// An enum describing all posible variable types that out language could handle.
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

// A common typedef used for interop between the scrpting language and C++.
// Example usages are: array_size array_push array_pop functions in the language.
struct Var;
struct Executor;
typedef int (*NativeFnPtr)(int argc, Var* argv[], Executor* exec, Var** ppResultVariable);

// The strcture that represents a single value(and variable) while executing the script.
struct Var
{
	Var(VarType const varType = varType_undefined)
		: m_varType(varType)
	{
		if(varType == varType_table) {
			m_tableLUT = std::make_shared<std::unordered_map<std::string, Var>>();
		}

		if(varType == varType_array) {
			m_arrayValues = std::make_shared<std::vector<Var>>();
		}
	}

	// TODO: These are kind of redundant and can be removed with a bit of work.
	void makeFloat32(const float value) {
		*this = Var(varType_f32);
		m_value_f32 = value;
	}

	void makeString(std::string s) {
		*this = Var(varType_string);
		m_value_string = std::move(s);
	}

	void makeTable() {
		*this = Var(varType_table);
	}

	void makeArray() {
		*this = Var(varType_array);
	}

	void makeFunction(int functionIndex) {
		*this = Var(varType_fn);
		m_fnIdx = functionIndex;
	}

	void makeNativeFunction(NativeFnPtr const nativeFn) {
		*this = Var(varType_fnNative);
		m_fnNative = nativeFn;
	}

public :

	VarType m_varType = varType_undefined;

	// The data that could be used depending on the type of the variable:
	float m_value_f32 = 0.f; // A float representing a number in our language.
	int m_fnIdx = -1;  // An int containing the function id of the function that we point to (see registerFunction).
	NativeFnPtr m_fnNative = nullptr; // Used to enable our script to call native C++ functions via that function-pointer typedef.
	std::string m_value_string; // A std::string for strings in our language.

	// These two are shared_ptrs in order to replicate the bahiavior that is used in JavaScript.
	// When we pass these around we pass them by reference (all other types are by value).
	std::shared_ptr<std::unordered_map<std::string, Var>> m_tableLUT; // If this variable is a table, holds names and values of all of its members.
	std::shared_ptr<std::vector<Var>> m_arrayValues; // If this is an array, hold the member values for each index.
};

// Just a function that prints the type and value of the specified variable to std::out.
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
			printVariable(&pair.second);
			
		}
		printf(" }\n");
	}
	else if(expr->m_varType == varType_array)
	{
		printf("[ \n");
		if(expr->m_arrayValues)
			for(const Var& var : *expr->m_arrayValues)
			{
				printVariable(&var);
			}
		printf(" ]\n");
	}
	else
		printf("<undefined>\n");
};

// Represents a 'scope' in our language. Each function or a block create it's own scope
// in order to enable us to have colliding variable names.
// Even if the variables are in different functions we still need this scope.
// In our case, depending on the current block, we just amend all scopes (the currenct scope is stored in an array).,
// at the begining of the variable name - this is name collisions are resolved.
struct Scope
{
	std::string scope;
};

// The executor itself.
// Takes and AST node and executes.
struct Executor
{
	Executor() {
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
		if(m_scopeStack.empty()) {
			ThrowError(Location(), "Internal Error: Wrong scope pop");
		}
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
					ThrowError(n->location, "Only tables have members");
					return nullptr;
				}

				Var member;

				auto itr = (*left->m_tableLUT).find(n->memberName);
				if(itr == std::end(*left->m_tableLUT))
				{
					(*left->m_tableLUT)[n->memberName] = Var();
					return &(*left->m_tableLUT)[n->memberName];
				}
				else
				{
					return &itr->second;
				}
				
			}break;
			case astNodeType_tableMaker:
			{
				const AstTableMaker* const n = (AstTableMaker*)root;
				Var* result = newVariableRaw(nullptr, varType_table);
				for(const auto& pair : n->memberToExpression)
				{
					(*result->m_tableLUT)[pair.first] = *evaluate(pair.second, ctx);
				}

				return result;
			}break;
			case astNodeType_arrayMaker:
			{
				const AstArrayMaker* const n = (AstArrayMaker*)root;
				Var* result = newVariableRaw(nullptr, varType_array);
				for(const AstNode* const expr : n->arrayElements)
				{
					 ;
					(*result->m_arrayValues).push_back(*evaluate(expr, ctx));
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
					if(n->op == tokenType_plus) return newVariableFloat(left->m_value_f32 + right->m_value_f32);
					else if(n->op == tokenType_minus) return newVariableFloat(left->m_value_f32 - right->m_value_f32);
					else if(n->op == tokenType_asterisk) return newVariableFloat(left->m_value_f32 * right->m_value_f32);
					else if(n->op == tokenType_slash) return newVariableFloat(left->m_value_f32 / right->m_value_f32);
					else if(n->op == tokenType_equals) return newVariableFloat(left->m_value_f32 == right->m_value_f32);
					else if(n->op == tokenType_notEquals) return newVariableFloat(left->m_value_f32 != right->m_value_f32);
					else if(n->op == tokenType_lessEquals) return newVariableFloat(left->m_value_f32 <= right->m_value_f32);
					else if(n->op == tokenType_greaterEquals) return newVariableFloat(left->m_value_f32 >= right->m_value_f32);
					else if(n->op == tokenType_less) return newVariableFloat(left->m_value_f32 < right->m_value_f32);
					else if(n->op == tokenType_greater) return newVariableFloat(left->m_value_f32 > right->m_value_f32);
				}

				if(left->m_varType == varType_string && right->m_varType == varType_string)
				{
					if(n->op == tokenType_equals) return newVariableFloat(left->m_value_string == right->m_value_string);
				}

				if(left->m_varType == varType_string && n->op == tokenType_plus)
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
				else if(right->m_varType == varType_string && n->op == tokenType_plus)
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
				ThrowError(n->location, "Uknown/Unimplemented binary operation");
			}break;
			case astNodeType_unop:
			{
				const AstUnOp* const n = (AstUnOp*)root;
				const Var* const left = evaluate(n->left, ctx);

				if(left->m_varType != varType_f32) {
					ThrowError(n->location, "Expected a number variable");
				}

				float v = 0.f;
				if(n->op == tokenType_minus) v = -left->m_value_f32;
				else if(n->op == tokenType_plus) v = left->m_value_f32;
				else if(n->op == tokenType_not) v = left->m_value_f32 ? 0.f : 1.f;
				else ThrowError(n->location, "Unknown unary operation!");

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
							ThrowError(n->location, "Wrong number of arguments specified to a function call");
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
							ThrowError(n->location, "Failed on native function call");
							return nullptr;
						}
					}
				}
				
				ThrowError(n->location, "Uknown function call");
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
							ThrowError(n->location, "Out of bounds array indexing");
							return nullptr;
						}
						return &(*array->m_arrayValues)[idx];
					} else {
						ThrowError(n->location, "Array index must be a number");
						return nullptr;
					}
				}

				ThrowError(n->location, "Only arrays can be indexed");
				return nullptr;
			}break;
			case astNodeType_statementList:
			{
				const AstStatementList* const n = (AstStatementList*)root;

				if(n->needsOwnScope) {
					pushScope(n, nullptr);
				}

				Var* result = nullptr;
				for(AstNode* node : n->m_statements) {
					result = evaluate(node, ctx);
				}

				if(n->needsOwnScope) {
					popScope();
				}

				return result;
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

		ThrowError(root->location, "Uknown AST operation");
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
				ThrowError(Location(), "Internal Error: Uninitialized array");
			}

			*ppResultVariable = exec->newVariableFloat(fSize);
			return 1;
		};

		newVariableNativeFunction("array_size", array_size);
	
		NativeFnPtr const array_pop = [](int argc, Var* argv[], Executor* exec, Var** ppResultVariable) -> int {
			if(argv[0] == nullptr|| argv[0]->m_varType != varType_array)
				return 0;
			
			if(argc == 1)
			{

				if(argv[0]->m_arrayValues) {
					if(argv[0]->m_arrayValues->empty() == false) {
						argv[0]->m_arrayValues->pop_back();
					}
				}else{
					ThrowError(Location(), "Internal Error: Uninitialized array");
				}
			}
			if(argc == 2)
			{
				if(argv[0]->m_arrayValues) {
					if(argv[0]->m_arrayValues->empty() == false) {
						argv[0]->m_arrayValues->erase(argv[0]->m_arrayValues->begin() + (int)argv[1]->m_value_f32);
					}
				}else{
					ThrowError(Location(), "Internal Error: Uninitialized array");
				}
			}

			return 1;
		};
	
		newVariableNativeFunction("array_pop", array_pop);

		NativeFnPtr const array_push = [](int argc, Var* argv[], Executor* exec, Var** ppResultVariable) -> int {
			if(argc != 2 || argv[0] == nullptr|| argv[0]->m_varType != varType_array) {
				return 0;
			}

			if(argv[0]->m_arrayValues) {
				argv[0]->m_arrayValues->push_back(*argv[1]);
			}else{
				ThrowError(Location(), "Internal Error: Uninitialized array");
			}

			return 1;
		};

		newVariableNativeFunction("array_push", array_push);
	}
	
public :

	// TODO: Currently "Parser* parser" used only for m_fnIdx2fn, with a tiny bit of work this dependancy could be removed.
	// This currently prevents us form injecting more code in our enviornment.
	Parser* parser = nullptr;
	std::unordered_map<std::string, Var*> m_variablesLut;
	std::vector<Var*> m_allocatedVariables;
	std::vector<std::string> m_scopeStack;
}; 

///
///
///
int main(int argc, const char* argv[])
{
	if(argc <= 1) {
		return 0;
	}

	// Read the contents of the specified file.
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

	try 
	{
		// Generate the token list for the parser.
		std ::vector<Token> tokens;
		Lexer lexer;
		lexer.getAllTokens(fileContents.data(), tokens);

		// Update the list of tokens that are going to be used for parsing and well... parse them.
		Parser p;
		p.m_token = tokens.data();
		AstNode* nodeToExecute = p.parse();

		// Evaluate the produced AST.
		Executor e;
		e.parser = &p;
		Executor::EvalCtx ctx;
		e.evaluate(nodeToExecute, ctx);

		const int done = 0;
	}
	catch(Error& e)
	{
		printf("Error at %d, %d:\n\t%s", e.location.line, e.location.column, e.message.c_str());
	}
	catch(...)
	{
		printf("Unknown error");
	}

	system("pause");

	return 0;
}