#pragma once

#include <string>

class Command;

class CommandProcessor {
    public:
        CommandProcessor(); // Default Constructor.
        CommandProcessor(const CommandProcessor &obj); // Deep copy constructor for CommandProcessor.
        ~CommandProcessor(); // Destructor.
        Command* getCommand();
        std::vector<Command*> getCommandObjects();
        void saveEffect();
        bool validate();

    private:
        void readCommand();
        void saveCommand(std::string commandRead);
        std::vector<Command*> commandObjects;

};

class FileCommandProcessorAdapter {
    public:
        Command* getCommand();
        void saveEffect();
        bool validate();

    private:
        void readCommand();
        void saveCommand(std::string commandRead);
        std::vector<Command*> commandObjects;


};