#pragma once
#include "Scene.h"

// The whole story. It knows the first scene and nothing else.
//
// RULES (do not change):
//   - Keep every name and signature exactly as written.
//   - The private helpers must be recursive. No loops that walk down the tree.
//   - This class must work for ANY T. Never mention Moment in here.
template <typename T>
class StoryTree {
private:
    Scene<T>* root_;

    // TODO 6: how many scenes are in the story that starts at scene?
    // An empty story has 0 scenes.
    int countScenes(Scene<T>* scene) const {
        if (scene == nullptr) {                                 // this is our base case when we know we've reached the end
            return 0;
        }

        int count = 1;                                          // we'll use this value to count number of scenes starting with current one
        int i = 0;                                              // i will be used to iterate through child choices

        while ( Scene<T>* child = scene->getChoice(i) ) {       // while will loop through each of the child choices of current choice
            count += countScenes(child);                        // recursively add the count of each child branch
            i++;
        }
        return count;                               // return total scene count for this tree or subtree
    }                                               // end of method

    // TODO 7: how many endings are in the story that starts at scene?
    int countEndings(Scene<T>* scene) const {
        if (scene == nullptr) {                                 // this is our base case when we know we've reached the end
            return 0;
        }
        if (scene->isEnding()) {                                // this checks if current scene is ending where it will return 1
            return 1;
        }

        int count = 0;                                          // we'll use this value to count number of endings starting with 0
        int i = 0;                                              // i will be used to iterate through child choices

        while ( Scene<T>* child = scene->getChoice(i) ) {       // while will loop through each of the child choices of current choice
            count += countEndings(child);                       // recursively add the count of each found ending
            i++;
        }
        return count;                               // return total scene count for this tree or subtree
    }                                               // end of method

    // TODO 8: the most choices a player can make on the way to an ending,
    // starting at scene. A single ending scene has height 0.
    // An empty story has height -1.
    int height(Scene<T>* scene) const {
        return -1;
    }

public:
    StoryTree() : root_(nullptr) {}

    ~StoryTree() {
        clear();
    }

    void setRoot(Scene<T>* root) {
        clear();
        root_ = root;
    }

    Scene<T>* getRoot() const {
        return root_;
    }

    bool isEmpty() const {
        return root_ == nullptr;
    }

    // TODO 9: free every scene in the story and leave the tree empty.
    // Hint: what does deleting a Scene already do for you?
    void clear() {
    }

    int countScenes() const {
        return countScenes(root_);
    }

    int countEndings() const {
        return countEndings(root_);
    }

    int height() const {
        return height(root_);
    }
};
