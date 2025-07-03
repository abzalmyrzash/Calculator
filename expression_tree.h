#pragma once
#include "token.h"
#include "operation_priority.h"
#include "calculate.h"
#include "dynarr.h"

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
	free(node);
}

int _TreeNode_split(struct TreeNode* node) {
	Token* chosenOperation = NULL; // the operation token where the split happens
	// chosen operation must have minimum bracketLevel and priority
	OperationPriority priority, minPriority;
	// bracket level is how many bracket pairs we are inside of
	int bracketLevel = 0;
	int minBracketLevel;
	for (int i = node->exprLen-1; i >= 0; i--) {
		Token *token = node->expr + i;
		switch (token->type) {
	
		case TOKEN_TYPE_BRACKET2:
		case TOKEN_TYPE_SQRBR2:
			bracketLevel++;
			break;

		case TOKEN_TYPE_BRACKET1:
			bracketLevel--;
			break;

		case TOKEN_TYPE_OPERATION:
			priority = OperationPriority_getPriority(token->str);
			
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

		case TOKEN_TYPE_FUNCTION:
			priority = OPERATION_PRIORITY_UNARY;

			if (chosenOperation == NULL ||
		(priority < minPriority && bracketLevel == minBracketLevel)
			|| bracketLevel < minBracketLevel) {

				chosenOperation = token;
				minPriority = priority;
				minBracketLevel = bracketLevel;
			}
			break;

		case TOKEN_TYPE_SQRBR1:
			bracketLevel--;
			priority = OPERATION_PRIORITY_UNARY;

			if (chosenOperation == NULL ||
		(priority < minPriority && bracketLevel == minBracketLevel)
			|| bracketLevel < minBracketLevel) {

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
	int exprLen = 1;
	if (chosenOperation->type == TOKEN_TYPE_SQRBR1) {
		Token* token = chosenOperation+1;
		while (token != node->expr + node->exprLen) {
			if (token->type == TOKEN_TYPE_SQRBR1) {
				printf("ERROR: nested square bracket!\n");
				return 1;
			}
			if (token->type == TOKEN_TYPE_SQRBR2) {
				exprLen = token - chosenOperation + 1;
				break;
			}
			token++;
		}
		if (token->type != TOKEN_TYPE_SQRBR2) {
			printf("ERROR: no matching square bracket!\n");
			return 1;
		}
	}
	int leftLen = chosenOperation - node->expr;
	int rightLen = node->exprLen + (node->expr - chosenOperation) - exprLen;
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
		Token* rightToken = chosenOperation + exprLen;
		node->right = _TreeNode_new(rightToken, rightLen);
		if(_TreeNode_split(node->right) == 1) {
			_TreeNode_free(node->right);
			node->right = NULL;
		}
	}
	node->expr = chosenOperation;
	node->exprLen = exprLen;
	return 0;
}

Variable* evaluate_within_brackets(Token* expr, int exprLen);

// get dimensions from square brackets and write them into dims[2]
void get_dimensions(Token* expr, int exprLen, int dims[2]);
// use said dimensions to convert list into vector or matrix
Variable* convert_list(Variable* list, int dims[2]);
// use dimensions as indices to retrieve value from vector or matrix
Variable* get_by_indices(Variable* var, int indices[2]);

Variable* _TreeNode_evaluate(struct TreeNode* node) {
	if (node == NULL) return NULL;
	if (node->value != NULL) return node->value;
	if (node->expr->type == TOKEN_TYPE_OPERATION) {
		Variable* leftVal = _TreeNode_evaluate(node->left);
		if (leftVal != NULL && leftVal->type == VAR_TYPE_ERROR) return leftVal;
		Variable* rightVal = _TreeNode_evaluate(node->right);
		if (rightVal != NULL && rightVal->type == VAR_TYPE_ERROR) return rightVal;
		node->value = calculate_operation(leftVal, rightVal, node->expr->str);
		Variable_free(leftVal);
		Variable_free(rightVal);
	}
	if (node->expr->type == TOKEN_TYPE_FUNCTION) {
		Variable* rightVal = _TreeNode_evaluate(node->right);
		if (rightVal != NULL && rightVal->type == VAR_TYPE_ERROR) return rightVal;
		node->value = calculate_function(node->expr->str, rightVal);
		Variable_free(rightVal);
	}
	else if (node->expr->type == TOKEN_TYPE_SQRBR1) {
		int dims[2] = {0, 0};
		get_dimensions(node->expr, node->exprLen, dims);
		if (dims[0] == 0) {
			goto return_error;
		}
		// if there is something from the left side, use square brackets as indices
		// indices start from 1 not 0
		if(node->left != NULL && node->right == NULL) {
			Variable* leftVal = _TreeNode_evaluate(node->left);
			if (leftVal->type == VAR_TYPE_MATRIX || leftVal->type == VAR_TYPE_VECTOR) {
				node->value = get_by_indices(leftVal, dims);
			}
			Variable_free(leftVal);
		}
		// if there is something from the right side, use square brackets as dimensions
		else if(node->right != NULL && node->left == NULL) {
			Variable* rightVal = _TreeNode_evaluate(node->right);
			node->value = convert_list(rightVal, dims);
			Variable_free(rightVal);
		}
		else {
			printf("ERROR: failed to apply square brackets!\n");
			goto return_error;
		}
	}

	return node->value;
	return_error:
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
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
	free(tree);
}

void _TreeNode_print(struct TreeNode* node, int level) {
	for (int i = 0; i < level; i++) {
		printf("\t");
	}
	if(node == NULL) {
		printf("NULL\n");
		return;
	}
	for (int i = 0; i < node->exprLen; i++) {
		printf("%s", node->expr[i].str);
	}
	printf("\n");
	if (node->left != NULL || node->right != NULL) {
		_TreeNode_print(node->left, level+1);
		_TreeNode_print(node->right, level+1);
	}
}

void ExpressionTree_print(ExpressionTree* tree) {
	_TreeNode_print(tree->root, 0);
	//printf("tree printed\n");
}

Variable* evaluate_within_brackets(Token* expr, int exprLen) {
	if(exprLen <= 2) return NULL;
	ExpressionTree* tree = ExpressionTree_new(expr+1, exprLen-2);
	ExpressionTree_split(tree);
	Variable* res = ExpressionTree_evaluate(tree);
	ExpressionTree_free(tree);
	return res;
}

void get_dimensions(Token* expr, int exprLen, int dims[2]) {
	Variable* res = evaluate_within_brackets(expr, exprLen);
	if (res == NULL) {
		printf("ERROR: empty dimensions!\n");
		return;
	}
	if (res->type == VAR_TYPE_NUMBER) {
		int rounded = lround(*(double*)res->data);
		if (rounded > 0) {
			dims[0] = rounded;
		} else {
			printf("ERROR: dimensions must be positive!\n");
			goto outside;
		}
	}
	else if (Variable_is_list_of_numbers(res)) {
		DynArr* list = res->data;
		if (list->len != 2) {
			printf("ERROR: must contain no more than 2 dimensions!\n");
			goto outside;
		}
		Variable* arr = list->data;
		for (int i = 0; i < 2; i++) {
			int rounded = lround(*(double*)arr[i].data);
			if (rounded > 0) {
				dims[i] = rounded;
			} else {
				printf("ERROR: dimensions must be positive!\n");
				goto outside;
			}
		}
	}
	else {
		printf("ERROR: non-number dimensions!\n");
	}
outside:
	Variable_free(res);
}

Variable* get_by_indices(Variable* var, int indices[2]) {
	Variable* element;
	// human indices start from 1 not 0
	int i = indices[0];
	int j = indices[1];
	if (i > 0 && j == 0 && var->type == VAR_TYPE_VECTOR) {	
		i--; // decrement human index to computer index
		Vector* vector = var->data;
		if (!Vector_valid_index(vector, i)) {
			goto return_error;
		}
		double* val = malloc(sizeof(double));
		*val = vector->val[i];
		element = Variable_new(VAR_TYPE_NUMBER, NULL, val);
	}
	else if (i > 0 && j > 0 && var->type == VAR_TYPE_MATRIX) {
		i--; j--; // decrement human indices to computer indices
		Matrix* matrix = var->data;
		if (!Matrix_valid_indices(matrix, i, j)) {
			goto return_error;
		}
		double* val = malloc(sizeof(double));
		*val = *Matrix_at(matrix, i, j);
		element = Variable_new(VAR_TYPE_NUMBER, NULL, val);
	}
	else {
		goto return_error;
	}
	return element;
return_error:
	printf("ERROR: Invalid indices!\n");
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
}

Variable* convert_list(Variable* list, int dims[2]) {
	Variable* arr = ((DynArr*)list->data)->data;
	int len = ((DynArr*)list->data)->len;
	double* values = malloc(sizeof(double)*len);
	for (int i = 0; i < len; i++) {
		if (arr[i].type != VAR_TYPE_NUMBER) {
			goto return_error;
		} else {
			values[i] = *(double*)arr[i].data;
		}
	}
	Variable* var;
	int N = dims[0];
	int M = dims[1];
	if (N > 0 && M == 0) {
		if (N != len) {
			goto return_error;
		}
		Vector* vector = malloc(sizeof(Vector));
		vector->N = N;
		vector->val = values;
		var = Variable_new(VAR_TYPE_VECTOR, NULL, vector);
	}
	else if (N > 0 && M > 0) {
		if (N * M != len) {
			goto return_error;
		}
		Matrix* matrix = malloc(sizeof(Matrix));
		matrix->val = values;
		matrix->N = N;
		matrix->M = M;
		var = Variable_new(VAR_TYPE_MATRIX, NULL, matrix);
	}
	else {
		goto return_error;
	}
	return var;
return_error:
	printf("ERROR: invalid dimensions!\n");
	free(values);
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
}

