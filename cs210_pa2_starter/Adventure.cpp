#include "Adventure.h"
#include <iostream>

// ===================== GIVEN HELPERS (do not change) =====================

// Reads the next line that is not blank.
// Spaces at the start of the line are removed.
// Returns false when the file has no more lines.
bool Adventure::nextLine(std::ifstream& in, std::string& line) {
    std::string raw;
    while (std::getline(in, raw)) {
        int start = 0;
        while (start < (int)raw.size() && (raw[start] == ' ' || raw[start] == '\t')) {
            start++;
        }
        line = raw.substr(start);
        // Files saved on Windows can end each line with an extra '\r'.
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line = line.substr(0, line.size() - 1);
        }
        if (!line.empty()) {
            return true;
        }
    }
    return false;
}

// Reads the next line and checks that it starts with keyword.
// Example: for the line "HP -2" and keyword "HP", value becomes "-2".
// Prints an error and returns false if the line is missing or wrong.
bool Adventure::readField(std::ifstream& in, const std::string& keyword, std::string& value) {
    std::string line;
    if (!nextLine(in, line)) {
        std::cout << "Error: expected a " << keyword << " line but the file ended." << std::endl;
        return false;
    }
    std::string prefix = keyword + " ";
    if (line.substr(0, prefix.size()) != prefix) {
        std::cout << "Error: expected a " << keyword << " line but found: " << line << std::endl;
        return false;
    }
    value = line.substr(prefix.size());
    return true;
}

// Adds one choice number to a path.
// extendPath("", 2) gives "2". extendPath("2-1", 3) gives "2-1-3".
std::string Adventure::extendPath(const std::string& path, int choiceNumber) const {
    if (path.empty()) {
        return std::to_string(choiceNumber);
    }
    return path + "-" + std::to_string(choiceNumber);
}

// ===================== LOADING =====================

// TODO 10: open the story file and build the tree.
//   - If the file will not open, print exactly:
//       Error: could not open file <fileName>
//     and return false.
//   - Read the START_HP line and save the number in startHp_.
//     (std::stoi turns "10" into 10.)
//   - Build the whole story with readScene. The first scene's label is "START".
//   - If anything failed, return false. Otherwise give the story to tree_.
bool Adventure::load(const std::string& fileName) {
    std::ifstream in(fileName);                                                     // loads file as in
    if (!in.is_open()) {                                                            // if in cannot open print error
        std::cout << "Error: could not open file " << fileName << std::endl;             // and return false
        return false;
    }

    std::string line;                                                               // string variable under name line
    if (!readField(in, "START_HP", line)) {                   // if "START_HP" doesn't exist in in
        return false;                                                               // return false
    }
    startHp_ = std::stoi(line);                                                     // stores string value following "START_HP" into an int startHp_

    Scene<Moment>* root = readScene(in, "START");                        // first scene = root = "START" in file
    if (!root) {                                                                    // if "START" does not exsist in in
        return false;                                                               // return false
    }
    tree_.setRoot(root);                                                            // creates tree with set root
    return true;                                                                    // returns true for successful load and build
}                                                                       // end of method

// TODO 11: read ONE scene and everything below it. Return it.
//   - Read SCENE, HP, LOOT, ITEM, and CHOICES with readField, in that order.
//   - If CHOICES is below 0 or above Scene<Moment>::MAX_CHOICES, print exactly:
//       Error: scene "<scene text>" has <n> choices. The limit is 3.
//     and return nullptr.
//   - Each choice is a LABEL line followed by a whole scene.
//   - If anything goes wrong at any depth, return nullptr
//     and make sure nothing you already built is leaked.
Scene<Moment>* Adventure::readScene(std::ifstream& in, const std::string& label) {
    std::string fullScene;                                          // reads scene returns null if none
    if (!readField(in, "SCENE", fullScene)) {
        return nullptr;
    }

    std::string hp;                                                 // reads hp returns null if none
    if (!readField(in, "HP", hp)) {
        return nullptr;
    }
    int hps = std::stoi(hp);                                        // stores value in an int


    std::string loot;                                               // reads loot returns null if none
    if (!readField(in, "LOOT", loot)) {
        return nullptr;
    }
    int loots = std::stoi(loot);                                    // stores value in an int

    std::string item;                                               // reads item returns null if none
    if (!readField(in, "ITEM", item)) {
        return nullptr;
    }

    std::string choice;                                             // reads choices returns null if none
    if (!readField(in, "CHOICES", choice)) {
        return nullptr;
    }
    int choices = std::stoi(choice);                                // stores value in an int

    // verifies that number of choices is within valid range else returns null
    if ( choices < 0 || choices > Scene<Moment>::MAX_CHOICES ) {
        std::cout << "Error: scene " << fullScene << " has " << choices << " choices. The limit is 3." << std::endl;
        return nullptr;
    }

    // Moment* and number of choices are passed into Scene construction to create new Scene
    Scene<Moment>* scene = new Scene<Moment>(new Moment(fullScene, label, hps, loots, item), choices);

    // for loop iterates through each choice option
    for (int i = 0; i < choices; i++) {                               // when choices == 0 this is our base case to end recursion, skipping for loop and returning scene
        std::string nextChoice;
        if (!nextLine(in, nextChoice)) {                        // checks if there is another choice available
            delete scene;                                             // if no next choice deletes scene and returns nullptr to leave the tree
            return nullptr;
        }

        Scene<Moment>* child = readScene(in, nextChoice);           // recursive case calls readScene again to read off choices of child chosen

        if (child == nullptr) {                                        // checks if child does is unable to build, this will delete scene return nullptr
            delete scene;                                              // returning the null in recursive method will cause null to bubble up
            return nullptr;                                            // deleting the scenes to prevent memory leaks on the way
        }

        scene->setChoice(i,child);                                     // stores our pointer into the choices_ array in Scene.h
    }
    return scene;                                                      // returns scene
}                                                                      // end of method

