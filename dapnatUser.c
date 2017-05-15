struct _user{
	struct _user *left;
	struct _user *right;
	Socket soc;
	char name[30]; //20 chars
};
typedef struct _user *User;

void  mapPut(char *name, User user);
User mapGet(char *name);

User __root = NULL;

User newUser(char *name, Socket soc){
	User temp = (User)malloc(sizeof(struct _user));
	temp->left = NULL;
	temp->right = NULL;
	strcpy(temp->name, name);
	temp->soc = soc;

	return temp;
}

User getUser(User root, char *name){
	if(root == NULL) return NULL;
	int compare = strcmp(root->name, name);
	if(compare == 0) return root;
	if(compare >= 0) return getUser(root->left, name);
	else return getUser(root->right, name);
}

void putUser(User *root, User new){
	if(*root == NULL){
		*root = new;
		return;
	}
	if(strcmp((*root)->name, new->name) >= 0){
		putUser(&((*root)->left), new );
	}
	else{
		putUser(&((*root)->right), new);
	}
}

void mapPut(char *name, User user){
	putUser(&__root, user);
}

User mapGet(char *name){
	return getUser(__root, name);
}

User parseUser(char *string){
	char name[30];
	char ip[16];
	char port[6];
	strcpy(name, strtok(string, "_"));
	strcpy(ip, strtok(NULL, "_"));
	strcpy(port, strtok(NULL, "_"));

	User user = newUser(
		name, newSocket(ip, port)
	);
	return user;
}

char *serializeUser(User user){
	char *string = (char *)malloc(30+16+6);
	sprintf(string, "%s_%s_%s",
	  user->name,
	  user->soc->ip,
	  user->soc->port
	);
	return string;
}