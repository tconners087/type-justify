/*
 ============================================================================
 Name        : typeJustify.c
 Author      : Taylor C
 Version     : 1.0 
 Copyright   : I don't know dude, you can use it or whatever.
 Description : A program which prints a justified .txt file to the terminal.
 ============================================================================
 */

//Taylor Conners
//ITCS-3146-001 SPRING 2016
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define M 30

struct listNode {
	char data[M];
	struct listNode *lastPtr;
	struct listNode *nextPtr;
};

typedef struct listNode LISTNODE;
typedef LISTNODE *LISTNODEPTR;

void freeList(LISTNODEPTR*);
void instructions(void);
void instructions2(void);
void findParagraphStart(LISTNODEPTR *);
int findParagraphEnd(LISTNODEPTR *);
void insert(LISTNODEPTR *, char[M], LISTNODEPTR*);
void printList(LISTNODEPTR);
void printListReverse(LISTNODEPTR);
int typeJustify(LISTNODEPTR*, int, int, int*);

int main() {

	//THESE LINSE ARE INCLUDED BECAUSE MY CONSOLE IN ECLIPSE WOULDNT DISPLAY OUTPUT IN WINDOWS 10
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	FILE *fPtr;
	char word[M];
	char file_name[20];
	char thisLine[1024];
	char *token;
	const char delim[2] = " ";
	int i, numWordsPerLine, numWordsPar, numWordsPrinted = 0;

	LISTNODEPTR startPtr = NULL;
	LISTNODEPTR startPtrCpy = NULL;

	//Asks the user for file name and file path
	instructions();
	scanf("%s", file_name);
	instructions2();
	scanf("%d", &numWordsPerLine);

	fPtr = fopen(file_name, "r"); //open file

	while(fgets(thisLine, 1024, fPtr)){	//Reads in lines from text file
		printf("\n");
		token = strtok(thisLine, delim);	//Breaks up every word in the file as a string token
		while(token != NULL) {
			memset(word, '\0', sizeof(word));
			for(i = 0; i < strlen(token); i++){ //copies the string to another single character array
				if(!isalpha(token[i]) && !isdigit(token[i]) && (token[i] != '\n') && token[i] != '\0' && !isprint(token[i])) {
					token[i] = '\0';
				}
				word[i] = token[i];
			}
			insert(&startPtr, word, &startPtrCpy);
			token = strtok(NULL, delim); //loops through the tokens - delimeters are spaces
		}
	}

	fclose(fPtr); //close file

	while(startPtrCpy != NULL) { //Begin type-justification.
		findParagraphStart(&startPtrCpy);
		if(startPtrCpy != NULL){ //There is something to justify.
			numWordsPar = findParagraphEnd(&startPtrCpy); //Finds the total number of words before an empty line occurs.
			numWordsPrinted = 0;
			while(numWordsPrinted < numWordsPar) { //Only justify until the empty line.
				numWordsPrinted += typeJustify(&startPtrCpy, numWordsPerLine, numWordsPar, &numWordsPrinted);
				printf("\n");
			}
			//printf("\n"); //This can be uncommented for extra spacing on the output.
		}
	}
	freeList(&startPtr); //Frees all dynamically allocated memory in case OS does not.
	printf("\nEnd of program.");
	return 0;
}

void instructions(void) {
	printf("\nPlease enter the name and path of the file containing the text that you want formatted: ");
}

void instructions2(void) {
	printf("How many characters would you like in each line? Enter a number n such that 40 <= n <= 100 : ");
}

void insert(LISTNODEPTR *sPtr, char value[M], LISTNODEPTR *sPtr2) {
	LISTNODEPTR newPtr, currentPtr;
	newPtr = malloc(sizeof(LISTNODE));
	int i;

	if (newPtr != NULL) {
		for(i = 0; i < M; i++) {
			newPtr -> data[i] = value[i];
		}
		newPtr -> lastPtr = NULL;
		newPtr -> nextPtr = NULL;

		currentPtr = *sPtr;

		if (currentPtr == NULL) { //Two pointers to the starting pointer are made merely because I wanted to continue to be
			*sPtr = newPtr;		  //able to print the list using the printList and printListReverse functions, but one pointer is
			*sPtr2 = newPtr;	  //used to traverse the list during type-justification - thus does not continually point to the start of the list.
			return;
		}

		while(currentPtr->nextPtr != NULL) { //find that last node in the list
			currentPtr = currentPtr -> nextPtr;
		}
		currentPtr->nextPtr = newPtr; //add new node to end of list
		newPtr->lastPtr = currentPtr; //new node's previous ptr is the previous end of the list
	} else {
		printf("%s not inserted. No memory available. \n", value);
	}
}

void printList(LISTNODEPTR currentPtr) {
	if(currentPtr == NULL) {
		printf("List is empty.\n\n");
	} else {
		printf("\n\nThe list is: \n");

		while(currentPtr != NULL) {
			printf("%s ", currentPtr -> data);
			currentPtr = currentPtr -> nextPtr;
		}
		printf("\n");
	}
}

