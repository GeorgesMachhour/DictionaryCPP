#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

struct sWord {
    char data[30];
    sWord* next;
};

struct dWord {
    char data[30];
    sWord* synonym;
    sWord* antonym;
    dWord* next;
    dWord* previous;
};

struct Dictionary {
    dWord* head;
    dWord* tail;
};

void initialize(Dictionary& l) {
    l.head = nullptr;
    l.tail = nullptr;
}

void addSynonym(sWord*& head, const char* synonym) {
    sWord* newSynonym = new sWord;
    strncpy_s(newSynonym->data, synonym, sizeof(newSynonym->data));
    newSynonym->next = nullptr;

    if (!head) {
        head = newSynonym;
    }
    else {
        sWord* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newSynonym;
    }
}

void addAntonym(sWord*& head, const char* antonym) {
    sWord* newAntonym = new sWord;
    strncpy_s(newAntonym->data, antonym, sizeof(newAntonym->data));
    newAntonym->next = nullptr;

    if (!head) {
        head = newAntonym;
    }
    else {
        sWord* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newAntonym;
    }
}

void addWord(Dictionary& dictionary, const char* word, sWord* synonyms, sWord* antonyms) {
    dWord* newWord = new dWord;
    strncpy_s(newWord->data, word, sizeof(newWord->data));
    newWord->synonym = synonyms;
    newWord->antonym = antonyms;
    newWord->next = nullptr;
    newWord->previous = nullptr;

    if (!dictionary.head) {
        dictionary.head = dictionary.tail = newWord;
    }
    else {
        dictionary.tail->next = newWord;
        newWord->previous = dictionary.tail;
        dictionary.tail = newWord;
    }
}

void displayDictionary(const Dictionary& dictionary) {
    dWord* current = dictionary.head;
    while (current) {
        cout << "Word: " << current->data << "\nSynonyms: ";
        sWord* synonym = current->synonym;
        while (synonym) {
            cout << synonym->data;
            synonym = synonym->next;
            if (synonym) {
                cout << " ";
            }
        }

        cout << "\nAntonyms: ";
        sWord* antonym = current->antonym;
        while (antonym) {
            cout << antonym->data;
            antonym = antonym->next;
            if (antonym) {
                cout << " ";
            }
        }

        cout << "\n\n";
        current = current->next;
    }
}

void insertWordsFromFile(const char* filename, Dictionary& dictionary) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string word;
        getline(ss, word, ':');

        sWord* synonyms = nullptr;
        sWord* antonyms = nullptr;

        // Read synonyms
        string synonymsStr;
        getline(ss, synonymsStr, '#');
        if (!synonymsStr.empty()) {
            stringstream synonymsStream(synonymsStr);
            string synonym;
            while (getline(synonymsStream, synonym, ':')) {
                if (!synonym.empty()) {
                    addSynonym(synonyms, synonym.c_str());
                }
            }
        }

        // Read antonyms
        string antonymsStr;
        getline(ss, antonymsStr);
        if (!antonymsStr.empty()) {
            stringstream antonymsStream(antonymsStr);
            string antonym;
            while (getline(antonymsStream, antonym, '#')) {
                if (!antonym.empty()) {
                    addAntonym(antonyms, antonym.c_str());
                }
            }
        }

        addWord(dictionary, word.c_str(), synonyms, antonyms);
    }

    file.close();
}

void writeDictionaryToFile(const char* filename, const Dictionary& dictionary) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    dWord* current = dictionary.head;
    while (current) {
        outputFile << current->data;

        // Write synonyms
        sWord* synonym = current->synonym;
        if (synonym) {
            outputFile << ":";
            while (synonym) {
                outputFile << synonym->data;
                synonym = synonym->next;
                if (synonym) {
                    outputFile << ":";
                }
            }
        }

        // Write antonyms
        sWord* antonym = current->antonym;
        if (antonym) {
            outputFile << "#";
            while (antonym) {
                outputFile << antonym->data;
                antonym = antonym->next;
                if (antonym) {
                    outputFile << "#";
                }
            }
        }

        outputFile << endl;
        current = current->next;
    }

    outputFile.close();
}

void sortDictionary(Dictionary& dictionary) {
    dWord* current = dictionary.head;
    dWord* last = nullptr;
    bool swapped;

    if (current == nullptr)
        return;  // Empty dictionary

    do {
        swapped = false;
        current = dictionary.head;

        while (current->next != last) {
            int compareResult = strcmp(current->data, current->next->data);

            if (compareResult > 0) {
                // Swap nodes
                if (current->previous)
                    current->previous->next = current->next;
                else
                    dictionary.head = current->next;

                if (current->next->next)
                    current->next->next->previous = current;
                else
                    dictionary.tail = current;

                dWord* temp = current->next->next;
                current->next->next = current;
                current->next->previous = current->previous;
                current->previous = current->next;
                current->next = temp;

                swapped = true;
            }
            else {
                current = current->next;
            }
        }

        last = current;

    } while (swapped);
}

