
#pragma once

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>


class Observer; // Forward declaration


class ILoggable {

    public:
    virtual ~ILoggable() = default;
    virtual std::string stringToLog() const = 0;

};

class Subject {

    public:
    Subject();
    Subject(const Subject&);
    Subject& operator=(const Subject&);
    virtual ~Subject();
    
    friend std::ostream& operator<<(std::ostream& os, const Subject& subject);

    // Observer management
    void attach(Observer* observer);
    void detach(Observer* observer);
    void notify() const;
    
    // Observer propagation - allows parent subjects to propagate observers to child subjects
    void propagateObserversTo(Subject* childSubject) const;

    private:
    std::vector<Observer*> observers;
};

class Observer {

    public:
    virtual ~Observer() = default;
    virtual void update(const ILoggable& logUpdate) = 0;
};

class LogObserver : public Observer {

public:
    LogObserver();
    LogObserver(const LogObserver& other);
    ~LogObserver() = default;

    LogObserver& operator=(const LogObserver& other);
    friend std::ostream& operator<<(std::ostream& os, const LogObserver& observer);

    void update(const ILoggable& logUpdate) override;

private:
    std::string logFilePath;
};
