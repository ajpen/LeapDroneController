//
// Created by anfer on 11/19/2019.
//

#include "parser.h"

bool ConfigParser::openFile(std::string& path) {
    /*
     * Opens a file and returns whether the file opened successfully
     * */
    configFile.open(path);
    return configFile.is_open();
}

std::string ConfigParser::readLine() {
    /*
     * Reads a line from the config file.
     * If the file is closed, an Exception is thrown.
     * */
    std::string line;

    if(!configFile.is_open()){
        throw ClosedFileOperationException();
    }

    std::getline(configFile, line);
    return line;
}

std::string ConfigParser::extractFromDoubleQuotes(std::string &str){
    /*
     * Extracts any string between double Quotes.
     * str: String containing a substring surrounded by double quotes.
     * */
    std::string extracted;
    bool started = false;

    for (char i : str){

        // If string hasn't started and a double quotes was found
        // start collecting chars on the next iteration
        if(i == '"' && !started){
            started = true;
            continue;
        }
        // If second double quotes found, stop and return string
        else if (i == '"' && started){
            return extracted;
        }

        if (started){
            extracted.push_back(i);
        }
    }

    // If loop ended before the extracted string was returned, then either there
    // was no quoted string, or the matching quote was not found
    throw ConfigIncompleteQuotesException();
}

std::pair<std::string, std::string> ConfigParser::parseLine(std::string& str){
    /*
     * Parses a single configuration line. Splits the line by the separator
     * and extracts the
     * */
    std::size_t separatorPos = str.find(separator);
    if (separatorPos == std::string::npos){
        throw ConfigBadConfigException();
    }

    std::string key = str.substr(0, separatorPos);
    std::string value = str.substr(separatorPos);

    if (key.size() < 3){
        throw ConfigBadConfigException();
    }

    if (value.size() < 3){
        throw ConfigBadConfigException();
    }

    key = extractFromDoubleQuotes(key);
    value = extractFromDoubleQuotes(value);

    return std::pair<std::string, std::string>{key, value};
}

Config ConfigParser::Parse(std::string& configPath) {
    std::string line;
    Config configuration;

    if(!openFile(configPath)){
        return Config{};
    }

    std::pair<std::string, std::string> configElement;

    for(line=readLine(); !line.empty(); line=readLine()){
        configElement = parseLine(line);
        configuration[configElement.first] = configElement.second;
    }

    configFile.close();
    return configuration;
}