void printListReverse(LISTNODEPTR currentPtr) {
	while (currentPtr -> nextPtr != NULL) {
		currentPtr = currentPtr->nextPtr;
	}
	while (currentPtr != NULL) {
		printf("%s ", currentPtr->data);
		currentPtr = currentPtr->lastPtr;
	}
}

int typeJustify(LISTNODEPTR *currentPtr, int numCharWords, int numWordsInParagraph, int *numWordsPrinted) {

	LISTNODEPTR temp;
	temp = malloc(sizeof(LISTNODE));
	char space[2] = " ";
	int length = 0, words = 0, difference = 0, i, j = 0, totalWordsThisRun = 0;

	if (temp != NULL) {
		temp = *currentPtr;
	}

	//This loop iterates until an appropriate number of words have been found for the current line.
	while(temp != NULL) {
		for(i=0; i < strlen(temp->data);i++){
			if (temp->data[i] == ' ') { //For some reason, blank spaces were appended to strings on repeated iterations.
				temp->data[i] = '\0';   //This removes them, and consolidates space.
			}
		}
		words++;
		totalWordsThisRun++;
		length += strlen(temp->data);
		temp = temp->nextPtr;
		if((length+words) > numCharWords || temp == NULL || totalWordsThisRun > numWordsInParagraph || (length+words) > numCharWords-(words/2)) {break;}
	}

	difference = numCharWords - length; //How much space needs to be filled in order for the justification to look as desired.
	if(difference < words || length > numCharWords){ //if there are more words than there are free spaces to be padded,
		words--;
		totalWordsThisRun--;
		if(temp != NULL && temp->lastPtr != NULL){
			length-=(strlen((temp->lastPtr)->data));
		}
		difference = numCharWords - length;
	}

	temp = *currentPtr;
	if (totalWordsThisRun + *numWordsPrinted < numWordsInParagraph) { //There are still lines to print after this one.
		for(i = 0; i < difference; i++) {
			strcat(temp->data, space);
			temp = temp->nextPtr;
			j++;
			if(j == words-1) { //Never pad the last word in a sentence so that the final character aligns perfectly with the right margin.
				temp = *currentPtr;
				j=0;
			}
		}
	} else { //This is the last line in the paragraph, and it only needs one space between each word.
		for (i = 0; i < words-1; i++) {
			strcat(temp->data, space);
			temp = temp->nextPtr;
		}
	}

	temp = *currentPtr;
	for(i = 0; i < words; i++) { //Print the justified line to the console.
		if (temp != NULL && temp->data[0] != '\n') {
			printf("%s", temp->data);
			temp = temp->nextPtr;
		}
	}

	*currentPtr = temp;
	return totalWordsThisRun; //Returns the number of words printed this run to be added to the total number of words printed.
}

int findParagraphEnd(LISTNODEPTR *currentPtr) {
	LISTNODEPTR removal;
	LISTNODEPTR temp;
	char newParagraph = 'f';
	int num = 0, i;
	temp = *currentPtr;
	if (temp!=NULL) {
		while(temp != NULL && newParagraph == 'f') {
			for(i = 0;i < strlen(temp->data);i++){
				if (temp->data[i] == '\n') { //found a new line character
					if(temp->nextPtr != NULL) { //if this pointer w/ new line ch has a next node
						if(temp->nextPtr->data[0] != '\n') { //and the next node is not a new line ch
							temp->data[i] = '\0';
							//There might be more complicated cases for removal - not yet sure.
							//There might be a better place for this (a function call with various cases, maybe)
							//As the rest of the code is written, it will only ever remove nodes from the middle of the list, so this works.
							if (temp->data[0] == '\0'){ //Only remove the node if it contains a junk string - if its string's first index is NULL.
								removal = temp;
								removal->lastPtr->nextPtr = temp->nextPtr;
								removal->nextPtr->lastPtr = temp->lastPtr;
								num-=1;
								free(removal);
							}
						}
					}
				}
				if (temp->data[i] == '\n') {
					if(temp->nextPtr != NULL) {
						if(temp->nextPtr->data[0] == '\n') { //The end of the current paragraph has been reached.
							newParagraph = 't';				 //There is only ever a new paragraph if there is a blank line between
						}									 //sentences, requiring two consecutive new-line characters.
					}
				}

			}
			num+=1;
			temp = temp->nextPtr;
		}
	}
	return num;
}

void findParagraphStart(LISTNODEPTR *currentPtr) {
	char newParagraph = 'f';
	LISTNODEPTR temp;
	temp = *currentPtr;
	if (temp!=NULL) {
		while (temp!= NULL && newParagraph == 'f') {
			if (temp->data[0] != '\n'){ //first word in paragraph
				*currentPtr = temp;
				newParagraph = 't';
			}
			if (temp->nextPtr == NULL) { //End of list
				*currentPtr = temp->nextPtr;
				newParagraph = 't';
			}
			if (temp->data[0] == '\n') { //looking for a new paragraph
				temp = temp->nextPtr;

			}
		}
	}
}

void freeList(LISTNODEPTR *currentPtr) {
	LISTNODEPTR temp;
	LISTNODEPTR temp2;
	temp = *currentPtr;
	temp2 = *currentPtr;
	while (temp2 != NULL) {
		temp = temp2 ->nextPtr;
		free(temp2);
		temp2 = temp;
	}
}
