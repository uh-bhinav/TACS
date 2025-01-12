#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define ALPHABET_SIZE 26
#define MAX_RESULTS 10
#define MAX_WORD_LENGTH 100

// Trie Node Definition
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int is_end_of_word;
} TrieNode;

// Create a new Trie Node
TrieNode* createNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (!node) {
        perror("Failed to allocate memory for TrieNode");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Insert a word into the Trie
void insert(TrieNode* root, const char* word) {
    TrieNode* current = root;
    while (*word) {
        char c = tolower(*word) - 'a';
        if (c < 0 || c >= ALPHABET_SIZE) {
            word++;
            continue; // Skip non-alphabet characters
        }
        if (!current->children[c]) {
            current->children[c] = createNode();
        }
        current = current->children[c];
        word++;
    }
    current->is_end_of_word = 1;
}

// Search for a prefix in the Trie
TrieNode* searchPrefix(TrieNode* root, const char* prefix) {
    TrieNode* current = root;
    while (*prefix) {
        char c = tolower(*prefix) - 'a';
        if (c < 0 || c >= ALPHABET_SIZE || !current->children[c]) {
            return NULL;
        }
        current = current->children[c];
        prefix++;
    }
    return current;
}

// Depth-First Search to collect words
void dfs(TrieNode* node, char* prefix, int prefix_length, char results[][MAX_WORD_LENGTH], int* result_count, int max_results) {
    if (*result_count >= max_results || !node) {
        return;
    }
    if (node->is_end_of_word) {
        prefix[prefix_length] = '\0';
        strcpy(results[*result_count], prefix);
        (*result_count)++;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            prefix[prefix_length] = 'a' + i;
            dfs(node->children[i], prefix, prefix_length + 1, results, result_count, max_results);
        }
    }
}

// Autocomplete function
void autocomplete(TrieNode* root, const char* prefix, char results[][MAX_WORD_LENGTH], int* result_count) {
    TrieNode* node = searchPrefix(root, prefix);
    if (!node) {
        *result_count = 0;
        return;
    }
    char buffer[MAX_WORD_LENGTH];
    strcpy(buffer, prefix);
    dfs(node, buffer, strlen(prefix), results, result_count, MAX_RESULTS);
}

// Load words from a file into the Trie
int loadWordsFromFile(const char* filename, TrieNode* root) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("File not found. Using default words.");
        return 0;
    }
    char word[MAX_WORD_LENGTH];
    while (fgets(word, sizeof(word), file)) {
        word[strcspn(word, "\n")] = '\0'; // Remove newline character
        insert(root, word);
    }
    fclose(file);
    return 1;
}

// Free the Trie
void freeTrie(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(node->children[i]);
    }
    free(node);
}

// Main program
int main() {
    TrieNode* root = createNode();
    loadWordsFromFile("google-10000-english.txt", root); // Load words into the Trie

    char paragraph[1000] = "";
    char typing[MAX_WORD_LENGTH] = "";
    char suggestions[MAX_RESULTS][MAX_WORD_LENGTH];
    int result_count = 0;
    int selected_index = 0;

    // Initialize ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    while (1) {
        clear();
        mvprintw(0, 0, "Paragraph: %s", paragraph);
        mvprintw(1, 0, "Typing: %s", typing);

        // Get suggestions based on the current typing
        if (strlen(typing) > 0) {
            autocomplete(root, typing, suggestions, &result_count);
        }
        else {
            result_count = 0; // If no input, clear suggestions
        }

        mvprintw(3, 0, "Suggestions:");
        for (int i = 0; i < result_count; i++) {
            if (i == selected_index) {
                attron(A_REVERSE); // Highlight the selected suggestion
            }
            mvprintw(4 + i, 0, "%s", suggestions[i]);
            if (i == selected_index) {
                attroff(A_REVERSE);
            }
        }
        refresh();

        int ch = getch(); // Get user input
        if (ch == 27) { // ESC to exit
            break;
        } else if (ch == '\n') { // Enter to accept suggestion or add new word
            if (result_count > 0) {
                // Add the selected suggestion to the paragraph
                strcat(paragraph, suggestions[selected_index]);
                strcat(paragraph, " ");
                typing[0] = '\0'; // Clear typing
            } else {
                // If the word is not in the suggestions, add it to the Trie and the paragraph
                insert(root, typing);
                strcat(paragraph, typing);
                strcat(paragraph, " ");
                typing[0] = '\0'; // Clear typing
            }
        } else if (ch == KEY_BACKSPACE || ch == 127) { // Handle backspace
            int len = strlen(typing);
            if (len > 0) {
                typing[len - 1] = '\0';
            }
        } else if (ch == KEY_UP) { // Navigate up the suggestions
            if (selected_index > 0) {
                selected_index--;
            }
        } else if (ch == KEY_DOWN) { // Navigate down the suggestions
            if (selected_index < result_count - 1) {
                selected_index++;
            }
        } else if (isprint(ch)) { // Add character to typing buffer
            int len = strlen(typing);
            typing[len] = ch;
            typing[len + 1] = '\0';
        }
        // Ensure suggestions update after every key press, even for empty or short typing
        if (strlen(typing) > 0) {
        autocomplete(root, typing, suggestions, &result_count); // Refresh suggestions
        } else {
        result_count = 0; // Clear suggestions if no input
        }
    }

    endwin(); // End ncurses
    freeTrie(root); // Free the Trie
    return 0;
}
