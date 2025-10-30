#include <iostream>
#include "../include/CommandProcessing.h"

void testCommandProcessor() {
    std::cout << "In testCommandProcessor.cpp file..." << std::endl;

    CommandProcessor* commandPro = new CommandProcessor();

    commandPro->getCommand();

}