#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define NUM_CHAR 256


typedef struct trienode{
    struct trienode* children[NUM_CHAR];
    bool terminal;
} trienode;


trienode* createNode(){
    trienode* newNode = malloc(sizeof(trienode));
    //setting null
    for(int i=0;i<NUM_CHAR;i++){
        newNode->children[i] = NULL;
    }
    newNode->terminal = false;
    return newNode;
}

bool insertNode(trienode** root,char* str){

    if(*root == NULL){
        *root = createNode();
    }

    unsigned char* text = (unsigned char*)str;
    trienode* tmp = *root;
    int len = strlen(str);

    for(int i=0;i<len;i++){
        if( tmp->children[text[i]]==NULL ){
            tmp->children[text[i]] = createNode();
        }
        tmp = tmp->children[text[i]];
    }

    if(tmp->terminal==true){
        return false;
    }else{
        tmp->terminal = true;
        return true;
    }
}

bool searchStr(trienode* root, char* str){
    trienode* tmp = root;
    int len = strlen(str);

    for(int i=0;i<len;i++){
        if(tmp->children[str[i]]==NULL){
            return false;
        }
        tmp = tmp->children[str[i]];
    }
    return tmp->terminal;
}

bool hasChildren(trienode* node) {
    for (int i = 0; i < NUM_CHAR; i++) {
        if (node->children[i] != NULL)
            return true;
    }
    return false;
}

bool deleteTrieUtil(trienode* node, char* str, int depth, bool* wordDeleted) {
    if (node == NULL)
        return false;

    if (depth == strlen(str)) {

        if (!node->terminal)
            return false;

        node->terminal = false;
        *wordDeleted = true;

        return !hasChildren(node);
    }

    unsigned char index = (unsigned char)str[depth];

    if (deleteTrieUtil(node->children[index], str, depth + 1, wordDeleted)) {
        free(node->children[index]);
        node->children[index] = NULL;

        return (!node->terminal && !hasChildren(node));
    }

    return false;
}


void deleteWord(trienode** root, char* str) {
    if (*root == NULL)
        return false;

    bool wordDeleted = false;

    deleteTrieUtil(*root, str, 0, &wordDeleted);

    if (wordDeleted)
        printf("The word is deleted\n");
    else
        printf("Word not found\n");
}




void printTrieMap(trienode* root, int level, char ch) {
    if (root == NULL) return;

    for (int i = 0; i < level; i++)
        printf("   ");

    if (level == 0)
        printf("(ROOT)\n");
    else
        printf("|-- %c%s\n", ch, root->terminal ? " (END)" : "");

    for (int i = 0; i < NUM_CHAR; i++) {
        if (root->children[i] != NULL) {
            printTrieMap(root->children[i], level + 1, (char)i);
        }
    }
}

void printtrie(trienode* root) {
    if (root == NULL) {
        printf("(ROOT) -> EMPTY\n");
        return;
    }
    printTrieMap(root, 0, '\0');
}



int main() {
    trienode* root = NULL;

    insertNode(&root,"tea");
    insertNode(&root,"test");
    insertNode(&root,"teacher");

    printf("Before Delete:\n");
    printtrie(root);

    deleteWord(&root,"test");
    printtrie(root);

    return 0;
}




