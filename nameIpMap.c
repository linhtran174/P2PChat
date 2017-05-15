char map[100][16]; //ddd.ddd.ddd.ddd

int _mapCount = 0;

void  mapPut(char *key, char *value);
char* mapGet(char *key);


struct _bstNode{
	struct _bstNode *left;
	struct _bstNode *right;
	char key[50]; //name
	char value[16]; //ip
};

typedef struct _bstNode *Node;

Node __root = NULL;

Node newNode(char *key, char *value){
	Node node = (Node)malloc(sizeof(struct _bstNode));
	node->left = NULL;
	node->right = NULL;
	strcpy(node->key, key);
	strcpy(node->value, value);	

	return node;
}

Node getNode(Node root, char *key){
	if(root == NULL) return NULL;
	int compare = strcmp(root->key, key);
	if(compare == 0) return root;
	if(compare >= 0) return getNode(root->left, key);
	else return getNode(root->right, key);
}

void putNode(Node *root, Node new){
	if(*root == NULL){
		*root = new;
		// printf("node putted: key: %s, value: %s\n", root->key, root->value);
		// printf("__root putted: key: %s, value: %s\n", __root->key, __root->value);
		return;	
	} 
	if(strcmp((*root)->key, new->key) >= 0){
		putNode(&((*root)->left), new);
	}
	else{
		putNode(&((*root)->right), new);
	}
}


void mapPut(char *key, char *value){
	putNode(&__root, newNode(key, value));
}

char* mapGet(char *key){
	return getNode(__root, key)->value;
}

