#include "LoggingObserver.h"

// ======================= Subject Class =======================

/**
 * @brief Default constructor
 */
Subject::Subject() = default;

/**
 * @brief Destructor
 */
Subject::~Subject() = default;

/**
 * @brief Copy constructor - does not copy observers to avoid observer aliasing
 * @param other Subject to copy from
 */
Subject::Subject(const Subject&) : observers{} {}

/**
 * @brief Assignment operator - clears observers to avoid observer aliasing
 * @param other Subject to assign from
 * @return Reference to this subject
 */
Subject& Subject::operator=(const Subject&) {
    observers.clear();
    return *this;
}

/**
 * @brief Stream insertion operator for Subject
 * @param os Output stream
 * @param subject Subject to output
 * @return Reference to output stream
 */
std::ostream& operator<<(std::ostream& os, const Subject& subject) {
    os << "Subject [Observers: " << subject.observers.size() << "]";
    return os;
}

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
LogObserver::LogObserver(): logFilePath("gamelog.txt") {
    std::ofstream logFile(logFilePath, std::ios::trunc); // Clear File
    if (logFile.is_open()) {
        logFile << "=== Game Log Started ===" << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "ERROR: Could not create log file" << std::endl;
    }
}
LogObserver::LogObserver(const LogObserver& other) {
    // Copy constructor implementation
    logFilePath = other.logFilePath;
}

void LogObserver::update(const ILoggable& logUpdate) {
    std::ofstream logFile(logFilePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << logUpdate.stringToLog() << std::endl;
        logFile << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "ERROR: Could not open log file" << std::endl;
    }
}
