#pragma once

// One scene of the story and the doors leading out of it.
//
// RULES (do not change):
//   - Keep every member variable, name, and signature exactly as written.
//   - choices_ must stay a double pointer: Scene<T>** choices_.
//   - No std::vector, no std::array, no fixed-size array like choices_[3].
//   - The scene OWNS data_ and every scene its slots point to.

template <typename T>
class Scene {

private:
    T* data_;
    Scene<T>** choices_;
    int numChoices_;

public:
    static const int MAX_CHOICES = 3;

    Scene(T* data, int numChoices) : data_(data), choices_(nullptr), numChoices_(numChoices) {
        // TODO 1:
        // A scene with numChoices_ choices needs exactly numChoices_ slots.
        // An ending (0 choices) needs no slots at all.
        // Every slot must start out empty.

        if (numChoices_ > 0) {
            choices_ = new Scene<T>*[numChoices_];          // initializes array of scenes with # of values == numChoices_
            for (int i = 0; i < numChoices_; i++) {         // iterates 0 to MAX_CHOICES-1, in this case [0,3) => 0, 1, 2
                choices_[i] = nullptr;                      // initiallizes array slots 0, 1, 2 holding empty nullptrs
            }
        }
    }                                                       // end of method

    ~Scene() {
        // TODO 2:
        // Free everything this scene owns.
        // Think about what it owns, and what each slot points to.

        delete data_;                                       // deletes moment owned by the scene
        for (int i = 0; i < numChoices_; i++) {             // deletes each value stored in the array
            delete choices_[i];
        }
        delete [] choices_;                                 // deletes the entire array
    }                                                       // end of method

    T* getData() const {
        return data_;
    }

    int getNumChoices() const {
        return numChoices_;
    }

    // TODO 3: true if this scene is an ending.
    bool isEnding() const {
        if (numChoices_ == 0) {                             // scenes are only endings if there are no choices to make
            return true;
        }
        else {
            return false;
        }
    }                                                       // end of method

    // TODO 4: the scene behind slot index.
    // Return nullptr if index is not a real slot.
    Scene<T>* getChoice(int index) const {
        if (index >= 0 && index < numChoices_) {            // verifies that index is between [0,numChoices) bounds
            return choices_[index];                         // returns choice if valid
        }
        return nullptr;                                     // returns nullptr if index is out of bounds
    }                                                       // end of method

    // TODO 5: put next into slot index.
    // Do nothing if index is not a real slot.
    void setChoice(int index, Scene<T>* next) {
        if (index >= 0 && index < numChoices_) {            // if index is within bounds
            choices_[index] = next;                         // next scene is successfully stored
        }
    }                                                       // end of method
};                                                          // end of class
