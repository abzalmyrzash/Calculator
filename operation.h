#pragma once
#include <math.h>
#include "globals.h"
#include "token.h"

Variable* Operation_calculate(Variable* a, Variable* b, char* op) {
	if (a == NULL && b == NULL) {
		return NULL;
	}

	if (a->type == VAR_TYPE_NUMBER && b->type == VAR_TYPE_NUMBER) {
		double* res = malloc(sizeof(double));
		double a_val = *(double*)a->data;
		double b_val = *(double*)b->data;
		if (strsame(op, "+")) {
			*res = a_val + b_val;
		}
		else if (strsame(op, "-")) {
			*res = a_val - b_val;
		}
		else if (strsame(op, "*")) {
			*res = a_val * b_val;
		}
		else if (strsame(op, "/")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return NULL;
			}
			*res = a_val / b_val;
		}

		else if (strsame(op, "//")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return NULL;
			}
			*res = floor(a_val / b_val);
		}
		else if (strsame(op, "%%")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return NULL;
			}
			*res = a_val - b_val * floor(a_val / b_val);
		}
		else if (strsame(op, "^")) {
			*res = pow(a_val, b_val);
		}
		else {
			printf("ERROR: Operation %s is invalid for two numbers!\n", op);
			return NULL;
		}
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	else if (a->type == VAR_TYPE_NUMBER && b->type == VAR_TYPE_MATRIX) {
		Matrix* res;
		if (strsame(op, "*")) {
			res = Matrix_scale(b->data, *(double*)a->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a number and matrix!\n", op);
			return NULL;
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}

	else if (a->type == VAR_TYPE_MATRIX && b->type == VAR_TYPE_NUMBER) {
		Matrix* res;
		if (strsame(op, "*")) {
			res = Matrix_scale(a->data, *(double*)b->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a matrix and number!\n", op);
			return NULL;
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}
	else if (a->type == VAR_TYPE_MATRIX && b->type == VAR_TYPE_MATRIX) {
		Matrix* res;
		if (strsame(op, "+")) {
			res = Matrix_add(a->data, b->data);
		}
		else if (strsame(op, "-")) {
			res = Matrix_subtract(a->data, b->data);
		}
		else if (strsame(op, "*")) {
			res = Matrix_multiply(a->data, b->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for two matrices!\n", op);
			return NULL;
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}

}

typedef enum {
	OPERATION_PRIORITY_OR,
	OPERATION_PRIORITY_AND,
	OPERATION_PRIORITY_EQORNOT,
	OPERATION_PRIORITY_COMPAR,
	OPERATION_PRIORITY_ADDSUB,
	OPERATION_PRIORITY_MULTDIV,
	OPERATION_PRIORITY_EXPONENT,
	OPERATION_PRIORITY_UNARY
} OperationPriority;

char* addSubList[] = {"+", "-"};
char* multDivList[] = {"*", "/", "//", "%%"};
char* expList[] = {"^"};
char* unaryList[] = {"%", "!", "'", "~"};
char* orList[] = {"||"};
char* andList[] = {"&&"};
char* eqOrNotList[] = {"==", "~="};
char* comparList[] = {">", "<", ">=", "<="};

OperationPriority OperationPriority_getPriority(char* op) {
	if (str_is_one_of(op, addSubList, 2)) {
		return OPERATION_PRIORITY_ADDSUB;
	}
	if (str_is_one_of(op, multDivList, 4)) {
		return OPERATION_PRIORITY_MULTDIV;
	}
	if (str_is_one_of(op, expList, 1)) {
		return OPERATION_PRIORITY_EXPONENT;
	}
	if (str_is_one_of(op, unaryList, 4)) {
		return OPERATION_PRIORITY_UNARY;
	}
	if (str_is_one_of(op, comparList, 4)) {
		return OPERATION_PRIORITY_COMPAR;
	}
	if (str_is_one_of(op, eqOrNotList, 2)) {
		return OPERATION_PRIORITY_EQORNOT;
	}
	if (str_is_one_of(op, andList, 1)) {
		return OPERATION_PRIORITY_AND;
	}
	if (str_is_one_of(op, orList, 1)) {
		return OPERATION_PRIORITY_OR;
	}
	return -1;
}

struct TreeNode {
	struct TreeNode* left;
	struct TreeNode* right;
	Token* expr;
	int exprLen;
	Variable* value;
};

struct TreeNode* _TreeNode_new(Token* token, int exprLen) {
	struct TreeNode* node = malloc(sizeof(struct TreeNode));
	if(token == NULL) {
		node->expr = NULL;
		node->exprLen = 0;
	}
	else {
		node->expr = token;
		node->exprLen = exprLen;
	}
	node->left = NULL;
	node->right = NULL;
	node->value = NULL;
	return node;
}

void _TreeNode_split(struct TreeNode* node) {
	Token* chosenOperation = NULL; // the operation token where the split happens
	// chosen operation must have minimum bracketLevel and priority
	OperationPriority minPriority;
	// bracket level is how many bracket pairs we are inside of
	int bracketLevel = 0;
	int minBracketLevel;
	for (int i = node->exprLen-1; i >= 0; i--) {
		Token *token = node->expr + i;
		switch (token->type) {
	
		case TOKEN_TYPE_BRACKET2:
			bracketLevel++;
			break;
		case TOKEN_TYPE_BRACKET1:
			bracketLevel--;
			break;
		case TOKEN_TYPE_OPERATION:
			OperationPriority priority = OperationPriority_getPriority(token->str);
			if (chosenOperation == NULL ||
					(priority < minPriority && bracketLevel == minBracketLevel)
					||  bracketLevel < minBracketLevel ||
			(strsame(chosenOperation->str, "^") && strsame(token->str, "^")
				&& bracketLevel == minBracketLevel)) {
				chosenOperation = token;
				minPriority = priority;
				minBracketLevel = bracketLevel;
			}
			break;
		default:
			break;
		}
	}
	// if operation not found, set the expression to the number or the variable
	if (chosenOperation == NULL) {
		for (int i = 0; i < node->exprLen; i++) {
			Token *token = node->expr + i;
			switch(token->type) {

			case TOKEN_TYPE_NUMBER:
				node->expr = token;
				node->exprLen = 1;
				char* endptr;
				double* value = malloc(sizeof(double));
				*value = strtod(token->str, &endptr);
				//printf("%lf\n", *value);
				node->value = Variable_new(VAR_TYPE_NUMBER, NULL, value);
				//Variable_print(node->value);
				break;
			case TOKEN_TYPE_NAME:
				node->expr = token;
				node->exprLen = 1;
				Variable* var = HashMap_search(hashmap, token->str);
				node->value = Variable_copy(var);
				break;
			default:
				break;
			}
		}
		if (node->value == NULL) {
			printf("ERROR: failed to assign value to tree leaf!\n");
			return;
		}
		return;
	}
	// if operation was found, split expression into left and right children
	// with operation in the middle as their parent
	//printf("Chosen operation: %s\n", chosenOperation->str);
	Token* leftToken = node->expr;
	int leftLen = chosenOperation - leftToken;
	//Token_print(leftToken);
	//printf("leftLen: %d\n", leftLen);
	node->left = _TreeNode_new(leftToken, leftLen);
	Token* rightToken = chosenOperation + 1;
	int rightLen = node->exprLen + (node->expr - chosenOperation) - 1;
	//Token_print(rightToken);
	//printf("rightLen: %d\n", rightLen);
	node->right = _TreeNode_new(rightToken, rightLen);
	node->expr = chosenOperation;
	node->exprLen = 1;
	_TreeNode_split(node->right);
	_TreeNode_split(node->left);
}

Variable* _TreeNode_evaluate(struct TreeNode* node) {
	if (node->value) return node->value;
	_TreeNode_evaluate(node->left);
	_TreeNode_evaluate(node->right);
	node->value = Operation_calculate(node->left->value, node->right->value, node->expr->str);
	return node->value;
}

typedef struct {
	struct TreeNode* root;
} ExpressionTree;

ExpressionTree* ExpressionTree_new(Token* tokens, int len) {
	ExpressionTree* tree = malloc(sizeof(ExpressionTree));
	tree->root = _TreeNode_new(tokens, len);
	return tree;
}

void ExpressionTree_split(ExpressionTree* tree) {
	_TreeNode_split(tree->root);
}

Variable* ExpressionTree_evaluate(ExpressionTree* tree) {
	return _TreeNode_evaluate(tree->root);
}

void _TreeNode_free(struct TreeNode* node) {
	if(node == NULL) return;
	_TreeNode_free(node->left);
	_TreeNode_free(node->right);
	Variable_free(node->value);
	free(node);
}

void ExpressionTree_free(ExpressionTree* tree) {
	_TreeNode_free(tree->root);
}

void _TreeNode_print(struct TreeNode* node, int level) {
	if(node == NULL) return;
	for (int i = 0; i < level; i++) {
		printf("\t");
	}
	printf("%s\n", node->expr->str);
	_TreeNode_print(node->left, level+1);
	_TreeNode_print(node->right, level+1);
}

void ExpressionTree_print(ExpressionTree* tree) {
	_TreeNode_print(tree->root, 0);
	//printf("tree printed\n");
}
