#include <iostream>
#include <string>
#include <assert.h>

enum TokenType
{
	tokenType_none,
	tokenType_number,
	tokenType_identifier,

	//
	tokenType_semicolon, // ;
	tokenType_assign, // =
	tokenType_plus, // +
	tokenType_minus, // -
	tokenType_asterisk, // *
	tokenType_slash, // /
	tokenType_lparen, // (
	tokenType_rparen, // )
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

bool isSign(char ch)
{
	return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

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
			Token token;

			// This should be an indentifier.
			token.type = tokenType_identifier;
			while(isalpha(*m_ptr) || *m_ptr == '_'|| isdigit(*m_ptr))
			{
				token.strData.push_back(*m_ptr);
				m_ptr++;
			}

			return token;
		}
		else if(*m_ptr == ';') {
			m_ptr++;
			return Token(tokenType_semicolon);
		}
		else if(*m_ptr == '=') {
			m_ptr++;
			return Token(tokenType_assign);
		}
		else if(*m_ptr == '*') {
			m_ptr++;
			return Token(tokenType_asterisk);
		}
		else if(*m_ptr == '/') {
			m_ptr++;
			return Token(tokenType_slash);
		}
		else if(*m_ptr == '(') {
			m_ptr++;
			return Token(tokenType_lparen);
		}
		else if(*m_ptr == ')') {
			m_ptr++;
			return Token(tokenType_rparen);
		}
		else if(*m_ptr == '+' || *m_ptr == '-' || isdigit(*m_ptr))
		{
			float numberSign = 1.f;

			// This could be a number or just a sign. To check that we need to look one symbol ahead.
			if(*m_ptr == '+' || *m_ptr == '-')
			{
				if(isdigit(*(m_ptr+1)) == false)
				{
					// This is not a number, it must be just a sign.
					if(*m_ptr == '+') { 
						m_ptr++;
						return Token(tokenType_plus);
					}
					else if(*m_ptr == '-') {
						m_ptr++;
						return Token(tokenType_minus);
					}
					else 
					{
						// Should never happen.
						assert(false);
						return Token();
					}
				}
				else
				{
					if(*m_ptr == '-') numberSign = -1.f;
					m_ptr++;
				}
			}

			// This should be a number.
			float numberAccum = 0;
			while(isdigit(*m_ptr)){
				int digit = *m_ptr - '0';
				numberAccum = numberAccum*10.f + (float)digit;
				m_ptr++;
			}

			Token token;
			token.type = tokenType_number;
			token.numberData = numberAccum * numberSign;

			return token;
		}

		// Should never happen.
		assert(false);
		return Token();
	}

	const char* m_code = nullptr;
	const char* m_ptr = nullptr;
};


int main()
{
	const char* const testCode = R"(
ident0 = 334;
ident0 = -1*ident1 + ident2;
ident3=-500*+1/3*-3;
)";

	Lexer lexer(testCode);

	while(true)
	{
		const Token tok = lexer.getNextToken();
	
		if(tok.type == tokenType_number) printf("NUMBER %f;\n", tok.numberData);
		if(tok.type == tokenType_identifier) printf("IDENT %s;\n", tok.strData.c_str());
		if(tok.type == tokenType_semicolon) printf("; \n");
		if(tok.type == tokenType_assign) printf("= \n");
		if(tok.type == tokenType_plus) printf("+ \n");
		if(tok.type == tokenType_minus) printf("- \n");
		if(tok.type == tokenType_asterisk) printf("* \n");
		if(tok.type == tokenType_slash) printf("/ \n");
		if(tok.type == tokenType_none) { printf("NONE;\n "); break; }
	}


	return 0;
}