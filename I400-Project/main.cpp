#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <limits>
#include <map>

// Utility functions for input handling
void handleInputError() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Invalid input. Please try again.\n";
    }
}

template <typename T>
T getValidInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (!std::cin.fail()) {
            return value;
        }
        handleInputError();
    }
}

std::string getValidString(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input); 
        if (!input.empty()) {
            return input;
        }
        std::cerr << "Input cannot be empty. Please try again.\n";
    }
}

using Transitions = std::map<std::pair<int, char>, int>;

struct Automaton {
    int id;
    std::set<int> states;
    std::set<char> alphabet;
    Transitions transitions;
    int initialState;
    std::set<int> statesTerminal;
};

using Automatons = std::vector<Automaton>;
Automatons automatons;

void addAutomaton(const Automaton& automaton) {
    automatons.push_back(automaton);
}

void deleteAutomaton(int id) {
    automatons.erase(std::remove_if(automatons.begin(), automatons.end(), [id](const Automaton& automaton) { return automaton.id == id; }), automatons.end());
}

Automaton* findAutomaton(int id) {
    auto it = std::find_if(automatons.begin(), automatons.end(),
        [id](const Automaton& a) { return a.id == id; });
    return (it != automatons.end()) ? &(*it) : nullptr;
}

Automaton createPredefinedAutomaton() {
    Automaton automaton;
    automaton.id = 0; // Predefined ID

    // Alphabet: Letters, digits, and special symbols
    for (char c = 'a'; c <= 'z'; ++c) automaton.alphabet.insert(c);
    for (char c = 'A'; c <= 'Z'; ++c) automaton.alphabet.insert(c);
    for (char c = '0'; c <= '9'; ++c) automaton.alphabet.insert(c);
    automaton.alphabet.insert('=');
    automaton.alphabet.insert('+');
    automaton.alphabet.insert('-');
    automaton.alphabet.insert('*');
    automaton.alphabet.insert('/');
    automaton.alphabet.insert(';');
    automaton.alphabet.insert(' '); // Space for separation

    // States: 0 (start), 1 (identifier), 2 (integer), 3 (separator)
    automaton.states = { 0, 1, 2, 3 };
    automaton.initialState = 0;
    automaton.statesTerminal = { 1, 2, 3 };

    // General transitions
    for (char c = 'a'; c <= 'z'; ++c) automaton.transitions[{0, c}] = 1;
    for (char c = 'A'; c <= 'Z'; ++c) automaton.transitions[{0, c}] = 1;
    for (char c = '0'; c <= '9'; ++c) automaton.transitions[{0, c}] = 2;
    automaton.transitions[{2, '.'}] = 2;
    automaton.transitions[{0, '.'}] = 2;

    automaton.transitions[{0, '='}] = 3;
    automaton.transitions[{0, '+'}] = 3;
    automaton.transitions[{0, '-'}] = 3;
    automaton.transitions[{0, '*'}] = 3;
    automaton.transitions[{0, '/'}] = 3;
    automaton.transitions[{0, ';'}] = 3;

    // Allow multi-character identifiers
    for (char c = 'a'; c <= 'z'; ++c) automaton.transitions[{1, c}] = 1;
    for (char c = 'A'; c <= 'Z'; ++c) automaton.transitions[{1, c}] = 1;
    for (char c = '0'; c <= '9'; ++c) automaton.transitions[{1, c}] = 1;

    // Allow multi-digit numbers
    for (char c = '0'; c <= '9'; ++c) automaton.transitions[{2, c}] = 2;

    return automaton;
}

std::vector<std::string> tokenize(const std::string& program, const Automaton& automaton) {
    std::vector<std::string> tokens;
    int currentState = automaton.initialState;
    std::string currentToken;

    for (size_t i = 0; i < program.size(); ++i) {
        char ch = program[i];
        auto transition = automaton.transitions.find({ currentState, ch });

        if (transition != automaton.transitions.end()) {
            currentState = transition->second;
            currentToken += ch;
        }
        else {
            if (automaton.statesTerminal.count(currentState)) {
                if (currentState == 1) {
                    if (currentToken == "int" || currentToken == "float" || currentToken == "double" || currentToken == "long") {
                        tokens.push_back("KEYWORD");
                    }
                    else {
                        tokens.push_back("ID");
                    }
                }
                else if (currentState == 2) {
                    tokens.push_back("NUMBER");
                }
                else if (currentState == 3) {
                    tokens.push_back("SEP");
                }

                currentToken.clear();
                currentState = automaton.initialState;

                // Process the current character again in the next iteration (for cases where there are no spaces to seperate)
                --i;
            }
        }
    }

    // Process last token
    if (!currentToken.empty() && automaton.statesTerminal.count(currentState)) {
        if (currentState == 1) {
            if (currentToken == "int" || currentToken == "float" || currentToken == "double" || currentToken == "long") {
                tokens.push_back("KEYWORD");
            }
            else {
                tokens.push_back("ID");
            }
        }
        else if (currentState == 2) {
            tokens.push_back("NUMBER");
        }
        else if (currentState == 3) {
            tokens.push_back("SEP");
        }
    }

    return tokens;
}

