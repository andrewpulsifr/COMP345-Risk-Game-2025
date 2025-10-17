#include "../include/GameEngine.h"

#include <vector>

// Default constructor for CommandProcessor.
CommandProcessor::CommandProcessor() : commandObjects() {}

// Constructor that takes

// Deep copy constructor for CommmandProcessor.
CommandProcessor::CommandProcessor(CommandProcessor &obj) {
    for(int i = 0; i < obj.getCommandObjects.size(); i++) {
        Command* commandptr = obj.getCommandObjects[i];

        if(commamndptr) {
            Command* newCopy = new Command(obj.getName());
            this->commandObjects.push_back(newCopy);
        }
    }
}

// 

