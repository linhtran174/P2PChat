typedef struct _user{
	Socket soc;
	char name[30]; //20 chars
} *User;

User newUser(char *name, Socket soc){
	User temp = (User)malloc(sizeof(struct _user));
	strcpy(temp->name, name);
	temp->soc = soc;
	return temp;
}


