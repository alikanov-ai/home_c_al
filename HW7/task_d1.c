/*
#include <stdio.h>
#include <stdlib.h>

typedef int datatype;

typedef struct tree
{
	datatype key;
	struct tree *left, *right;
} tree;

tree* insert(tree *root, datatype key)
{
	if (!root)
	{
		tree *node = (tree *)malloc(sizeof(tree));
		node->key = key;
		node->left = NULL;
		node->right = NULL;
		return node;
	}
	if (key < root->key) root->left  = insert(root->left,  key);
	else if (key > root->key) root->right = insert(root->right, key);
	return root;
}
*/
void btUpView(tree *root)
{
	tree *currentNode = root;
	int nLeft = 10;
	int arrLeft[10] = {0};
	if(root != NULL)
	{
		for(int i = 0; (currentNode = currentNode->left); i++) arrLeft[i] = currentNode->key;
		for(int j = 0; j < nLeft; j++)
		{
			if(!arrLeft[nLeft - j - 1]) continue;
			printf("%d ", arrLeft[nLeft - j - 1]);
		}
		do printf("%d ", root->key);
		while((root = root->right));
	}
}
/*
int main()
{
	tree *root = NULL;
	datatype x;
	// Читаем числа до 0 (признак конца)
	while (scanf("%d", &x) == 1 && x != 0) root = insert(root, x);
    btUpView(root);
	return 0;
}
*/
