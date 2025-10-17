#pragma once

#include <string>

class Command;

class CommandProcessor {
    public:
        CommandProcessor(); // Default Constructor.
        CommandProcessor(const CommandProcessor &obj); // Deep copy constructor for CommandProcessor.
        Command* getCommand();
        std::vector<Command*> getCommandObjects;
        void saveEffect();
        boolean validate();

    private:
        void readCommand(std::string command);
        void saveCommand();
        std::vector<Command*> commandObjects;

}

class FileCommandProcessorAdapter {
    public:
        void saveCommand();
        Command* getCommand();
        void saveEffect();
        boolean validate();

    private:
        void readCommand(std::string command);
        std::vector<Command*> commandObjects;


}