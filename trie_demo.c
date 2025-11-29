#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>

#define NUM_CHAR 256
#define MAX_SUGGESTIONS 10
#define MAX_WORD_LEN 100

typedef struct trie{
    struct trie* children[NUM_CHAR];
    bool terminal;
} trie;

// Console color codes
#define COLOR_RED 12
#define COLOR_GREEN 10
#define COLOR_WHITE 7
#define COLOR_YELLOW 14
#define COLOR_CYAN 11

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

trie* createNode(){
    trie* newNode = malloc(sizeof(trie));
    for(int i=0;i<NUM_CHAR;i++){
        newNode->children[i] = NULL;
    }
    newNode->terminal = false;
    return newNode;
}

bool insertNode(trie** root, char* str){
    if(*root == NULL){
        *root = createNode();
    }

    unsigned char* text = (unsigned char*)str;
    trie* tmp = *root;
    int len = strlen(str);

    for(int i=0;i<len;i++){
        if(tmp->children[text[i]]==NULL){
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

bool searchStr(trie* root, char* str){
    if(root == NULL || str == NULL || strlen(str) == 0)
        return false;
    
    trie* tmp = root;
    int len = strlen(str);

    for(int i=0;i<len;i++){
        if(tmp->children[(unsigned char)str[i]]==NULL){
            return false;
        }
        tmp = tmp->children[(unsigned char)str[i]];
    }
    return tmp->terminal;
}

// Check if a hyphenated word is valid by checking parts
bool checkHyphenatedWord(trie* root, char* str) {
    // First try the whole word
    if(searchStr(root, str))
        return true;
    
    // If it contains a hyphen, check each part
    char* hyphen = strchr(str, '-');
    if(hyphen != NULL) {
        char part1[MAX_WORD_LEN] = {0};
        char part2[MAX_WORD_LEN] = {0};
        
        int part1Len = hyphen - str;
        strncpy(part1, str, part1Len);
        part1[part1Len] = '\0';
        
        strcpy(part2, hyphen + 1);
        
        // If both parts are valid words, consider the hyphenated word valid
        if(searchStr(root, part1) && searchStr(root, part2)) {
            return true;
        }
    }
    
    return false;
}

void insertWords(trie* currRoot, char* buffer, int depth, char res[][MAX_WORD_LEN], int* ri, int maxSuggestions){
    if(*ri >= maxSuggestions)
        return;

    if(currRoot->terminal){
        buffer[depth] = '\0';
        strcpy(res[*ri], buffer);
        (*ri)++;
    }

    for(int i=0;i<NUM_CHAR && *ri < maxSuggestions;i++){
        if(currRoot->children[i]!=NULL){
            buffer[depth] = (char)i;
            insertWords(currRoot->children[i], buffer, depth+1, res, ri, maxSuggestions);
        }
    }
}

void autocomplete(char res[][MAX_WORD_LEN], char* prefix, trie* root, int* ri, int maxSuggestions){
    *ri = 0;
    if(root == NULL || prefix == NULL || strlen(prefix) == 0)
        return;

    trie* tmp = root;
    int len = strlen(prefix);

    for(int i=0;i<len;i++){
        if(tmp->children[(unsigned char)prefix[i]]==NULL)
            return;
        tmp = tmp->children[(unsigned char)prefix[i]];
    }

    char buffer[MAX_WORD_LEN];
    strcpy(buffer, prefix);
    insertWords(tmp, buffer, len, res, ri, maxSuggestions);
}

int loadDictionary(trie** root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if(file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char word[MAX_WORD_LEN];
    int count = 0;
    
    printf("Loading dictionary");
    fflush(stdout);
    
    while(fgets(word, sizeof(word), file)) {
        // Remove newline
        word[strcspn(word, "\n\r")] = 0;
        
        // Convert to lowercase
        for(int i = 0; word[i]; i++){
            word[i] = tolower(word[i]);
        }
        
        if(strlen(word) > 0) {
            insertNode(root, word);
            count++;
            
            if(count % 50000 == 0) {
                printf(".");
                fflush(stdout);
            }
        }
    }
    
    fclose(file);
    printf("\n");
    return count;
}

void clearScreen() {
    system("cls");
}

void displayHeader() {
    setColor(COLOR_CYAN);
    printf("================================================================================\n");
    printf("||           TRIE DATA STRUCTURE                                            ||\n");
    printf("||           Autocomplete & Spell Checker                                   ||\n");
    printf("================================================================================\n");
    setColor(COLOR_WHITE);
    printf("\n");
    setColor(COLOR_YELLOW);
    printf("Instructions:\n");
    setColor(COLOR_WHITE);
    printf("  * Type your text naturally, word by word\n");
    printf("  * ");
    setColor(COLOR_GREEN);
    printf("Green text");
    setColor(COLOR_WHITE);
    printf(" = correctly spelled words, ");
    setColor(COLOR_RED);
    printf("Red text");
    setColor(COLOR_WHITE);
    printf(" = misspelled\n");
    printf("  * As you type, autocomplete suggestions appear below\n");
    printf("  * Press ");
    setColor(COLOR_CYAN);
    printf("TAB");
    setColor(COLOR_WHITE);
    printf(" repeatedly to cycle through suggestions (they replace as you cycle)\n");
    printf("  * Press ");
    setColor(COLOR_YELLOW);
    printf("ENTER");
    setColor(COLOR_WHITE);
    printf(" to accept current word and move to next\n");
    printf("  * Press ");
    setColor(COLOR_YELLOW);
    printf("SPACE");
    setColor(COLOR_WHITE);
    printf(" to accept and add space\n");
    printf("  * Press ");
    setColor(COLOR_RED);
    printf("ESC");
    setColor(COLOR_WHITE);
    printf(" to exit, ");
    setColor(COLOR_YELLOW);
    printf("BACKSPACE");
    setColor(COLOR_WHITE);
    printf(" to delete\n");
    setColor(COLOR_CYAN);
    printf("--------------------------------------------------------------------------------\n");
    setColor(COLOR_WHITE);
    printf("\n");
}

void displaySuggestions(char suggestions[][MAX_WORD_LEN], int count, int selectedIndex) {
    // Clear the suggestion area (lines 20-30 for up to 10 suggestions)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    DWORD written;
    
    for(int line = 20; line < 31; line++) {
        coord.X = 0;
        coord.Y = line;
        SetConsoleCursorPosition(hConsole, coord);
        FillConsoleOutputCharacter(hConsole, ' ', 120, coord, &written);
    }
    
    // Position cursor for suggestions header
    coord.X = 0;
    coord.Y = 20;
    SetConsoleCursorPosition(hConsole, coord);
    
    if(count > 0) {
        setColor(COLOR_YELLOW);
        printf("\nAutocomplete Suggestions (TAB to cycle, SPACE/ENTER to accept):\n");
        setColor(COLOR_WHITE);
        
        // Display each suggestion on its own line
        for(int i = 0; i < count && i < MAX_SUGGESTIONS; i++) {
            if(i == selectedIndex) {
                // Highlight selected suggestion
                setColor(COLOR_CYAN);
                printf("  >>> [%d] %s\n", i+1, suggestions[i]);
                setColor(COLOR_WHITE);
            } else {
                setColor(COLOR_GREEN);
                printf("      [%d] %s\n", i+1, suggestions[i]);
                setColor(COLOR_WHITE);
            }
        }
    }
}

void clearLines(int startY, int numLines) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    DWORD written;
    
    for(int i = 0; i < numLines; i++) {
        coord.X = 0;
        coord.Y = startY + i;
        SetConsoleCursorPosition(hConsole, coord);
        FillConsoleOutputCharacter(hConsole, ' ', 120, coord, &written);  // Increased to 120 to clear everything
    }
}

void redrawText(char* text, int cursorPos, trie* root, char* currentWord, int wordStart) {
    // Clear from line 13 onwards (20 lines)
    clearLines(13, 20);
    
    // Position cursor at start of text area
    COORD coord;
    coord.X = 0;
    coord.Y = 13;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    
    printf("Your text:\n");
    setColor(COLOR_WHITE);
    printf("================================================================================\n|| ");
    
    // Display text with color coding
    int i = 0;
    
    while(i < cursorPos) {
        // Check if this is a whitespace
        if(text[i] == ' ' || text[i] == '\n' || text[i] == '\t') {
            printf("%c", text[i]);
            i++;
        }
        // Check if this is punctuation or special char
        else if(strchr(".,!?;:\"'()[]{}@#$%^&*+-=/<>|\\~`", text[i]) != NULL) {
            // Print punctuation in white
            setColor(COLOR_WHITE);
            printf("%c", text[i]);
            i++;
        }
        else {
            // Start of a word - collect it
            char word[MAX_WORD_LEN] = {0};
            int wordLen = 0;
            int wordStartIdx = i;
            
            // Collect alphanumeric characters and hyphens/apostrophes
            while(i < cursorPos && 
                  ((text[i] >= 'a' && text[i] <= 'z') || 
                   (text[i] >= 'A' && text[i] <= 'Z') || 
                   (text[i] >= '0' && text[i] <= '9') ||
                   text[i] == '-' || text[i] == '\'')) {
                word[wordLen++] = tolower(text[i]);
                i++;
            }
            
            word[wordLen] = '\0';
            
            // Check spelling (use hyphenated word checker)
            bool isCorrect = checkHyphenatedWord(root, word);
            
            if(isCorrect) {
                setColor(COLOR_GREEN);
            } else {
                setColor(COLOR_RED);
            }
            
            // Print the word in original case from text buffer
            for(int j = 0; j < wordLen; j++) {
                printf("%c", text[wordStartIdx + j]);
            }
            
            setColor(COLOR_WHITE);
        }
    }
    
    // Print cursor
    setColor(COLOR_YELLOW);
    printf("_");
    setColor(COLOR_WHITE);
    
    printf("\n");
    printf("================================================================================");
    
    // Display current word being typed and its status
    printf("\n");
    if(strlen(currentWord) > 0) {
        bool isCorrect = checkHyphenatedWord(root, currentWord);
        printf("\nCurrent word: \"");
        if(isCorrect) {
            setColor(COLOR_GREEN);
            printf("%s", currentWord);
            setColor(COLOR_WHITE);
            printf("\" [OK] ");
            setColor(COLOR_GREEN);
            printf("Correct spelling");
        } else {
            setColor(COLOR_RED);
            printf("%s", currentWord);
            setColor(COLOR_WHITE);
            printf("\" [X] ");
            setColor(COLOR_RED);
            printf("Not in dictionary");
        }
        setColor(COLOR_WHITE);
        printf("                    ");
    } else {
        printf("                                                                               ");
    }
}

int main() {
    trie* root = NULL;
    
    clearScreen();
    setColor(COLOR_CYAN);
    printf("================================================================================\n");
    printf("||                   INITIALIZING TRIE SPELL CHECKER                         ||\n");
    printf("================================================================================\n");
    setColor(COLOR_WHITE);
    printf("\n");
    
    // Load dictionary
    int wordCount = loadDictionary(&root, "words_alpha.txt");
    
    if(wordCount == 0) {
        setColor(COLOR_RED);
        printf("\nFailed to load dictionary! Please ensure words_alpha.txt is in the current directory.\n");
        setColor(COLOR_WHITE);
        printf("Press any key to exit...");
        getch();
        return 1;
    }
    
    setColor(COLOR_GREEN);
    printf("\n[OK] Successfully loaded %d words into the trie!\n", wordCount);
    setColor(COLOR_WHITE);
    printf("\nPress any key to start...");
    getch();
    
    // Main interface
    clearScreen();
    displayHeader();
    
    char text[10000] = {0};
    char currentWord[MAX_WORD_LEN] = {0};
    char suggestions[MAX_SUGGESTIONS][MAX_WORD_LEN];
    int textPos = 0;
    int wordStart = 0;
    int suggestionCount = 0;
    int selectedSuggestion = 0;
    
    printf("Your text:\n");
    setColor(COLOR_WHITE);
    printf("================================================================================\n");
    printf("|| ");
    setColor(COLOR_YELLOW);
    printf("_");
    setColor(COLOR_WHITE);
    for(int i = 0; i < 77; i++) printf(" ");
    printf("||\n");
    printf("================================================================================");
    
    while(1) {
        char c = getch();
        
        if(c == 27) { // ESC
            clearScreen();
            setColor(COLOR_CYAN);
            printf("\n\n  Thank you for using the Trie Spell Checker!\n");
            printf("  This demonstrates the power of Trie data structure for:\n");
            setColor(COLOR_WHITE);
            printf("    * Fast word lookup (spell checking)\n");
            printf("    * Prefix-based autocomplete\n");
            printf("    * Efficient dictionary storage\n\n");
            break;
        }
        else if(c == 8) { // BACKSPACE
            if(textPos > 0) {
                textPos--;
                text[textPos] = '\0';
                
                // Update current word
                int i = textPos - 1;
                while(i >= 0 && text[i] != ' ' && text[i] != '\n' && text[i] != '\t') {
                    i--;
                }
                wordStart = i + 1;
                
                int wordLen = 0;
                for(int j = wordStart; j < textPos; j++) {
                    currentWord[wordLen++] = tolower(text[j]);
                }
                currentWord[wordLen] = '\0';
                
                // Get suggestions for updated word
                if(strlen(currentWord) > 0) {
                    autocomplete(suggestions, currentWord, root, &suggestionCount, MAX_SUGGESTIONS);
                    selectedSuggestion = 0;
                } else {
                    suggestionCount = 0;
                }
                
                redrawText(text, textPos, root, currentWord, wordStart);
                displaySuggestions(suggestions, suggestionCount, selectedSuggestion);
            }
        }
        else if(c == 9) { // TAB - cycle through suggestions
            if(suggestionCount > 0) {
                // Move to next suggestion
                selectedSuggestion = (selectedSuggestion + 1) % suggestionCount;
                
                // Remove current word
                textPos = wordStart;
                
                // Insert selected suggestion
                strcpy(&text[textPos], suggestions[selectedSuggestion]);
                textPos += strlen(suggestions[selectedSuggestion]);
                
                // Update current word to match suggestion
                strcpy(currentWord, suggestions[selectedSuggestion]);
                
                redrawText(text, textPos, root, currentWord, wordStart);
                displaySuggestions(suggestions, suggestionCount, selectedSuggestion);
            }
        }
        else if(c == 13) { // ENTER - accept current selection and add space
            if(suggestionCount > 0) {
                // Accept current selection
                text[textPos++] = ' ';
                text[textPos] = '\0';
                
                currentWord[0] = '\0';
                selectedSuggestion = 0;
                suggestionCount = 0;
                wordStart = textPos;
                
                redrawText(text, textPos, root, "", textPos);
                displaySuggestions(suggestions, 0, 0);
            } else {
                // Just add newline/space behavior
                text[textPos++] = ' ';
                text[textPos] = '\0';
                currentWord[0] = '\0';
                wordStart = textPos;
                redrawText(text, textPos, root, "", textPos);
            }
        }
        else if(c == ' ' || c == '\n' || c == '\t') { // Word boundary
            text[textPos++] = c;
            text[textPos] = '\0';
            
            currentWord[0] = '\0';
            selectedSuggestion = 0;
            wordStart = textPos;
            suggestionCount = 0;
            
            redrawText(text, textPos, root, "", textPos);
            displaySuggestions(suggestions, 0, 0);
        }
        else if(c >= 32 && c <= 126) { // Printable character
            // Check if it's a punctuation mark that ends a word
            if(strchr(".,!?;:\"'", c) != NULL) {
                // End current word first if there is one
                if(strlen(currentWord) > 0) {
                    // Get suggestions for current word before adding punctuation
                    autocomplete(suggestions, currentWord, root, &suggestionCount, MAX_SUGGESTIONS);
                }
                
                // Add the punctuation
                text[textPos++] = c;
                text[textPos] = '\0';
                
                // Reset word tracking
                currentWord[0] = '\0';
                selectedSuggestion = 0;
                wordStart = textPos;
                
                redrawText(text, textPos, root, "", textPos);
                
                if(suggestionCount > 0) {
                    displaySuggestions(suggestions, suggestionCount, 0);
                } else {
                    displaySuggestions(suggestions, 0, 0);
                }
                
                suggestionCount = 0;
            }
            // Check if it's alphanumeric (part of a word)
            else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
                    (c >= '0' && c <= '9') || c == '-' || c == '\'') {
                text[textPos++] = c;
                text[textPos] = '\0';
                
                // Update current word
                int wordLen = 0;
                for(int i = wordStart; i < textPos; i++) {
                    currentWord[wordLen++] = tolower(text[i]);
                }
                currentWord[wordLen] = '\0';
                
                // Get autocomplete suggestions
                if(strlen(currentWord) > 0) {
                    autocomplete(suggestions, currentWord, root, &suggestionCount, MAX_SUGGESTIONS);
                    selectedSuggestion = 0;
                }
                
                redrawText(text, textPos, root, currentWord, wordStart);
                
                if(suggestionCount > 0) {
                    displaySuggestions(suggestions, suggestionCount, selectedSuggestion);
                } else {
                    displaySuggestions(suggestions, 0, 0);
                }
            }
            // Other printable characters (treat as word separators)
            else {
                text[textPos++] = c;
                text[textPos] = '\0';
                
                currentWord[0] = '\0';
                selectedSuggestion = 0;
                wordStart = textPos;
                suggestionCount = 0;
                
                redrawText(text, textPos, root, "", textPos);
                displaySuggestions(suggestions, 0, 0);
            }
        }
    }
    
    return 0;
}
