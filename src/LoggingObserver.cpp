#include "LoggingObserver.h"

// ======================= Subject Class =======================

/**
 * @brief Attach an observer to this subject
 * @param observer Observer to attach
 */
void Subject::attach(Observer* observer) {
    if (observer) {
        observers.push_back(observer);
    }
}

/**
 * @brief Detach an observer from this subject
 * @param observer Observer to detach
 */
void Subject::detach(Observer* observer) {
    for (auto it = observers.begin(); it != observers.end(); ++it) {
        if (*it == observer) {
            observers.erase(it);
            break;
        }
    }
}

/**
 * @brief Notify all observers by calling their update method
 */
void Subject::notify() const {
    for (Observer* observer : observers) {
        if (observer) {
            observer->update(*dynamic_cast<const ILoggable*>(this));
        }
    }
}

// ======================= LogObserver Class =======================
LogObserver::LogObserver() {
    // Constructor implementation
}

LogObserver::LogObserver(const LogObserver& other) {
    // Copy constructor implementation
}

LogObserver::~LogObserver() {
    // Destructor implementation
}

LogObserver& LogObserver::operator=(const LogObserver& other) {
    // Copy assignment operator implementation
    return *this;
}

std::ostream& operator<<(std::ostream& os, const LogObserver& observer) {
    // Friend function implementation
    return os;
}

void LogObserver::update(const ILoggable& logUpdate) {
    logUpdate.stringToLog(); // Stub implementation
}
