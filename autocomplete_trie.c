#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
