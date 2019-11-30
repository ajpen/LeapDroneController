//
// Created by anfer on 11/19/2019.
//

#ifndef LEAPDRONECONTROLLER_PARSER_H
#define LEAPDRONECONTROLLER_PARSER_H

#include "config.h"
#include <string>
#include <fstream>


class ConfigParser {
    /*
     * Parses a configuration file of key-value pairs that are joined by a colon.
     * The configuration file should contain only one key-value pair per line, and
     * thus separated by the newline char. Space is ignored. Keys and values must be
     * surrounded by double quotes.
     * space is otherwise ignored
     * e.g
     * "host": "https://www.website.com"\n
     *
     * The above is parsed as a key value pair whose key is "host" and the value is
     * "https://www.website.com"
     * */
    private:
        const char separator = ':';
        std::ifstream configFile;


        bool openFile(std::string& path);
        std::string readLine();
        std::pair<std::string, std::string> parseLine(std::string& str);
        std::string extractFromDoubleQuotes(std::string& str);

    public:
        Config Parse(std::string& configPath);

};


#endif //LEAPDRONECONTROLLER_PARSER_H
