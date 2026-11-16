Reflection Questions

1. For every function you wrote (TODO 1 to 19), explain in your own words what it does and give its time complexity in terms of n, the number of scenes. One short paragraph each.


    TODO1: Time Complexity O(1). This is the Scene Constructor. This initializes the scene and allocates a dynamic array of size of number of choices, each choice will be set to nullpointer to start.
    TODO2: "  " O(1). This is the Scene Deconstructor. This frees up the scene's data_, deleting the allocated choices_ from array to prevent memory leaks.
    TODO3: "  " O(1). This is the isEnding check. Returns true if there are no choices, which indicates an ending node leaf.
    TODO4: "  " O(1). This is the getChoice function. This returns a pointer for a child scene at index i as long as i is valid. Returns nullpointer if not within bounds.
    TODO5: "  " O(1). This is the setChoice function. This stores a childer pointer into slot i of the choices array made in scene constructor. Function checks if index is valid.
    TODO6: "  " O(n). This is the countScenes function. This counts recursively through our tree or subtree whatever the given scene we start at. We add 1 plus the count of all the child branches to ensure we visit every scene.
    TODO7: "  " O(n). This is the countEndings function. This counts recursively through tree and returns one for every leaf and sums the total for a total count for all endings.
    TODO8: "  " O(n). This is our height counter function. This recursively counts haieght of tree by finding the bigest height variable the then adding 1. 
    TODO9: "  " O(n). This is our clean function. This uses recursion to delete all the scene nodes starting from root and then moving to the others. Root set to nullpointer.
    TODO10:"  " O(n). This is our load function. This will open story txt file reading starting values and build the tree. Root is set to tree_.
    TODO11:"  " O(n). This is our readScene function. This function recursively reads and builds the child scenes then cleans up memory and returns nullpointer if the reading of file fails.
    TODO12:"  " O(1). This is our askChoice function. This prompts user for input, validates input is within number of choices bounds. This loops for invalid inputs.
    TODO13:"  " O(n). This is the Play function. This handles a bulk of the program. This starts play at the root updating values and inventory through each choice. Paths have a max worst case of O(h) with h being the height of the tree, with my story 5 or O(n) number of scenes.
    TODO14:"  " O(1). This is the printStats functions. This is O(1) to run as it just prints values but it calls countScenes, countEndings, and the height check which are each O(n). Including the calls within the getStats we'd add O(n)+O(n)+O(n)=O(3n) drop the constant to get O(n).
    TODO15:"  " O(n). This is our printSpoilers function. This calls printEndings recursively to print out each ending in the tree.
    TODO16:"  " O(n). This is our printEndings function. This uses recursion to travel along paths and build strings of each ending.
    TODO17:"  " O(n). This is the greedyRun function. Greedy run will travel paths choosing the choice with the highest loot value each time. In mine I set this to a short ending to more easily track and compare for outputs. At worst time complexity this completes 1 full path of the max height.
    TODO18:"  " O(n). This is the bestLoot function. This uses recursion to explore all the paths where HP doesn hit 0 or below. This returns the path that has the highest loot attainable without dying.
    TODO19:"  " O(n). This is optimalRun function. This calls the bestLoot function starting at the root. It takes the path with the best loot, and prints the path from root to end with total loot and final ending.

2. Why does each scene allocate exactly numChoices slots instead of declaring Scene<T>* choices[3]? What does an ending hold in choices_, and why?


    If we declared a fixed array we would be wasting memory space. Each path can have up to 3 choices but doesn't require 3 total and the ending scenes would have 0 choices. With dynamically allocated memory we save significantly on space by matching the saze that we actually need instead of what we potentially need. For any unused choice slots, like with endings that have no choices, we store a nullpointer which will stop a child array from being declared.

3. Walk through your Scene destructor. What gets deleted, and in what order? What would go wrong if choices_ were deleted first?


    First me scene destruction deletes the scene's data_ payload and then deletes the array of choices. If the choices were deleted first then we'd loose the address to the choices array. If we deleted choices first we loose the address to child scene pointers and cause a memory leak.

4. Show the path greedy took and the path optimal took on your story, with the loot for each. Explain why greedy lost. If greedy did not lose, change your story until it does, and explain what you changed.


    The greedy path in my_story.txt is the ultimate good ending. My greedy path is just '1' with a loot of 5. Greedy path just chooses the option with the highest loot of each of the choices in this case 5. I had to change the path because my greedy path was a win. I altered loot values, Greedy run now shows path 2-1 with a result of "died" loot of 9. I changed the values of loot so that if you choose the highest loot value each time you were given a set of choices you would die. For my Optimal path I would get 2-1-1-1 with loot of 8 which is also a bad ending but not one in which player dies.

5. bestLoot tries every path, yet it runs in O(n) time. Explain why it is not exponential, using what you know about paths and leaves in a tree.


    Best Loot tries every path but operations through a depth first traversal on our fixed tree so it doesn't need to overlap computing operations. It visits every node ones during traversal so scales linearly with n number of scenes.
    
6. Which List methods does your game use, and why? What does one search cost as the bag grows?


    I used add front because our List is a linkedList. With linkedList addFront is only O(1), I used this so we wouldn't have to walk O(n) for every add if we used addBack. Search is still an 0(n) function with n being the number of items in the inventory.
