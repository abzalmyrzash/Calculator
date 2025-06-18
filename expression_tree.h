#pragma once
#include "token.h"
#include "operation_priority.h"
#include "calculate.h"

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
	node->value = calculate(leftVal, rightVal, node->expr->str);
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
