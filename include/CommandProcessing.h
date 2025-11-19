/**
 * @file CommandProcessing.h
 * @brief Assignment 1 – Part 1: Assignment 2 - Part 1: Command Processor and Command Adapter.
 *
 * @details
 *  This file contains the declaration of the command processor and command adapter that processes
 *  the command that is entered either through -console or read through -file. It works with the GameEngine to
 *  process the states of the game. The command adapter follows the Adapter Pattern, and makes use of polymorphism.
 *
 * @note A1, Part 1 classes/functions live in this duo (CommandProcessing.h/CommandProcessing.cpp). 
 *       The driver `testCommandProcessor()` is implemented in CommandProcessingDriver.cpp.
 */

#pragma once
#include <string>
#include <fstream>
#include <vector>
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


        // === A3, Part 2: Tournament Mode ===
        std::string cleanWhiteSpace(const std::string& command);
        std::vector<int> validateTournament(const std::string& command);

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