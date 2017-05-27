typedef struct _treeNode{
	struct _treeNode *left;
	struct _treeNode *right;
	char key[50];
	char value[16+6]; //ddd.ddd.ddd.ddd:ddddd
}* Node;

void   phonebookPut(char *name, char* soc);
char*  phonebookGet(char *nameOrSocket);

Node __root = NULL;

Node newNode(char* key, char* value){
	Node temp = (Node)malloc(sizeof(struct _treeNode));
	strcpy(temp->key, key);
	strcpy(temp->value, value);
	return temp;
}

Node getNode(Node root, char *key){
	if(root == NULL) return NULL;
	int compare = strcmp(root->key, key);
	if(compare == 0) return root;
	if(compare >= 0) return getNode(root->left, key);
	return getNode(root->right, key);
}

void putNode(Node *root, Node new){
	if(*root == NULL){
		*root = new;
		return;
	}
	if(strcmp((*root)->key, new->key) >= 0){
		putNode(&((*root)->left), new );
	}
	else{
		putNode(&((*root)->right), new);
	}
}

void phonebookPut(char *name, char* soc){
	// printf("phonebookPut: name: %s, soc: %s\n", name, soc);
	Node nameToSoc = newNode(name, soc);
	Node socToName = newNode(soc, name);
	putNode(&__root, nameToSoc);
	putNode(&__root, socToName);
}

char* phonebookGet(char *nameOrSocket){
	Node result = getNode(__root, nameOrSocket);
	if(result == NULL) return NULL;
	return result->value;
}

typedef struct _dapnatUser{
	char name[50];
	Socket soc;
} *User;

User newUser(char *name, Socket soc){
	User new = (User)malloc(sizeof(struct _dapnatUser));
	strcpy(new->name , name);
	new->soc = newSocket(soc->ip, soc->port);
	return new;
}