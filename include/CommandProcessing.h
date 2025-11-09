#pragma once
#include <string>
#include <fstream>
#include "LoggingObserver.h"

class Command;
class GameEngine;

class CommandProcessor: public ILoggable , public Subject {
    public:
        CommandProcessor(); // Default Constructor.
        CommandProcessor(const CommandProcessor &obj); // Deep copy constructor for CommandProcessor.
        virtual ~CommandProcessor(); // Destructor.
        virtual void getCommand(GameEngine& engine);
        Command* getCommandObjects();
        bool validate(GameEngine& engine, Command& cmdptr);
        // Assignment Operator and Output Operator.
        CommandProcessor& operator=(const CommandProcessor& other);
        friend std::ostream& operator<<(std::ostream& os, const CommandProcessor& commandPro);
        
        // ILoggable interface
        std::string stringToLog() const override;

    protected:
        Command* saveCommand(std::string& commandRead);
        virtual std::string readCommand();

        // Protected variable of commandObjects.
        std::vector<Command*> commandObjects;

};

class FileCommandProcessorAdapter : public CommandProcessor {
    public:
        FileCommandProcessorAdapter(std::string fileName); // Param. Constructor.
        ~FileCommandProcessorAdapter(); // Destructor
        void getCommand(GameEngine& engine); // Inherited virtual function.

        // FileCommandProcessorAdapter Assignment operator and Output operator.
        FileCommandProcessorAdapter(const FileCommandProcessorAdapter&) = delete; 
        FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter&) = delete;  


    protected:
        std::string readCommand();

    private:
        std::ifstream file; // File to be read from.
};