#pragma once
#include <string>

class Command;

class CommandProcessor {
    public:
        CommandProcessor(); // Default Constructor.
        CommandProcessor(const CommandProcessor &obj); // Deep copy constructor for CommandProcessor.
        ~CommandProcessor(); // Destructor.
        void getCommand();
        void saveEffect(Command* cmdObject, std::string& effect); // Param references string itself instead of making copy.
        bool validate(std::string& commandEntered);

    private:
        void readCommand(std::string& lineEntered);
        void saveCommand(std::string& commandRead);
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