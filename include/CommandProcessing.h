#pragma once

#include <string>

class Command;

class CommandProcessor {
    public:
        void saveCommand();
        Command* getCommand();
        void saveEffect();

    private:
        void readCommand(std::string command);
        std::vector<Command*> commandObjects;

}

class FileCommandProcessorAdapter {
    public:

    private:
        void readCommand(std::string command);
        
}