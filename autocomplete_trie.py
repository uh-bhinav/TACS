class TrieNode:
    def __init__(self):
        self.children = {}
        self.is_end_of_word = False


class Trie:
    def __init__(self):
        self.root = TrieNode()

    def insert(self, word):
        word = word.lower()
        node = self.root
        for char in word:
            if char not in node.children:
                node.children[char] = TrieNode()
            node = node.children[char]
        node.is_end_of_word = True

    def search_prefix(self, prefix):
        prefix = prefix.lower()
        node = self.root
        for char in prefix:
            if char not in node.children:
                return None
            node = node.children[char]
        return node

    def autocomplete(self, prefix, max_results=10):
        prefix = prefix.lower()
        results = []
        node = self.search_prefix(prefix)
        if not node:
            return []
        self.dfs(node, prefix, results, max_results)
        return results

    def dfs(self, node, prefix, results, max_results):
        if len(results) >= max_results:
            return
        if node.is_end_of_word:
            results.append(prefix)
        for char, child_node in node.children.items():
            self.dfs(child_node, prefix + char, results, max_results)


def load_words_from_file(filename):
    try:
        with open(filename, 'r') as file:
            return [line.strip().lower() for line in file if line.strip()]
    except FileNotFoundError:
        print(f"File '{filename}' not found. Using default sample words.")
        return [
            "apple", "application", "apply", "appliance",
            "banana", "band", "banner",
            "cat", "caterpillar", "cattle",
            "dog", "dodge", "dolphin"
        ]


if __name__ == "__main__":
    words = load_words_from_file('google-10000-english.txt')
    trie = Trie()

    for word in words:
        trie.insert(word)

    print("Trie has been populated with words.")

    while True:
        command = input("\nEnter a prefix, 'reload' to reload words, or 'exit' to quit: ").strip()
        if command.lower() == "exit":
            print("Exiting the program. Goodbye!")
            break
        elif command.lower() == "reload":
            words = load_words_from_file('google-10000-english.txt')
            trie = Trie()
            for word in words:
                trie.insert(word)
            print("Words have been reloaded into the Trie.")
        elif command:
            suggestions = trie.autocomplete(command, max_results=10)
            if suggestions:
                print("Suggestions:", suggestions)
            else:
                print("No suggestions found.")
        else:
            print("Please enter a valid command.")
