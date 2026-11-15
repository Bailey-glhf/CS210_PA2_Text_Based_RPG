#include "Adventure.h"

#include <filesystem>
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
    int hp = startHp_;
    int loot = 0;
    List<Item> inventory;

    Scene<Moment>* current = tree_.getRoot();
    if (!current) {
        return;
    }

    while (current != nullptr) {
        Moment* moment = current->getData();
        if (!moment) {
            break;
        }

        hp+= moment->getHpChange();                             // add its HP change to hp
        loot+= moment->getLoot();                               // add loot to loot

        std::cout << std::string(40,'-')<<std::endl;        // print line of 40 dashes
        std::cout << moment->getText() << std::endl;            // print scene text

        std::string item = moment->getItem();
        if (!item.empty()) {                                    // if scene has item
            bool hasItem = inventory.search(item);

            if (hasItem) {
                std::cout << "You already have: " << item << std::endl;     // if the bag already has it notify user
            }
            else {
                inventory.addFront(new Item(item));
                std::cout << "You picked up: " << item << std::endl;        // if the bag doesn't have it notify user it has been picked up
            }
        }

        std::cout<< "HP: " << hp << "  LOOT: " << loot << std::endl;        // display HP & LOOT status

        if (hp <= 0 ) {                                                     // if HP = 0 GAME OVER
            std::cout << "Your HP hit 0. GAME OVER." << std::endl;
            break;
        }
        if (current->getNumChoices() == 0) {                                // if ending has been reached GAME OVER
            std::cout << "THE END." << std::endl;
            break;
        }

        int numChoices = current->getNumChoices();
        for (int i = 0; i < numChoices; i++) {
            Scene<Moment>* child = current->getChoice(i);
            std::string label = "";                                         // initializes label string
            if (child && child->getData()) {
                label = child->getData()->getLabel();

            }
            std::cout << "[" << (i+1) << "]" << label << std::endl;         // print each choice as "[1] <label>"
        }

        int choice = askChoice(numChoices);                                 // use askChoice
        if (choice == 0){                                                   // if askChoice returns 0
            break;                                                          // leave play right away
        }

        current = current->getChoice(choice-1);
    }
    // when the game stops, print final hp, final loot, final inventory
    std::cout << "Final HP: " << hp << "  Final LOOT: " << loot << std::endl;
    std::cout << "Inventory (" << inventory.size() << " items)" << std::endl;
    inventory.print();
}                                                                             // end of method

// ===================== ANALYZING =====================

// TODO 14: print exactly three lines:
//   Scenes: <count>
//   Endings: <count>
//   Longest playthrough: <height> choices
void Adventure::printStats() const {
    std::cout << "Scenes: " << tree_.countScenes() << std::endl;
    std::cout << "Endings: " << tree_.countEndings() << std::endl;
    std::cout << "Longest playthrough: " << tree_.height() << std::endl;
}                                                                       // end of method

// TODO 15: print every ending in the story, one per line.
void Adventure::printSpoilers() const {
    printEndings(tree_.getRoot(),"");
}                                                                       // end of method

