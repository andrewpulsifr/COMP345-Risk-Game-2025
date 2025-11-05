
#pragma once

#include <ostream>
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
    Subject() = default;
    virtual ~Subject() = default;

    // Disable copy and move semantics
    Subject(const Subject&) = delete;
    Subject& operator=(const Subject&) = delete;
    Subject(Subject&&) = delete;
    Subject& operator=(Subject&&) = delete;

    // Observer management
    void attach(Observer* observer);
    void detach(Observer* observer);
    void notify() const;

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
    ~LogObserver();

    LogObserver& operator=(const LogObserver& other);
    friend std::ostream& operator<<(std::ostream& os, const LogObserver& observer);

    void update(const ILoggable& logUpdate) override;
};
