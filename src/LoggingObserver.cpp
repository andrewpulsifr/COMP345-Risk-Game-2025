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