void addWordToSortedDictionary(Dictionary& dictionary, const char* word, sWord* synonyms, sWord* antonyms) {
    // Check if the word already exists in the dictionary
    dWord* current = dictionary.head;
    while (current) {
        int compareResult = strcmp(word, current->data);
        if (compareResult == 0) {
            // Word already exists, don't add it again
            cerr << "Word '" << word << "' already exists in the dictionary.\n";
            return;
        }
        else if (compareResult < 0) {
            // Found the correct position to insert the new word
            dWord* newWord = new dWord;
            strncpy_s(newWord->data, word, sizeof(newWord->data));
            newWord->synonym = synonyms;
            newWord->antonym = antonyms;
            newWord->next = current;
            newWord->previous = current->previous;

            if (current->previous) {
                current->previous->next = newWord;
            }
            else {
                dictionary.head = newWord;
            }

            current->previous = newWord;
            return;
        }

        current = current->next;
    }

    // Word should be inserted at the end of the dictionary
    dWord* newWord = new dWord;
    strncpy_s(newWord->data, word, sizeof(newWord->data));
    newWord->synonym = synonyms;
    newWord->antonym = antonyms;
    newWord->next = nullptr;
    newWord->previous = dictionary.tail;

    if (dictionary.tail) {
        dictionary.tail->next = newWord;
    }
    else {
        dictionary.head = newWord;
    }

    dictionary.tail = newWord;
}

void addWordWithRelationship(Dictionary& dictionary, const char* newWord, const char* existingWord, bool isSynonym) {
    dWord* current = dictionary.head;
    dWord* existing = nullptr;

    // Find the existing word in the dictionary
    while (current) {
        int compareResult = strcmp(existingWord, current->data);
        if (compareResult == 0) {
            existing = current;
            break;
        }
        current = current->next;
    }

    if (!existing) {
        cerr << "Existing word '" << existingWord << "' not found in the dictionary.\n";
        return;
    }

    sWord* newSynonyms = nullptr;
    sWord* newAntonyms = nullptr;

    // Add the new word to the dictionary
    

    // Collect synonyms or antonyms from the existing word and add to respective lists
    sWord* synonym = existing->synonym;
    sWord* antonym = existing->antonym;

    while (synonym) {
        // Add synonym of existing word to newSynonyms for the new word
        addSynonym(newSynonyms, synonym->data);
        synonym = synonym->next;
    }

    while (antonym) {
        // Add antonym of existing word to newAntonyms for the new word
        addAntonym(newAntonyms, antonym->data);
        antonym = antonym->next;
    }

    if (isSynonym) {
        // New word is a synonym of the existing word
        addSynonym(existing->synonym, newWord);
        addSynonym(newSynonyms, existingWord);
        addWordToSortedDictionary(dictionary, newWord, newSynonyms, newAntonyms);
    }
    else {
        // New word is an antonym of the existing word
        addAntonym(newAntonyms, existingWord);
        addAntonym(existing->antonym, newWord);
        addWordToSortedDictionary(dictionary, newWord, newAntonyms, newSynonyms);
    }
}


void deleteWordsWithLetters(Dictionary& dictionary, const char* letters) {
    dWord* current = dictionary.head;
    dWord* prev = nullptr;

    // Loop through the dictionary
    while (current != nullptr) {
        bool deleteWord = false;
        int len = 0;
        len = strlen(current->data);

        // Check if any of the letters are present in the word
        for (int i = 0; i < len; ++i) {
            if (strchr(letters, current->data[i]) != nullptr) {
                deleteWord = true;
                break;
            }
        }

        // Delete the word if it contains any of the specified letters
        if (deleteWord) {
            dWord* temp = current;

            // Update pointers
            if (prev) {
                prev->next = current->next;
            }
            else {
                dictionary.head = current->next;
            }

            current = current->next;

            if (temp == dictionary.tail) {
                dictionary.tail = prev;
            }

            delete temp;
        }
        else {
            // Move to the next word
            prev = current;
            current = current->next;
        }
    }
}

void searchWordsStartingWith(Dictionary& dictionary, const char* startingLetters) {
    dWord* current = dictionary.head;
    bool exsist = false;
    // Traverse the dictionary
    while (current != nullptr) {
        // Check if the word starts with the given set of letters
        if (strncmp(current->data, startingLetters, strlen(startingLetters)) == 0) {
            // If it matches, display the word
            exsist = true;
            cout << "Word starting with '" << startingLetters << "': " << current->data << endl;
        }
        current = current->next;
    }
    if (exsist == false)
    {
        cout << "Word starting with '" << startingLetters << "'\n";
    }
}

