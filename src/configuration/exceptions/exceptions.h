//
// Created by anfer on 11/19/2019.
//

#ifndef LEAPDRONECONTROLLER_EXCEPTIONS_H
#define LEAPDRONECONTROLLER_EXCEPTIONS_H

#include <exception>

/*
 * Thrown when an operation is done on a closed file
 * */
class ClosedFileOperationException : public std::exception {
    const char * what () const noexcept override {
        return "File operation done on a closed file.";
    }
};


class ConfigIncompleteQuotesException : public  std::exception{
    const char * what() const noexcept override {
        return "Bad config: Quoted string lacks terminating quote.";
    }
};

class ConfigBadConfigException : public std::exception{
    const char * what() const noexcept override {
        return "Bad config format.";
    }
};

#endif //LEAPDRONECONTROLLER_EXCEPTIONS_H
