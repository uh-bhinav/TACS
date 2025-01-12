#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "autocomplete_trie.c"  // Include your autocomplete_trie.c for trie-related functions

#define MAX_SUGGESTIONS 10

void display_suggestions(WINDOW *win, char results[][MAX_WORD_LENGTH], int result_count) {
    werase(win);  // Clear the window
    box(win, 0, 0);  // Draw border around the window

    mvwprintw(win, 1, 1, "Autocomplete Suggestions:");

    for (int i = 0; i < result_count && i < MAX_SUGGESTIONS; i++) {
        mvwprintw(win, i + 2, 1, "- %s", results[i]);
    }

    wrefresh(win);  // Refresh the window to display the changes
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    
    // Create a window for displaying suggestions
    WINDOW *suggestions_win = newwin(15, 50, 5, 5);
    
    // Create the Trie
    TrieNode* trie = createNode();
    loadWordsFromFile("google-10000-english.txt", trie);  // Load words from file

    char command[MAX_WORD_LENGTH];
    char results[MAX_RESULTS][MAX_WORD_LENGTH];
    int result_count = 0;
    
    while (1) {
        // Get user input with ncurses
        mvprintw(0, 0, "Enter a prefix, 'reload' to reload words, or 'exit' to quit: ");
        refresh();
        
        // Clear the command buffer and get the user input
        echo();
        nocbreak();
        mvgetstr(1, 0, command);  // Get input from user
        noecho();
        cbreak();
        
        command[strcspn(command, "\n")] = '\0';  // Remove newline character

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "reload") == 0) {
            freeTrie(trie);
            trie = createNode();
            loadWordsFromFile("google-10000-english.txt", trie);
            mvprintw(2, 0, "Words reloaded.\n");
            refresh();
        } else if (*command) {
            autocomplete(trie, command, results, &result_count);
            display_suggestions(suggestions_win, results, result_count);
        } else {
            mvprintw(2, 0, "Please enter a valid command.\n");
            refresh();
        }
    }

    // Clean up ncurses
    freeTrie(trie);
    endwin();

    return 0;
}