void searchSynonymsAndAntonyms(Dictionary& dictionary, const char* word) {
    dWord* current = dictionary.head;

    // Traverse the dictionary
    while (current != nullptr) {
        // Check if the current word matches the given word
        if (strcmp(current->data, word) == 0) {
            // If there are synonyms, display them
            sWord* synonym = current->synonym;
            if (synonym) {
                cout << "Synonyms for '" << word << "': ";
                while (synonym) {
                    cout << synonym->data << " ";
                    synonym = synonym->next;
                }
                cout << endl;
            }
            else {
                cout << "No synonyms found for '" << word << "'.\n";
            }

            // If there are antonyms, display them
            sWord* antonym = current->antonym;
            if (antonym) {
                cout << "Antonyms for '" << word << "': ";
                while (antonym) {
                    cout << antonym->data << " ";
                    antonym = antonym->next;
                }
                cout << endl;
            }
            else {
                cout << "No antonyms found for '" << word << "'.\n";
            }

            // Found the word, no need to continue searching
            return;
        }
        current = current->next;
    }

    // Word not found in the dictionary
    cout << "Word '" << word << "' not found in the dictionary.\n";
}

void deleteWordsStartingWith(Dictionary& dictionary, const char* startingLetters) {
    dWord* current = dictionary.head;
    dWord* prev = nullptr;

    // Traverse the dictionary
    while (current != nullptr) {
        // Check if the word starts with the given set of letters
        if (strncmp(current->data, startingLetters, strlen(startingLetters)) == 0) {
            dWord* temp = current;

            // Update pointers
            if (prev) {
                prev->next = current->next;
            }
            else {
                dictionary.head = current->next;
            }

            current = current->next;

            if (temp == dictionary.tail) {
                dictionary.tail = prev;
            }

            delete temp;
        }
        else {
            // Move to the next word
            prev = current;
            current = current->next;
        }
    }
}

void Start() {
    Dictionary myDictionary;
    initialize(myDictionary);
    const char* inputFilename = "dictionary.txt";
    const char* outputFilename = "output.txt";
    insertWordsFromFile(inputFilename, myDictionary);
    int choice;
    do {
        cout << "\nDictionary Menu:\n";
        cout << "1. Display Dictionary\n";
        cout << "2. Write Dictionary to File\n";
        cout << "3. Sort Dictionary\n";
        cout << "4. Add Word to Sorted Dictionary\n";
        cout << "5. Add Word with Relationship\n";
        cout << "6. Delete Words with Letters\n";
        cout << "7. Search Words Starting With\n";
        cout << "8. Search Synonyms and Antonyms\n";
        cout << "9. Delete Words Starting With\n";
        cout << "10. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "\nCurrent Dictionary:\n";
            displayDictionary(myDictionary);
            break;
        case 2: {
            writeDictionaryToFile(outputFilename, myDictionary);
            cout << "Dictionary written to file: " << outputFilename << endl;
            break;
        }
        case 3:
            sortDictionary(myDictionary);
            cout << "Dictionary sorted.\n";
            displayDictionary(myDictionary);
            break;
        case 4: {
            char newWord[30];
            cout << "Enter the new word: ";
            cin >> newWord;

            sWord* newSynonyms = nullptr;  // Replace with user input for synonyms
            sWord* newAntonyms = nullptr;  // Replace with user input for antonyms

            addWordToSortedDictionary(myDictionary, newWord, newSynonyms, newAntonyms);
            cout << "Word added to sorted dictionary.\n";
            break;
        }
        case 5: {
            char newWord[30];
            cout << "Enter the new word: ";
            cin >> newWord;

            char existingWord[30];
            cout << "Enter an existing word: ";
            cin >> existingWord;

            bool isSynonym;
            cout << "Is it a synonym (1) or antonym (0): ";
            do {
                cin >> isSynonym;
            } while (isSynonym != 0 && isSynonym != 1);

            addWordWithRelationship(myDictionary, newWord, existingWord, isSynonym);
            cout << "Word added with relationship.\n";
            break;
        }
        case 6: {

            char lettersToDelete[40];
            cout << "Enter the letters to delete words: (enter 0 when you enter all the letters you want)\n";
            for (int i = 0; i < 40; i++) {
                cin >> lettersToDelete[i];
                if (lettersToDelete[i] == '0')
                    break;
            }
            deleteWordsWithLetters(myDictionary, lettersToDelete);
            cout << "Words with specified letters deleted.\n";
            break;
        }
        case 7: {
            char startingLetters[30];
            cout << "Enter the starting letters: ";
            cin >> startingLetters;
            searchWordsStartingWith(myDictionary, startingLetters);
            break;
        }
        case 8: {
            char wordToSearch[30];
            cout << "Enter the word to search for synonyms and antonyms: ";
            cin >> wordToSearch;
            searchSynonymsAndAntonyms(myDictionary, wordToSearch);
            break;
        }
        case 9: {
            char startingLettersToDelete[30];
            cout << "Enter the starting letters to delete words: ";
            cin >> startingLettersToDelete;
            deleteWordsStartingWith(myDictionary, startingLettersToDelete);
            cout << "Words starting with specified letters deleted.\n";
            break;
        }
        case 10:
            cout << "Exiting the program.\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }

        // Clear the input buffer
        cin.ignore(INT_MAX, '\n');

    } while (choice != 10);

}
int main() {
    Start();
    return 0;
}