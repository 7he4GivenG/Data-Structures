#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFSIZE 256
#define MAX_WORDS 10000  // Adjust based on your dictionary size
#define MAX_WORD_LENGTH 50
#define MAX_SUGGESTIONS 100

typedef struct hashItem {
    int key;
    char *value;
    struct hashItem *next;
} HashItem;

typedef struct hashTable {
    int capacity;
    int size;
    HashItem **data;
} HashTable;

HashItem* initializeItem(int key, char *value);
HashTable* initializeTable(int capacity);
int hash(char *value, int capacity);
bool searchT(HashTable *table, int key, char *value);
void insertToTable(char *wrd, HashTable *table);
bool is_inverted_adjacent(const char *input, const char *dict_word);
int is_missing_letter(const char *word, const char *dict_word);
int is_extra_letter(const char *word, const char *dict_word);
char* find_suggestions(HashTable *table, const char *input);

HashItem* initializeItem(int key, char *value) {
    HashItem *item = malloc(sizeof(HashItem));
    if (!item) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    item->key = key;
    item->value = malloc(strlen(value) + 1); // Allocate memory for the string
    if (!item->value) {
        fprintf(stderr, "Memory allocation for value failed\n");
        exit(1);
    }
    strcpy(item->value, value);  // Copy the value string
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
    table->data = calloc(capacity, sizeof(HashItem*)); // Properly initialize to NULL
    return table;
}

int hash(char *value, int capacity) {
    __uintmax_t h = 0;
    __uintmax_t pow = 1;
    while (*value != '\0') {
        h += (*value * pow) % capacity;
        value++;
        pow *= 31;
    }

    return h % capacity;
}

bool searchT(HashTable *table, int key, char *value) {
    HashItem *array = table->data[key];
    //printf("%s", array->value);
    while (array != NULL) {
        //printf("%s\n", array->value);
        if(strcmp(array->value, value) == 0) {
            return true;
        } else {
            array = array->next;
        }
    }
    return false;
}

void insertToTable(char *wrd, HashTable *table) {
    int index = hash(wrd, table->capacity);
        HashItem *newItem = initializeItem(index, wrd);
        
        if (table->data[index] == NULL) {
            table->data[index] = newItem;
        } else {
            HashItem *tableItem = table->data[index];
            while (tableItem->next != NULL) {
                tableItem = tableItem->next;
            }
        tableItem->next = newItem;
    }
}

bool is_inverted_adjacent(const char *input, const char *dict_word) {
    int len = strlen(input);
    if (strlen(dict_word) != len) return false; // Must be same length

    for (int i = 0; i < len - 1; i++) {
        if (input[i] == dict_word[i + 1] && input[i + 1] == dict_word[i]) {
            if (strncmp(input, dict_word, i) == 0 && strcmp(input + i + 2, dict_word + i + 2) == 0) {
                return true;
            }
        }
    }
    return false;
}

// Check if input is missing exactly one letter compared to dict_word
bool is_missing_one_letter(const char *input, const char *dict_word) {
    int len1 = strlen(input), len2 = strlen(dict_word);
    if (len1 + 1 != len2) return false; // Must be missing exactly one letter

    for (int i = 0; i < len2; i++) {
        if (strncmp(input, dict_word, i) == 0 && strcmp(input + i, dict_word + i + 1) == 0) {
            return true;
        }
    }
    return false;
}

// Check if input has one extra letter compared to dict_word
bool is_extra_one_letter(const char *input, const char *dict_word) {
    int len1 = strlen(input), len2 = strlen(dict_word);
    if (len1 - 1 != len2) return false; // Must have exactly one extra letter

    for (int i = 0; i < len1; i++) {
        if (strncmp(input, dict_word, i) == 0 && strcmp(input + i + 1, dict_word + i) == 0) {
            return true;
        }
    }
    return false;
}

// Suggest words based on one single error type
char* find_suggestions(HashTable *table, const char *input) {
    char *suggestions = malloc(MAX_SUGGESTIONS * 256);  // Allocate enough space
    if (!suggestions) return NULL;
    suggestions[0] = '\0';
    
    for (int i = 0; i < table->capacity; i++) {
        HashItem *current = table->data[i];
        while (current) {
            if (is_inverted_adjacent(input, current->value) ||
                is_missing_one_letter(input, current->value) ||
                is_extra_one_letter(input, current->value)) {
                strcat(suggestions, current->value);
                strcat(suggestions, " ");
            }
            current = current->next;
        }
    }
    return suggestions;
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
    for (int i = 0; i < numOfWords; i++)
    {
        fscanf(fp, "%s \n", wrd);
        //You can print the words for Debug purposes, just to make sure you are loading the dictionary as intended
        //printf("%d: %s\n",i,wrd);
        //HINT: here is a good place to insert the words into your hash table
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
		//printf("Word: %s, Key: %d\n", word, hash(word, table->capacity));
        //printf("Word: %s, Key: %d\n", "hello", hash("hello", table->capacity));
        //printf("Word: %s, Key: %d\n", "zwitterionic", hash("hello", table->capacity));
		//read the line word by word
		while(word!=NULL)
		{
            // You can print the words of the inpit file for Debug purposes, just to make sure you are loading the input text as intended
			//printf("%s\n",word);

            
            // HINT: Since this nested while loop will keep reading the input text word by word, here is a good place to check for misspelled words
            int key = hash(word, table->capacity);
            if (!searchT(table, key, word)) {
                noTypo = 0;
                printf("Misspelled word: %s\n",word);
                if (insertToDictionary == 1) {
                    insertToTable(word, table);
                }
                printf("Suggestions: %s\n", find_suggestions(table, word));
            }
            
            // INPUT/OUTPUT SPECS: use the following line for printing a "word" that is misspelled.
            //printf("Misspelled word: %s\n",word);
            
            // INPUT/OUTPUT SPECS: use the following line for printing suggestions, each of which will be separated by a comma and whitespace.
            //printf("Suggestions: "); //the suggested words should follow
            
            
            
			word = strtok(NULL,delimiter); 
		}
	}
	fclose(fp);
    
    //HINT: If the flag noTypo is not altered (which you should do in the loop above if there exists a word not in the dictionary), then you should print "No typo!"
    if(noTypo==1)
        printf("No typo!\n");
    

    // DON'T FORGET to free the memory that you allocated
    
	return 0;
}
