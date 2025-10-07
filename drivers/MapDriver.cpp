#include "../include/Map.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void testLoadMaps(){

    cout << "=== Testing Map Loading ===" << endl;
    try{
        // Initialize MapLoader and get available map files
        MapLoader mapLoader;
        vector<string> mapFiles = mapLoader.getMapFiles();
        mapLoader.printMapFiles(mapFiles);

        // Prompt user to select a map
        cout << "Select a map by index: ";
        int index = 0;
        if (!(cin >> index)){
            cin.clear(); // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            throw runtime_error("Invalid input. Please enter a number.");
        }

        if(index < 1 || index > static_cast<int>(mapFiles.size())) {
            throw runtime_error("Invalid map selection.");
        }

        // Load the selected map
        string selectedMap = mapFiles[index - 1];
        Map mapOutput;
        mapLoader.loadMap(selectedMap, mapOutput);
        // TODO : print map details and validate
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    // TO DO: Implement actual map validation tests
}