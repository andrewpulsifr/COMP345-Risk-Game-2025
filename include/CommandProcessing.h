#pragma once
#include <string>

class Command;
class GameEngine;

class CommandProcessor {
    public:
        CommandProcessor(); // Default Constructor.
        CommandProcessor(const CommandProcessor &obj); // Deep copy constructor for CommandProcessor.
        virtual ~CommandProcessor(); // Destructor.
        virtual void getCommand(GameEngine& engine);
        void saveEffect(Command* cmdObject, std::string& effect); // Param references string itself instead of making copy.
        bool validate(GameEngine& engine, std::string& commandEntered);

    protected:
        virtual std::string readCommand();
        virtual void saveCommand(std::string& commandRead);

    private:
        // Private variable of commandObjects.
        std::vector<Command*> commandObjects;

};

class FileCommandProcessorAdapter : public CommandProcessor {
    public:
        FileCommandProcessorAdapter(); // Default Constructor
        ~FileCommandProcessorAdapter(); // Destructor
        void getCommand(GameEngine& engine);

    protected:
        std::string readCommand();
        void saveCommand(std::string& commandRead);

};