// ===================== PLAYING =====================

// TODO 12: ask the player to pick a choice from 1 to numChoices.
//   - Print "Your choice: " (no newline) and read a whole line.
//   - If the line is not a valid number in range, print exactly:
//       Please type a number from 1 to <numChoices>.
//     and ask again.
//   - If the input ends (getline fails), return 0.
int Adventure::askChoice(int numChoices) {
    std::string input_;                                         // creates input string variable

    while (true) {                                              // begins input loop
        std::cout << "Your choice: ";                           // prompts user for input

        if (!std::getline(std::cin, input_)) {            // checks if getline fails
            return 0;                                           // returns 0 if true
        }
        try {
            int choiceNumber = std::stoi(input_);                   // stores string into an int variable

            if (choiceNumber >= 1 && choiceNumber <= numChoices) {  // checks for valid input [1, numChoices]
                return choiceNumber;                                // ends loop and returns value
            }
        } catch (const std::exception& e) {}

        // if input is not within valid [1, numChoices] range then error message displayed and loop continues
        std::cout << "Please type a number from 1 to " << numChoices << "." << std::endl;
    }
}                                                               // end of method

// TODO 13: play the story once from the start.
// Every time the player ENTERS a scene, in this order:
//   1. Add its HP change to hp and its loot to loot.
//   2. Print a line of 40 dashes, then the scene text.
//   3. If the scene has an item:
//        if the bag already has it, print "You already have: <item>"
//        otherwise add it to the bag and print "You picked up: <item>"
//   4. Print "HP: <hp>  Loot: <loot>" (two spaces before Loot).
//   5. If hp is 0 or less, print "Your HP hit 0. GAME OVER." and stop.
//      If the scene is an ending, print "THE END." and stop.
//   6. Print each choice as "[1] <label>" and use askChoice.
//      If askChoice returns 0, leave play right away.
// When the game stops, print:
//   Final HP: <hp>  Final loot: <loot>
//   Inventory (<count> items):
// and then print the bag.
// The bag is a List<Item>. Start with HP = startHp_ and loot = 0.
void Adventure::play() {
}

// ===================== ANALYZING =====================

// TODO 14: print exactly three lines:
//   Scenes: <count>
//   Endings: <count>
//   Longest playthrough: <height> choices
void Adventure::printStats() const {
}

// TODO 15: print every ending in the story, one per line.
void Adventure::printSpoilers() const {
}

// TODO 16: print every ending below scene.
// path holds the choice numbers taken to reach scene, like "2-1".
// Each ending prints as:
//   Path <path>: <scene text>
// If the very first scene is an ending, print "Path (start): <scene text>".
// Endings print in order: choice 1 before choice 2 before choice 3.
void Adventure::printEndings(Scene<Moment>* scene, const std::string& path) const {
}

// TODO 17: the greedy advisor.
// Start at the first scene with startHp_ and 0 loot.
// Enter scenes the same way play does (add HP and loot).
// Stop when hp is 0 or less, or you reach an ending.
// Otherwise always move to the choice with the MOST loot right now.
// On a tie, pick the lower choice number.
// Print:
//   Greedy advisor
//     Path: <path>
//     Loot: <loot>
//     Result: died               (or "Result: reached an ending")
void Adventure::greedyRun() const {
}

// TODO 18: the best total loot you can collect from scene down to an ending,
// arriving at scene with hp health and having taken path to get here.
//   - Return -1 if every way down from here kills you.
//   - Put the path of the best run into bestPath.
//   - Must be recursive and look at every choice.
int Adventure::bestLoot(Scene<Moment>* scene, int hp, const std::string& path, std::string& bestPath) const {
    return -1;
}

// TODO 19: the optimal advisor. Use bestLoot from the first scene.
// Print:
//   Optimal advisor
//     Path: <path>
//     Loot: <loot>
//     Result: reached an ending
// If no path survives, print only:
//   Optimal advisor
//     Result: no path survives this story
void Adventure::optimalRun() const {
}