int main() {
    int selectedMenu;
    int nextAutomatonId = 1; // To assign unique IDs to automatons for cases where we don't have spaces to seperate

    Automaton predefinedAutomaton = createPredefinedAutomaton();
    addAutomaton(predefinedAutomaton);

    do {
        std::cout << "\nPlease pick an action:\n"
            << "1- Add an Automaton\n"
            << "2- Delete an Automaton\n"
            << "3- Lexical Analysis\n"
            << "4- Quit\n";
        selectedMenu = getValidInput<int>("");

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (selectedMenu) {
        case 1: {
            std::cout << "1- Adding an automaton\n";
            Automaton automaton;
            automaton.id = nextAutomatonId++;

            // Input alphabet
            std::cout << "Input the alphabet, 0 when done:\n";
            char letter;
            while ((letter = getValidInput<char>("")) != '0') {
                automaton.alphabet.insert(letter);
            }

            // Input states
            std::cout << "Input the states (as integers), -1 when done:\n";
            int state;
            while ((state = getValidInput<int>("")) != -1) {
                automaton.states.insert(state);
            }

            // Input initial state
            automaton.initialState = getValidInput<int>("Enter the initial state:\n");
            while (automaton.states.find(automaton.initialState) == automaton.states.end()) {
                std::cerr << "Invalid State. Try again.\n";
                automaton.initialState = getValidInput<int>("Enter the initial state:\n");
            }

            // Input terminal states
            std::cout << "Input the terminal states, -1 when done:\n";
            int terminalState;
            while ((terminalState = getValidInput<int>("")) != -1) {
                if (automaton.states.find(terminalState) != automaton.states.end()) {
                    automaton.statesTerminal.insert(terminalState);
                }
                else {
                    std::cerr << "Invalid State. Try again.\n";
                }
            }

            // Input transitions
            int n = getValidInput<int>("Enter the number of transitions:\n");
            for (int i = 0; i < n; i++) {
                std::cout << "\nInput the transition\n";
                int origin = getValidInput<int>("Origin: ");
                char symbol = getValidInput<char>("Symbol: ");
                int target = getValidInput<int>("Target: ");

                if (automaton.states.find(origin) != automaton.states.end() &&
                    automaton.states.find(target) != automaton.states.end() &&
                    automaton.alphabet.find(symbol) != automaton.alphabet.end()) {
                    automaton.transitions.insert({ {origin, symbol}, target });
                }
                else {
                    std::cerr << "Invalid Transition. Try again.\n";
                    i--;
                }
            }

            addAutomaton(automaton);
            std::cout << "Automaton added successfully with ID: " << automaton.id << "\n";
            break;
        }

        case 2: {
            std::cout << "2- Deleting an automaton\n";
            int id = getValidInput<int>("Enter the ID of the automaton to delete: ");
            deleteAutomaton(id);
            std::cout << "Automaton deleted successfully.\n";
            break;
        }

        case 3: {
            std::cout << "3- Lexical Analysis\n";
            if (automatons.empty()) {
                std::cout << "No automatons available. Please add an automaton first.\n";
                break;
            }

            // Use the predefined Automaton for tokenization
            Automaton* chosenAutomaton = findAutomaton(0);
            if (!chosenAutomaton) {
                std::cout << "Predefined Automaton not found.\n";
                break;
            }

            std::string program = getValidString("Enter the program to be analyzed:\n");
            std::vector<std::string> tokens = tokenize(program, *chosenAutomaton);

            std::cout << "Tokens:\n";
            for (const auto& token : tokens) {
                std::cout << token << " ";
            }
            std::cout << std::endl;
            break;
        }

        case 4:
            std::cout << "Exiting...\n";
            break;

        default:
            std::cerr << "Invalid option. Please try again.\n";
        }
    } while (selectedMenu != 4);

    return 0;
}