// TODO 16: print every ending below scene.
// path holds the choice numbers taken to reach scene, like "2-1".
// Each ending prints as:
//   Path <path>: <scene text>
// If the very first scene is an ending, print "Path (start): <scene text>".
// Endings print in order: choice 1 before choice 2 before choice 3.
void Adventure::printEndings(Scene<Moment>* scene, const std::string& path) const {
    if (!scene) {                                       // checks if scene pointer null
        return;                                         // exits if true
    }
    if (scene->isEnding()) {                            // checks if current scene is a leaf node
        std::cout << "Path ";                           // formated output
        if (path.empty()) {                             // checks if root itself is an ending
            std::cout << "(start)";                     // formatted output
        }
        else {
            std::cout << path;                          // prints choice numbers path
        }
        std::cout << ": " << *(scene->getData()) << std::endl;      // prints scene text
    }
    else {
        for (int i = 0; i < scene->getNumChoices(); i++) {                  // loops through available choice options
            std::string nextPath = extendPath(path, (i+1));     // extends string with next choice number
            printEndings(scene->getChoice(i), nextPath);              // recursive call to go down child path
        }
    }
}                                                                           // end of method

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
    // start and enter scenes same way that play does - copied from play method
    // removed unnecessary output formatting

    int hp = startHp_;
    int loot = 0;
    std::string path = "";                                  // string variable to track path ex 2-2-1

    Scene<Moment>* current = tree_.getRoot();
    if (!current) {
        return;
    }

    while (current != nullptr) {
        Moment* moment = current->getData();
        if (!moment) {
            break;
        }

        hp+= moment->getHpChange();                             // add its HP change to hp
        loot+= moment->getLoot();                               // add loot to loot

        if (hp <= 0 || current->isEnding()) {                   // ends run if died or ending reaching
            break;
        }

        int numChoices = current->getNumChoices();
        if (numChoices == 0) {                                  // if there are no further choices ending has been reached
            break;
        }

        int bestChoice = 0;                                     // will be used to track best choice index
        int bestLoot = -1;                                      // will be used to track highest loot
        bool first = true;

        for (int i = 0; i < numChoices; i++) {                  // loop will iterate through each choice to find best loot
            Scene<Moment>* child = current->getChoice(i);       // gets each choice in order
            if (child && child->getData()) {                    // if child is valid node
                int iLoot = child->getData()->getLoot();        // loot is stored in iLoot
                if (first || iLoot > bestLoot) {               // first choice loot always stored in best choice
                    bestLoot = iLoot;                           // beyond first highest loot option chosen
                    bestChoice = i;
                    first = false;
                }
            }
        }

        path = extendPath(path, (bestChoice+1));    // adds choice number to string of path
        current = current->getChoice(bestChoice);               // moves on to check loot choices of next choice

        }

        std::cout<< "Greedy Advisor \n" << "Path: " << path << "\nLoot: " << loot << std::endl;

        if (hp <= 0 ) {                                                     // if HP = 0 GAME OVER
            std::cout << "Result: died" << std::endl;
        }
        else {                                                              // if ending has been reached GAME OVER
            std::cout << "Result: reached an ending" << std::endl;
        }
}                                                                            // end of method

// TODO 18: the best total loot you can collect from scene down to an ending,
// arriving at scene with hp health and having taken path to get here.
//   - Return -1 if every way down from here kills you.
//   - Put the path of the best run into bestPath.
//   - Must be recursive and look at every choice.
int Adventure::bestLoot(Scene<Moment>* scene, int hp, const std::string& path, std::string& bestPath) const {
    if (!scene || !scene->getData()) {                          // checks if scene and data are valid / not corrupted
        return -1;                                              // returns -1 if data invalid
    }
    hp += scene->getData()->getHpChange();
    if (hp <= 0 ) {                                             // returns -1 if player dies
        return -1;
    }

    int currentLoot = scene->getData()->getLoot();

    if (scene->isEnding()) {                                    // checks if scene is ending
        bestPath = path;                                        // first path to end = best path
        return currentLoot;                                     // returns loot value from path
    }

    int greatestLoot = -1;                                          // int variable will track highest loot values for each ending
    std::string thisPath = "";                                  // string variable to track best subtree paths

    for (int i = 0; i < scene->getNumChoices(); i++) {
        Scene<Moment>* child = scene->getChoice(i);
        if (!child) {                                           // if child choice invalid moves to next choice
            continue;
        }

        std::string nextPath = extendPath(path, (i+1));     // will count path for each individual choice
        std::string subPath = "";                                       // starts new path string for subTree

        int subPathLoot = bestLoot(child, hp, nextPath, subPath);       // recursive call

        if (subPathLoot != -1) {                                        // if sub path provides any loot
            int totalLoot = subPathLoot + currentLoot;                  // sub path loot added to total
            if (totalLoot > greatestLoot) {                             // if total for this path is greater than current best
                greatestLoot = totalLoot;                               // then total is set to the new best
                bestPath = subPath;                                     // and this path is set to the new best
            }
        }
    }

    return greatestLoot;                                                 // returns highest loot available from all paths diverging from scene passed
}                                                                        // end of method

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
    std::string path = "";                          // current path starts empty since we begin at root
    std::string optimalPath = "";                   // optimal path will be chosen by bestLoot function


    int optimalLoot = bestLoot((tree_.getRoot()), startHp_, path, optimalPath );    // calls bestLoot, returned int saved as most optimal Loot
                                                                                               // if there is no survivable ending bestLoot returns -1

    if (optimalLoot != -1) {                                                    // as long as survivable ending found print optimal path, loot, & result
        std::cout << "Optimal Advisor" << std::endl;
        std::cout << "Path: " << optimalPath << std::endl;
        std::cout << "Loot: " << optimalLoot << std::endl;
        std::cout << "Result: reached an ending" << std::endl;
    }else {                                                                     // else print no survivable path result
        std::cout << "Optimal Advisor" << std::endl;
        std::cout << "Result: no path survives this story " << std::endl;
    }
}                                                                               // end of method
