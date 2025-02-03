#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

#define BUFSIZE 256
#define INITIAL_CAPACITY 1

typedef struct item{
  char *word;
  int weight;
}Item;

typedef struct dataStruct{
    int capacity;
    int size;
    struct item** data;
}DataStruct;

Item* initializeItem(char* word, int weight)
{
	Item* newItem = malloc(sizeof(Item));
	
	newItem->word = malloc(sizeof(char)*(strlen(word)+1));
	strcpy(newItem->word, word);

	newItem->weight = weight;
	
	return newItem;
}

void freeItem(Item* item)
{
	if (item != NULL)
	{
		free(item->word);
        free(item);
	}

}

DataStruct* initializeHeap();

void deleteHeap(DataStruct* myHeap);

void doubleCapacity(DataStruct* myHeap);

void bubbleUp(DataStruct* myHeap, int index);

void insertItem(DataStruct* myHeap, Item* word);

void insertionSort(Item** suggestions, int size);

Item** createSuggestions(DataStruct* myHeap, char* word);

DataStruct* initializeHeap() {
    DataStruct* myHeap = malloc(sizeof(DataStruct));

    myHeap->capacity = INITIAL_CAPACITY;
    myHeap->size = 0;
    myHeap->data = malloc(sizeof(Item*) * myHeap->capacity);
    return myHeap;
}

void deleteHeap(DataStruct* myHeap) {
    if (myHeap != NULL) {
        for (int i = 0; i < myHeap->size; i++) {
            freeItem(myHeap->data[i]);
        }
        free(myHeap->data);
        free(myHeap);
    }
}

void doubleCapacity(DataStruct* myHeap) {
    myHeap->capacity *= 2;
    myHeap->data = realloc(myHeap->data, sizeof(Item*) * myHeap->capacity);
}

void bubbleUp(DataStruct* myHeap, int index) {
    int parent = (index - 1) / 2;  // Get the parent index

    // Check if the current node's weight is greater than its parent's weight
    while (index > 0 && myHeap->data[index]->weight > myHeap->data[parent]->weight) {
        // Swap the items
        Item* temp = myHeap->data[index];
        myHeap->data[index] = myHeap->data[parent];
        myHeap->data[parent] = temp;

        // Move up to the parent index
        index = parent;
        parent = (index - 1) / 2;
    }
}

void insertItem(DataStruct* myHeap, Item* word) {
    if (myHeap->size == myHeap->capacity - 1) {
        doubleCapacity(myHeap);
    }
    myHeap->data[myHeap->size] = word;
    myHeap->size += 1;
    bubbleUp(myHeap, myHeap->size - 1);
}

// Function to sort the suggestions array in descending order by weight
void insertionSort(Item** suggestions, int size) {
    for (int i = 1; i < size; i++) {
        Item* key = suggestions[i];
        int j = i - 1;

        // Move elements that are smaller than the key to one position ahead
        while (j >= 0 && suggestions[j]->weight < key->weight) {
            suggestions[j + 1] = suggestions[j];
            j--;
        }
        suggestions[j + 1] = key;
    }
}

Item** createSuggestions(DataStruct* myHeap, char* word) {
    Item** suggestions = malloc(sizeof(Item*) * 10);  // Array to store suggestions
    int suggestionCount = 0;

    // Iterate over the heap to check for words starting with the given "word"
    for (int i = 0; i < myHeap->size; i++) {
        Item* currentItem = myHeap->data[i];

        // Check if the word starts with the given prefix
        if (strncmp(currentItem->word, word, strlen(word)) == 0) {
            // Insert the currentItem into the suggestions array if there's space
            if (suggestionCount < 10) {
                suggestions[suggestionCount] = currentItem;
                suggestionCount++;
            } else {
                // Find the item with the smallest weight in the suggestions array
                int minIndex = 0;
                for (int j = 1; j < 10; j++) {
                    if (suggestions[j]->weight < suggestions[minIndex]->weight) {
                        minIndex = j;
                    }
                }

                // If the current item has a larger weight than the smallest, replace it
                if (currentItem->weight > suggestions[minIndex]->weight) {
                    suggestions[minIndex] = currentItem;
                }
            }
        }
    }
    if (suggestionCount == 0) {
        free(suggestions);  // Free the memory allocated for suggestions
        return NULL;
    }
    // Sort the suggestions array in descending order by weight
    insertionSort(suggestions, suggestionCount);

    return suggestions;
}

