#pragma once
#include <string>
#include <fstream>

class Command;
class GameEngine;

class CommandProcessor {
    public:
        CommandProcessor(); // Default Constructor.
        CommandProcessor(const CommandProcessor &obj); // Deep copy constructor for CommandProcessor.
        virtual ~CommandProcessor(); // Destructor.
        virtual void getCommand(GameEngine& engine);
        bool validate(std::string& lineEntered);

    protected:
        virtual std::string readCommand();
        Command* saveCommand(std::string& commandRead);

        // Protected variable of commandObjects.
        std::vector<Command*> commandObjects;

};

class FileCommandProcessorAdapter : public CommandProcessor {
    public:
        FileCommandProcessorAdapter(std::string fileName); // Param. Constructor.
        ~FileCommandProcessorAdapter(); // Destructor
        void getCommand(GameEngine& engine); // Inherited virtual function.

    protected:
        std::string readCommand();

    private:
        std::ifstream file; // File to be read from.
};