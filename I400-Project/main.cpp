#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <limits>

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
        std::cin >> input;
        if (!std::cin.fail() && !input.empty()) {
            return input;
        }
        handleInputError();
    }
}

// Automaton structures
struct Transition {
    Transition(std::string origin, char symbol, std::string target)
        : origin(origin), symbol(symbol), target(target) {}
    std::string origin;
    char symbol;
    std::string target;

    bool operator<(const Transition& other) const {
        return std::tie(origin, symbol, target) < std::tie(other.origin, other.symbol, other.target);
    }
};

struct Automaton {
    std::set<char> alphabet;
    std::set<std::string> states;
    std::string initialState;
    std::set<std::string> statesTerminal;
    std::set<Transition> transitions;
};

using Automatons = std::unordered_map<std::string, std::unique_ptr<Automaton>>;
Automatons automatons;
const std::unordered_set<int> ValidMenus = { 1, 2, 3 };

bool isValidState(const std::string& state, const std::set<std::string>& states) {
    return states.count(state) > 0;
}

bool isValidSymbol(char letter, const std::set<char>& alphabet) {
    return alphabet.count(letter) > 0;
}

int main() {
    int selectedMenu;
    do
    {
        selectedMenu = getValidInput<int>("Please pick an action:\n1-Add an Automaton\n2-Delete an Automaton\n3-Lexical analysis (prototype)\n4-quit\n");

        switch (selectedMenu) {

        case 1: {
            std::cout << "1- Adding an automaton\n";
            auto automaton = std::make_unique<Automaton>();

            std::cout << "Input the alphabet, 0 when done:\n";
            char letter;
            while ((letter = getValidInput<char>("")) != '0') {
                automaton->alphabet.insert(letter);
            }

            std::cout << "Input the states, 'finished' when done:\n";
            std::string state;
            while ((state = getValidString("")) != "finished") {
                automaton->states.insert(state);
            }

            automaton->initialState = getValidString("Enter the initial state:\n");
            while (!isValidState(automaton->initialState, automaton->states)) {
                std::cerr << "Invalid State. Try again.\n";
                automaton->initialState = getValidString("Enter the initial state:\n");
            }

            std::cout << "Input the terminal states, 'finished' when done:\n";
            std::string terminalState;
            while ((terminalState = getValidString("")) != "finished") {
                if (isValidState(terminalState, automaton->states)) {
                    automaton->statesTerminal.insert(terminalState);
                }
                else {
                    std::cerr << "Invalid State. Try again.\n";
                }
            }

            int n = getValidInput<int>("Enter the number of transitions:\n");
            while (n <= 0) {
                std::cerr << "Number must be greater than 0. Try again.\n";
                n = getValidInput<int>("Enter the number of transitions:\n");
            }

            for (int i = 0; i < n; i++) {
                std::cout << "\nInput the transition\n";
                std::string origin = getValidString("Origin: ");
                char symbol = getValidInput<char>("Symbol: ");
                std::string target = getValidString("Target: ");

                if (!isValidState(origin, automaton->states) ||
                    !isValidState(target, automaton->states) ||
                    !isValidSymbol(symbol, automaton->alphabet)) {
                    std::cerr << "Invalid Transition. Try again.\n";
                    n++;
                    continue;
                }
                if (!automaton->transitions.insert(Transition(origin, symbol, target)).second) {
                    std::cerr << "Duplicate Transition. Try again.\n";
                    n++;
                }
            }

            std::string name = getValidString("Enter a name for your new automaton: ");
            automatons[name] = std::move(automaton);
            break;
        }
        case 2: {
            std::cout << "2- Deleting an automaton\n";
            std::string name = getValidString("Enter the name of the automaton to delete: ");
            if (automatons.erase(name)) {
                std::cout << "Automaton deleted successfully.\n";
            }
            else {
                std::cout << "Automaton not found.\n";
            }
            break;
        }

        case 3: {
            std::cout << "Feature not ready yet!";
            break;
        }

        }
    } while (selectedMenu != 4);
    return 0;
}