int main(int argc, char **argv) {
    char *dictionaryFilePath = argv[1]; //this keeps the path to dictionary file
    char *queryFilePath = argv[2]; //this keeps the path to the file that keeps a list of query wrods, 1 query per line
    int wordCount=0; //this variable will keep a count of words in the dictionary, telling us how much memory to allocate
    int queryCount=0; //this variable will keep a count of queries in the query file, telling us how much memory to allocate for the query words
    
    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// read dictionary file /////////////////////////
    ////////////////////////////////////////////////////////////////////////
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    ssize_t lineSize; //variable to be used for line counting
    
    //check if the file is accessible, just to make sure...
    if(fp == NULL){
        fprintf(stderr, "Error opening file:%s\n",dictionaryFilePath);
        return -1;
    }

    //First, let's count number of lines. This will help us know how much memory to allocate
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
    {
        wordCount++;
    }
    //Printing wordCount for debugging purposes. You can remove this part from your submission.
    //printf("%d\n",wordCount);
    
    /////////////////PAY ATTENTION HERE/////////////////
    //This might be a good place to allocate memory for your data structure, by the size of "wordCount"
    ////////////////////////////////////////////////////
    

    // This is for filling the dictionary data into memory
    //Read the file once more, this time to fill in the data into memory
    fseek(fp, 0, SEEK_SET);// rewind to the beginning of the file, before reading it line by line.
    char word[BUFSIZE]; //to be used for reading lines in the loop below
    int weight;
    DataStruct* myHeap = initializeHeap();
    for(int i = 0; i < wordCount; i++)
    {
        fscanf(fp, "%s %d\n",word,&weight);
        Item* item = initializeItem(word, weight);
        insertItem(myHeap, item);
        //Let's print them to the screen to make sure we can read input, for debugging purposes. You can remove this part from your submission.
        //printf("%s %d\n",word,weight);

        /////////////////PAY ATTENTION HERE/////////////////
        //This might be a good place to store the dictionary words into your data structure
        ////////////////////////////////////////////////////
    }
    // for (int i = 0; i < 20; i++) {
    //     printf("%s %d\n", myHeap->data[i]->word, myHeap->data[i]->weight);
    // }
    //close the input file
    fclose(fp);

    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// read query list file /////////////////////////
    ////////////////////////////////////////////////////////////////////////
    fp = fopen(queryFilePath, "r");
        
    //check if the file is accessible, just to make sure...
    if(fp == NULL){
        fprintf(stderr, "Error opening file:%s\n",queryFilePath);
        return -1;
    }

    //First, let's count number of queries. This will help us know how much memory to allocate
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
    {
        queryCount++;
    }
    free(line); //getline internally allocates memory, so we need to free it here so as not to leak memory!!

    //Printing line count for debugging purposes. You can remove this part from your submission.
    //printf("%d\n",queryCount);

    /////////////////PAY ATTENTION HERE/////////////////
    //This might be a good place to allocate memory for storing query words, by the size of "queryCount"
    ////////////////////////////////////////////////////

    fseek(fp, 0, SEEK_SET);// rewind to the beginning of the file, before reading it line by line.
    for(int i = 0; i < queryCount; i++)
    {
        fscanf(fp, "%s\n", word);
        //Let's print them to the screen to make sure we can read input, for debugging purposes. You can remove this part from your submission.
        //printf("%s %d\n",word);
        Item** suggestions = createSuggestions(myHeap, word);
        printf("Query word: %s\n", word);
        int index = 0;
        if (suggestions == NULL) {
            printf("No suggestion!\n");
        } else {
            while (suggestions[index] != NULL) {
                printf("%s %d\n", suggestions[index]->word, suggestions[index]->weight);
                index += 1;
            }
         }

        free(suggestions);
        /////////////////PAY ATTENTION HERE/////////////////
        //This might be a good place to store the query words in a list like data structure
        ////////////////////////////////////////////////////   
    }
    

    //close the input file
    fclose(fp);
    deleteHeap(myHeap);
    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// reading input is done ////////////////////////
    ////////////////////////////////////////////////////////////////////////
    
    //Now it is your turn to do the magic!!!
    //do search/sort/print, whatever you think you need to do to satisfy the requirements of the assignment!
    //loop through the query words and list suggestions for each query word if there are any
    //don't forget to free the memory before you quit the program!
    
    //OUTPUT SPECS:
    // use the following if no word to suggest: printf("No suggestion!\n");
    // use the following to print a single line of outputs (assuming that the word and weight are stored in variables named word and weight, respectively): 
    // printf("%s %d\n",word,weight);
    // if there are more than 10 outputs to print, you should print the top 10 weighted outputs.
    
    return 0;
}
