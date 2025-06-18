#pragma once
#include <math.h>
#include "mymath.h"
#include "globals.h"
#include "token.h"

Variable* Operation_calculate(Variable* a, Variable* b, char* op) {
	if (a == NULL && b == NULL) {
		printf("ERROR: Operation %s with nothing!\n", op);
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}

	if (a != NULL && b == NULL) {
		if (a->type == VAR_TYPE_PERCENT) {
			a->type = VAR_TYPE_NUMBER;
			*(double*)a->data /= 100;
		}

		if (a->type == VAR_TYPE_NUMBER) {
			if(a->name != NULL) {
				Variable* var = HashMap_search(hashmap, a->name);
				if(strsame(op, "++")) {
					*(double*)var->data = *(double*)a->data + 1;
					return Variable_copy(a);
				}
				else if(strsame(op, "--")) {
					*(double*)var->data = *(double*)a->data - 1;
					return Variable_copy(a);
				}
			}
			double* res = malloc(sizeof(double));
			if (strsame(op, "!")) {
				*res = factorial(*(double*)a->data);
				return Variable_new(VAR_TYPE_NUMBER, NULL, res);
			}
			if(strsame(op, "%")) {
				*res = *(double*)a->data;
				return Variable_new(VAR_TYPE_PERCENT, NULL, res);
			}
			else {
				free(res);
				printf("ERROR: Operation %s is invalid for a number!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}

		else if (a->type == VAR_TYPE_MATRIX) {
			Matrix* res;
			if (strsame(op, "'")) {
				res = Matrix_transpose(a->data);
				return Variable_new(VAR_TYPE_MATRIX, NULL, res);
			}
			else {
				printf("ERROR: Operation %s is invalid for a matrix!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}

		printf("ERROR: Invalid variable type!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}		

	if (a == NULL && b != NULL) {
		if (b->type == VAR_TYPE_PERCENT) {
			b->type = VAR_TYPE_NUMBER;
			*(double*)b->data /= 100;
		}

		if (b->type == VAR_TYPE_NUMBER) {
			if(b->name != NULL) {
				Variable* var = HashMap_search(hashmap, b->name);
				if(strsame(op, "++")) {
					*(double*)var->data = *(double*)b->data + 1;
					return Variable_copy(var);
				}
				else if(strsame(op, "--")) {
					*(double*)var->data = *(double*)b->data - 1;
					return Variable_copy(var);
				}
			}
			double* res = malloc(sizeof(double));
			if(strsame(op, "-")) {
				*res = -(*(double*)b->data);
				return Variable_new(VAR_TYPE_NUMBER, NULL, res);
			}
			else {
				free(res);
				printf("ERROR: Operation %s is invalid for a number!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}
		else if (b->type == VAR_TYPE_MATRIX) {
			Matrix *res;
			if(strsame(op, "-")) {
				res = Matrix_scale(b->data, -1);
				return Variable_new(VAR_TYPE_MATRIX, NULL, res);
			}
			else {
				printf("ERROR: Operation %s is invalid for a matrix!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}
		printf("ERROR: Invalid variable type!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
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
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			*res = a_val / b_val;
		}

		else if (strsame(op, "//")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			*res = floor(a_val / b_val);
		}
		else if (strsame(op, "%%")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			*res = a_val - b_val * floor(a_val / b_val);
		}
		else if (strsame(op, "^")) {
			*res = pow(a_val, b_val);
		}
		else {
			free(res);
			printf("ERROR: Operation %s is invalid for two numbers!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
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
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
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
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
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
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}

	else if (a->type == VAR_TYPE_NUMBER && b->type == VAR_TYPE_PERCENT) {
		double *res = malloc(sizeof(double));
		double number = *(double*)a->data;
		double percent = *(double*)b->data;
		if (strsame(op, "+")) {
			*res = number * (1 + percent/100);
		}
		else if (strsame(op, "-")) {
			*res = number * (1 - percent/100);
		}
		else {
			b->type = VAR_TYPE_NUMBER;
			*(double*)b->data /= 100;
			return Operation_calculate(a, b, op);
		}
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	else if (a->type == VAR_TYPE_PERCENT) {
		a->type = VAR_TYPE_NUMBER;
		*(double*)a->data /= 100;
		return Operation_calculate(a, b, op);
	}

	printf("ERROR: Invalid variable type!\n");
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
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
char* unaryList[] = {"%", "!", "'", "~", "++", "--"};
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

void _TreeNode_free(struct TreeNode* node) {
	if(node == NULL) return;
	_TreeNode_free(node->left);
	_TreeNode_free(node->right);
	Variable_free(node->value);
	free(node);
}

int _TreeNode_split(struct TreeNode* node) {
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
		if (node->value == NULL) return 1;
		return 0;
	}
	// if operation was found, split expression into left and right children
	// with operation in the middle as their parent
	// Token_print(chosenOperation);
	int leftLen = chosenOperation - node->expr;
	int rightLen = node->exprLen + (node->expr - chosenOperation) - 1;
	// printf("%d\n", leftLen);
	// printf("%d\n", rightLen);

	if (leftLen > 0) {
		Token* leftToken = node->expr;
		node->left = _TreeNode_new(leftToken, leftLen);
		if(_TreeNode_split(node->left) == 1) {
			_TreeNode_free(node->left);
			node->left = NULL;
		}
	}
	if (rightLen > 0) {
		Token* rightToken = chosenOperation + 1;
		node->right = _TreeNode_new(rightToken, rightLen);
		if(_TreeNode_split(node->right) == 1) {
			_TreeNode_free(node->right);
			node->right = NULL;
		}
	}
	node->expr = chosenOperation;
	node->exprLen = 1;
	return 0;
}

Variable* _TreeNode_evaluate(struct TreeNode* node) {
	if (node == NULL) return NULL;
	if (node->value != NULL) return node->value;
	Variable* leftVal = _TreeNode_evaluate(node->left);
	if (leftVal != NULL && leftVal->type == VAR_TYPE_ERROR) return leftVal;
	Variable* rightVal = _TreeNode_evaluate(node->right);
	if (rightVal != NULL && rightVal->type == VAR_TYPE_ERROR) return rightVal;
	node->value = Operation_calculate(leftVal, rightVal, node->expr->str);
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

int ExpressionTree_split(ExpressionTree* tree) {
	return _TreeNode_split(tree->root);
}

Variable* ExpressionTree_evaluate(ExpressionTree* tree) {
	Variable* var = _TreeNode_evaluate(tree->root);
	if (var == NULL || var->type == VAR_TYPE_ERROR) return var;
	if (var->type == VAR_TYPE_PERCENT) {
		var->type = VAR_TYPE_NUMBER;
		*(double*)var->data /= 100;
	}
	return var;
}

void ExpressionTree_free(ExpressionTree* tree) {
	_TreeNode_free(tree->root);
}

void _TreeNode_print(struct TreeNode* node, int level) {
	for (int i = 0; i < level; i++) {
		printf("\t");
	}
	if(node == NULL) {
		printf("NULL\n");
		return;
	}
	printf("%s\n", node->expr->str);
	if (node->left != NULL || node->right != NULL) {
		_TreeNode_print(node->left, level+1);
		_TreeNode_print(node->right, level+1);
	}
}

void ExpressionTree_print(ExpressionTree* tree) {
	_TreeNode_print(tree->root, 0);
	//printf("tree printed\n");
}
