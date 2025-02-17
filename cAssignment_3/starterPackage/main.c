#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 256

typedef struct hashItem {
    char *value;
    struct hashItem *next;
} HashItem;

typedef struct hashTable {
    int capacity;
    int size;
    HashItem **data;
} HashTable;

HashItem* initializeItem(char *value);
HashTable* initializeTable(int capacity);
int hash(char *value);
void insertToTable(char *wrd, HashTable *table);
int findValue(HashTable *table, char *value);
void isInvertedAdjacent(HashTable* table, char* value);
void isMissingLetter(HashTable* table, char* value);
void isExtraLetter(HashTable* table, char* value);
void suggestAlternatives(HashTable* table, char* value);
void freeHashTable(HashTable *table);
void freeItem(HashItem* item);

HashItem* initializeItem(char *value) {
    HashItem *item = malloc(sizeof(HashItem));
    if (!item) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    item->value = malloc(strlen(value) + 1);
    if (!item->value) {
        fprintf(stderr, "Memory allocation for value failed\n");
        free(item);  // Free the item as well
        exit(1);
    }
    strcpy(item->value, value);
    item->next = NULL;
    return item;
}

HashTable* initializeTable(int capacity) {
    HashTable *table = malloc(sizeof(HashTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    table->capacity = capacity;
    table->size = 0;
    table->data = calloc(capacity, sizeof(HashItem*));
    if (!table->data) {
        fprintf(stderr, "Memory allocation failed\n");
        free(table); // Free table before exiting
        exit(1);
    }    
    return table;
}

int hash(char *value) {
    unsigned int h = 0;
    while (*value) {
        h = 31 * h + (unsigned char)(*value++);
    }
    return h;
}

void insertToTable(char *wrd, HashTable *table) {
    unsigned int index = hash(wrd) % table->capacity;
    HashItem* newItem = initializeItem(wrd);
    newItem->next = table->data[index];
    table->data[index] = newItem;
}

int findValue(HashTable *table, char *value) {
    unsigned int index = hash(value) % table->capacity;
    HashItem* current = table->data[index];
    while (current != NULL) {
        if (strcmp(current->value, value) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void suggestAlternatives(HashTable* table, char* value) {
    printf("Suggestions: ");
    int length = strlen(value);
    char alternative[50];
    strcpy(alternative, value);
    for (int i = 0; i < length - 1; i++) {
        char temp = alternative[i];
        alternative[i] = alternative[i + 1];
        alternative[i + 1] = temp;
        if (findValue(table, alternative)) {
            printf("%s ", alternative);
        }
        temp = alternative[i];
        alternative[i] = alternative[i + 1];
        alternative[i + 1] = temp;
    }
    for (char c = 'a'; c <= 'z'; c++) {
        snprintf(alternative, sizeof(alternative), "%c%s", c, value);
        if (findValue(table, alternative)) {
            printf("%s ", alternative);
        }
    }
    for (char c = 'a'; c <= 'z'; c++) {
        snprintf(alternative, sizeof(alternative), "%s%c", value, c);
        if (findValue(table, alternative)) {
            printf("%s ", alternative);
        }
    }
    strcpy(alternative, value + 1);
    if (findValue(table, alternative)) {
        printf("%s ", alternative);
    }
    strcpy(alternative, value);
    alternative[length - 1] = '\0';
    if (findValue(table, alternative)) {
        printf("%s ", alternative);
    }
    printf("\n");
}

void freeItem(HashItem* item) {
    if (item) {
        free(item->value);
        free(item);
    }
}

void freeHashTable(HashTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        HashItem* current = table->data[i];
        while (current != NULL) {
            HashItem* temp = current;
            current = current->next;
            freeItem(temp);
        }
    }
    free(table->data);
    free(table);
}

int main(int argc, char **argv)
{
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file file
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; //this variable will tell us how much memory to allocate

	int insertToDictionary;
	if(strcmp(check,"add")==0)
		insertToDictionary = 1;
	else
		insertToDictionary = 0;
    
	////////////////////////////////////////////////////////////////////
	//read dictionary file
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    ssize_t lineSize; //variable to be used for line counting

    //check if the file is accessible, just to make sure...
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    //First, let's count number of words in the dictionary.
    //This will help us know how much memory to allocate for our hash table
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
        numOfWords++;

    //Printing line count for debugging purposes.
    //You can remove this part from your submission.
    //printf("%d\n",numOfWords);
    
    //HINT: You can initialize your hash table here, since you know the size of the dictionary
    HashTable *table = initializeTable(numOfWords);
    
    //rewind file pointer to the beginning of the file, to be able to read it line by line.
    fseek(fp, 0, SEEK_SET);

    char wrd[BUFSIZE];
    for (int i = 0; i < numOfWords; i++) {
        fscanf(fp, "%s \n", wrd);
        insertToTable(wrd, table);
    }
    fclose(fp);
    
	////////////////////////////////////////////////////////////////////
	//read the input text file word by word
    fp = fopen(inputFilePath, "r");
	
	//check if the file is accessible, just to make sure...
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

    //HINT: You can use a flag to indicate if there is a misspleed word or not, which is initially set to 1
	int noTypo=1;

	//read a line from the input file
	while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
	{
		char *word;
        //These are the delimiters you are expected to check for. Nothing else is needed here.
		const char delimiter[]= " ,.:;!\n";

		//split the buffer by delimiters to read a single word
		word = strtok(line,delimiter); 
		while(word!=NULL) {
            // You can print the words of the inpit file for Debug purposes, just to make sure you are loading the input text as intended
			//printf("%s\n",word);

            // HINT: Since this nested while loop will keep reading the input text word by word, here is a good place to check for misspelled words
            if (!findValue(table, word)) {
                printf("Misspelled word: %s\n", word);
                suggestAlternatives(table, word);
                noTypo = 0;
                if (insertToDictionary == 1) {
                    insertToTable(word, table);
                }
            }
            word = strtok(NULL, delimiter);
        }
    }
	fclose(fp);
    
    //HINT: If the flag noTypo is not altered (which you should do in the loop above if there exists a word not in the dictionary), then you should print "No typo!"
    if (noTypo == 1) {
        printf("No typo!\n");
    }
    
    // DON'T FORGET to free the memory that you allocated
    freeHashTable(table);
    free(line);
    return 0;
}
