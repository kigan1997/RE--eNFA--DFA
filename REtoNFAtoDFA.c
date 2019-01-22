/*
This program is to convert a Regular Expression to 
corrosponding NFA with epsilon move to 
it's corrosponding Equivalent DFA 

Created by - Akash Banerjee
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int pt = 0;

typedef struct e_closure {
	int data;
	struct e_closure *addr;
} E_CLOSURE;


typedef struct alphabet_stack {
	char data;
	struct alphabet_stack *addr;
} ALPHABET;

typedef struct state_stack {
	int start, end;
	struct state_stack *addr;
} STATES;

void push_alphabet(ALPHABET **s, char c) {
	ALPHABET *p = malloc(sizeof(ALPHABET));
	p->data = c;
	p->addr = *s;
	(*s) = p;
}

char pop_alphabet(ALPHABET **s) {
	if((*s) == NULL) return '\0';
	ALPHABET *temp = *s;
	char c = temp->data;
	(*s) = temp->addr;
	free(temp);
	return c;
}

void push_state(STATES **s, int start, int end) {
	STATES *p = malloc(sizeof(STATES));
	p->start = start;
	p->end = end;
	p->addr = *s;
	(*s) = p;
}

void pop_state(STATES **s, int *i, int *j) {
	if((*s) == NULL) {
		(*i) = -1;
		(*j) = -1;
	} else {
		STATES *temp = *s;
		(*i) = temp->start;
		(*j) = temp->end;
		(*s) = temp->addr;
		free(temp);
	}
}

int indexA(char symbol[],char ip){
	if(ip == 'E')
		return strlen(symbol);
	for(int i=0;i<strlen(symbol);i++)
		if(symbol[i]==ip)
			return i;
	return -1;
}


char* postfix(char *instr, int* states) {
	int len = strlen(instr), j=0;

	char ch, *outputStr = malloc(50 * sizeof(char));
	ALPHABET *head = NULL;
	for(int i = 0; i < len; i++) {
		if((instr[i] >= 'a' && instr[i] <= 'z') || (instr[i] == '*') || (instr[i] == 'E')) {	
			outputStr[j++] = instr[i];
			(*states)++;
		}
		else if((instr[i] == '+') || (instr[i] == '|')) {
			push_alphabet(&head, instr[i]);
			(*states)++;
		}
		else if((instr[i] == '.') || (instr[i] == '(')) {
			push_alphabet(&head, instr[i]);
		}
		else if (instr[i] == ')') {
			while((ch = pop_alphabet(&head)) && ch != '(') 
				outputStr[j++] = ch;
		}
	}
	while((ch= pop_alphabet(&head)) && ch != '(') 
		outputStr[j++] = ch;
	outputStr[j] = '\0';
	return outputStr;
}


//append to a link list in sorted order
int append(E_CLOSURE ** h,int data){
	E_CLOSURE * temp = (E_CLOSURE*)malloc(sizeof(E_CLOSURE));
	E_CLOSURE * c = *h, *d;

	temp->data = data;
	temp->addr = NULL;
	if(*h == NULL || (*h)->data > data){
		temp->addr = *h;
		*h = temp;
		return 1;
	}
	if((*h)->data == data)
		return 0;
	while(c->addr != NULL && c->addr->data <= data){	
		if(c->data == data)
			return 0;
		c = c->addr;
	}
	if(c->data == data)
		return 0;
	temp->addr = c->addr;
	c->addr = temp;
	return 1;
}


//find closure of a NFA state
void closure(E_CLOSURE ** h,int state,int N,int* (*table)[N + 1]){

	if(!append(h,state)) return;
	if(table[state][N][0] == -1)
		return; 
	int k=0;
	while(table[state][N][k] != -1)
	{
		closure(h,table[state][N][k],N,table);
		k++;
	}
}

//find closure of a DFA state(a set of NFA states)
void closure_of_list(E_CLOSURE * h,E_CLOSURE ** h2,int N,int* (*table)[N + 1]){
	E_CLOSURE * c = h;
	while(c != NULL){
		closure(h2,c->data,N,table);
		c = c->addr;
	}
}

//ep-move of a DFA state
void e_trasition(E_CLOSURE * h,E_CLOSURE ** h2,int N,int* (*table)[N + 1],int sym){
	E_CLOSURE * c = h;
	int k;
	while(c != NULL){
		k = 0;
		while(table[c->data][sym][k] > -1){
			append(h2,table[c->data][sym][k++]);
		}
		c = c->addr;
	}
}


void print_closure(E_CLOSURE *h) {
	while(h != NULL) {
		printf("%d, ", h->data);
		h = h->addr;
	}
	printf("END\n");
}

//check two DFA state is same or not
int check_list_same(E_CLOSURE * h1, E_CLOSURE * h2){
	while(h1 != NULL && h2 != NULL){
		if(h1->data != h2->data)
			return 1;
		h1 = h1->addr;
		h2 = h2->addr;
	}
	if(h1 == NULL && h2 == NULL)
		return 0;
	return 1;
}


//check a DFA state is new or not
int check_multiple_list_same(E_CLOSURE** h, E_CLOSURE* h2) {
	int j;
	for(j = 0; j <= pt; j++){
		if(!check_list_same(h[j], h2)) {
			break;
		}
	}
	if (j == pt + 1) 
		return -1;
	else
		return j;
}

//check a DFA state is final state or not
int check_final(E_CLOSURE * h,int final){
	if(h == NULL) return 2;
	while(h!=NULL){
		if(h->data == final)
			return 1;
		h = h->addr;
	}
	return 0;
}


int main(void){
	int states=0;
	char symbol[26],inStr[50],*inStr2;
	printf("Enter symbol set:");
	scanf("%s",symbol);
	printf("Enter any Regular Expression:");
	scanf("%s",inStr);
	
	inStr2=postfix(inStr,&states);
	printf("%s\n", inStr2);

	states=states * 2;
	int *table[states][strlen(symbol) + 1];

	for(int i=0;i<states;i++)
		for(int j=0;j<strlen(symbol) + 1;j++){
			table[i][j] = (int *)malloc(4 * sizeof(int));
			for(int k = 0; k < 4; k++)
				table[i][j][k] = -1;
		}	


	int k = -1;
	// Initialize the transition states for input symbols
	int ep = strlen(symbol);

	STATES *head = NULL;
	int a1, a2, b1, b2;
	int len = strlen(inStr2);


	for(int i = 0, j = 0; i < len; i++, j += 2) {
		if ((inStr2[i] >= 'a' && inStr2[i] <= 'z') || inStr2[i] == 'E'){
			if(indexA(symbol,inStr2[i])>-1){ // if symbol exist in symbol set
				push_state(&head, j, j+1);
				k = -1;
				while(table[j][indexA(symbol,inStr2[i])][++k]!=-1);
				table[j][indexA(symbol,inStr2[i])][k] = (j + 1);
			}
			else{ //if symbol doesn't exist in symbol set
				printf("Wrong Input\n");
				exit(1);
			}
		}
		else if ((inStr2[i] == '+') || (inStr2[i] == '|')) {
			pop_state(&head, &b1, &b2);
			pop_state(&head, &a1, &a2);

			k=-1;
			while(table[j][ep][++k]!=-1);
			table[j][ep][k++] = a1;	//j --> a1
			table[j][ep][k++] = b1;	//j --> b1

			k=-1;
			while(table[a2][ep][++k]!=-1);
			table[a2][ep][k++] = j + 1;	//a2 --> j+1

			k=-1;
			while(table[b2][ep][++k]!=-1);
			table[b2][ep][k++] = j + 1;	//b2 --> j+1


			push_state(&head, j, j + 1);
		}

		else if (inStr2[i] == '.') {
			j-= 2;
			pop_state(&head, &b1, &b2);
			pop_state(&head, &a1, &a2);

			k=-1;
			//a2 --> b1
			for(int n=0;n<=ep;n++){
				k=0;
				int l=-1;
				while(table[a2][n][++l]!=-1);
				while(table[b1][n][k]!=-1){
					table[a2][n][l++] = table[b1][n][k++];
				}
				table[b1][n][0] = -2;
			}

			for(int a=0;a<states;a++)
				for (int b=0;b<=ep;b++)
					for(int c=0;table[a][b][c]!=-1;c++)
						if(table[a][b][c]==b1)
							table[a][b][c]=a2;


			push_state(&head, a1, b2);
		}

		else if (inStr2[i] == '*') {
			pop_state(&head, &a1, &a2);

			k=-1;
			while(table[a2][ep][++k]!=-1);
			table[a2][ep][k++] = a1;	//a2 --> a1

			table[a2][ep][k++] = j + 1;	//a2 --> j+1


			k=-1;
			while(table[j][ep][++k]!=-1);
			table[j][ep][k++] = a1;	//j --> a1

			table[j][ep][k++] = j + 1;	//j --> j+1

			
			push_state(&head, j, j + 1);
		}
	}

	pop_state(&head, &a1, &a2);

	printf("\nStarting state: -->%d\n",a1);
	printf("Final state: (%d)\n",a2);

	//print table
	printf("The NFA transition table is as follows:\n");
	for(int i =0;i< ep;i++)
		printf("\t%c",symbol[i]);
	printf("\tep\n");
	for(int i=0;i<states;i++){
		if(i==a1) 	printf("q%d<--:\t", i);
		else if(i==a2) 	printf("(q%d):\t", i);
		else if(table[i][0][0]==-2) continue;
		else	printf("q%d:\t",i);
		for(int j=0;j<ep + 1;j++){
			for (int k = 0;(k < 1) || (table[i][j][k] != -1); k++){
				if(table[i][j][k] == -1)
					printf("-");
				else
				printf("q%d,", table[i][j][k]);
			}
			printf("\t");
		}	
		printf("\n");
	}


	//Converting e-NFA to equivalent DFA

	E_CLOSURE *head1 = NULL;
	closure(&head1, a1, ep, table); //ep-closure of starting state of NFA = starting state of DFA


	int iSize = 10, currentSize = iSize;
	E_CLOSURE ** head2 = NULL, *temp1 = NULL, *temp2 = NULL;
	head2 = (E_CLOSURE**)malloc(currentSize * sizeof(E_CLOSURE*));

	int ** table2 = (int **)malloc(currentSize * sizeof(int *)); //table2 = table for DFA
	for(int i = 0; i < currentSize; i++){
		table2[i] = (int *)malloc(ep * sizeof(int));
		head2[i] = NULL;
	}

	int i = 0;
	head2[i] = head1;
	
	for(i = 0; i <= pt;i++){
		if (i == currentSize-4){
				table2 = (int **)realloc(table2, (currentSize + iSize) * sizeof(int *));
				head2 = (E_CLOSURE**)realloc(head2, (currentSize + iSize) * sizeof(E_CLOSURE*));
				for(int k = currentSize; k < (currentSize + iSize); k++){
					table2[k] = (int *)malloc(ep * sizeof(int));
					head2[k] = NULL;
				}
				currentSize += iSize;	
			}
		for(int j = 0; j < ep ; j++){
			e_trasition(head2[i],&temp1,ep,table,indexA(symbol,symbol[j])); //ep transition of a state
			closure_of_list(temp1,&temp2,ep,table); //  ep-closure of that transition

			int abc = check_multiple_list_same(head2,temp2); //check for new state
			if(abc != -1)//for old state
				table2[i][j] = abc;
			else{	//for new state, put that state into list of states
				head2[++pt] = temp2;
				table2[i][j] = pt;
			}
			temp1 = temp2 = NULL;
		}
	}


//print DFA table
	printf("\nThe equivalent DFA transition table is as follows:\n");

	for(int p = 0; p < pt; p++)
		printf("\t%c",symbol[p]);
	printf("\n");

	for(int p = 0; p <= pt; p++){
		if(!p && check_final(head2[p],a2) == 1) printf("(S%d)<-:\t",p);
		else if(!p && check_final(head2[p],a2) == 0) printf("S%d<--:\t",p);
		else if(check_final(head2[p],a2) == 1) printf("(S%d):\t",p);
		else if(check_final(head2[p],a2) == 2) printf("S%d(R):\t",p);
		else printf("S%d:\t",p);
		for(int q = 0; q < ep; q++)
		{
			/*if(table2[p][q] == -1)
				printf("-\t");
			else*/
				printf("S%d\t", table2[p][q]);
		}
		printf("\n");
	}

	free(head);
	free(head1);
	free(head2);
	free(table2);
	free(temp1);
	free(temp2);
	for(int a = 0; a < states; a++)
		for(int b = 0; b < ep; b++)
			free(table[a][b]);
}
