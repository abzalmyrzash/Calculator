#include "token.h"

void DynArrToken_print(void* data, size_t len){
	for (size_t i = 0; i < len; i++) {
		Token_print((Token*)data + i);
	}
}

DynArrFunc DynArrTokenFunc = {
	DynArrToken_print,
	NULL,
	Token_free_void_ptr
};

#define NUM_OPERATIONS 23
#define NUM_SINGLE_CHAR_OPS 12
#define NUM_DOUBLE_CHAR_OPS 11

char* operations[NUM_OPERATIONS] = {
	",", "+", "-", "*", "/", "%", "^", "!", "'", ">", "<", "~"
	"++", "--", "**", "//", "%%", "==", "~=", ">=", "<=", "&&", "||",
};

char* single_char_ops = ",+-*/%^!'><~";

char* double_char_ops[NUM_DOUBLE_CHAR_OPS] = {
	"++", "--", "**", "//", "%%", "==", "~=", ">=", "<=", "&&", "||"
};

int str_is_op_len(char* str) {
	int len = 0;
	if(char_is_one_of(*str, single_char_ops)) len = 1;
	for(int i = 0; i < NUM_DOUBLE_CHAR_OPS; i++) {
		if (str_starts_with(str, double_char_ops[i])) {
			len = 2;
			break;
		}
	}
	return len;
}

#define SPECIAL_TOKENS_SIZE 13
char* special_tokens[SPECIAL_TOKENS_SIZE] = {
	"exit", "quit", "let", "calc", "solve", "matrix", "vector",
	"save", "del", "print", "help", "debug", "autoprint"
};

#define FUNCTION_TOKENS_SIZE 6
char* function_tokens[] = {
	"sqrt", "sin", "cos", "tan", "ln", "log"
};

DynArr split_into_tokens(char* command)
{
	DynArr tokens;
	DynArr_init(&tokens, sizeof(Token), 16, DynArrTokenFunc);
	Token token;
	char* tokenStart = command;
	char* tokenString;
	int tokenLen;
	TokenType tokenType;
	Token* prev = NULL;

	while(*tokenStart != '\0') {
		if (*tokenStart == ' ') {
			tokenStart++;
			continue;
		}
		int name_len = str_is_name_len(tokenStart); 
		int num_len = str_is_num_len(tokenStart);
		int op_len = str_is_op_len(tokenStart);
		bool is_equal_sign = *tokenStart == '=';
		bool is_bracket1 = *tokenStart == '(';
		bool is_bracket2 = *tokenStart == ')';
		bool is_sqrbr1 = *tokenStart == '[';
		bool is_sqrbr2 = *tokenStart == ']';

		if (name_len > 0) {
			tokenLen = name_len;
			tokenType = TOKEN_TYPE_NAME;
		}

		else if (op_len > 0) {
			tokenLen = op_len;
			tokenType = TOKEN_TYPE_OPERATION;
		}

		else if (num_len > 0) {
			tokenLen = num_len;
			tokenType = TOKEN_TYPE_NUMBER;
		}

		else if (is_equal_sign) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_EQUAL;
		}

		else if (is_bracket1) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_BRACKET1;
		}

		else if (is_bracket2) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_BRACKET2;
		}

		else if (is_sqrbr1) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_SQRBR1;
		}

		else if (is_sqrbr2) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_SQRBR2;
		}

		else {
			printf("ERROR: Invalid token!\n");
			DynArr_clear(&tokens);
			return tokens;
		}

		tokenString = str_get_null_terminated(tokenStart, tokenLen);

		if (tokenType == TOKEN_TYPE_NAME) {
			if (str_is_one_of(tokenString, special_tokens, SPECIAL_TOKENS_SIZE)) {
				tokenType = TOKEN_TYPE_SPECIAL;
			}
			else if (str_is_one_of(tokenString, function_tokens, FUNCTION_TOKENS_SIZE)) {
				tokenType = TOKEN_TYPE_FUNCTION;
			}
		}

		if (prev == NULL) goto add_token;

		check_prev_token:

		// if something like 2a is inputted, assume multiplication
		if (prev->type != TOKEN_TYPE_SPECIAL &&
	(prev->type == TOKEN_TYPE_NUMBER || prev->type == TOKEN_TYPE_NAME)
	&& (tokenType == TOKEN_TYPE_NAME || tokenType == TOKEN_TYPE_BRACKET1 ||
	tokenType == TOKEN_TYPE_FUNCTION)) {
			char* multiplyStr = malloc(2);
			strcpy(multiplyStr, "*");
			Token multiplyToken = (Token){multiplyStr, TOKEN_TYPE_OPERATION};
			DynArr_append(&tokens, &multiplyToken);
		}

		else if (prev->type == TOKEN_TYPE_NUMBER &&
			tokenType == TOKEN_TYPE_NUMBER)
		{
			int combinedLen = tokenLen + strlen(prev->str);
			char* combined = malloc(combinedLen + 1);
			combined[0] = '\0';

			strcat(combined, prev->str);
			strcat(combined, tokenString);

			free(prev->str);
			free(tokenString);

			prev->str = combined;
			int dots = 0;
			for (int i = 0; i < combinedLen; i++) {
				if (combined[i] == '.') dots++;
				if (dots == 2) {
					printf("ERROR: two dots\n");
					DynArr_clear(&tokens);
					return tokens;
				}
			}
			goto next_token;
		}

		add_token:

		prev = (Token*)DynArr_append_empty(&tokens);
		prev->str = tokenString;
		prev->type = tokenType;

		next_token:
		tokenStart += tokenLen;
	}
	return tokens;
}

bool validate_tokens(DynArr* tokens) {
	if (tokens->len == 0) return false;
	return true;
}

