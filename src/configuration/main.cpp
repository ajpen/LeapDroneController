//
// Created by anfer on 11/19/2019.
//

#include "parser.h"

#include <iostream>

int main(){

    ConfigParser p;
    std::string path = "config.txt";
    Config c = p.Parse(path);

	if (c.empty()) {
		std::cout << "Failed to parse" << std::endl;
	}

    if(c.find("something") == c.end() || c.find("something")->second != "somethingElse"){
        std::cout << "Test case failed. Could not find configuration key 'something'" << std::endl;
        exit(1);
    }

    if (c.find("a") == c.end() || c.find("a")->second != "b"){
        std::cout << "Test case failed. Could not find configuration key 'a'" << std::endl;
        exit(1);
    }

    if (c.find("123") == c.end() || c.find("123")->second != "321"){
        std::cout << "Test case failed. Could not find configuration key 'a'" << std::endl;
        exit(1);
    }

    std::cout << "Test case passed." << std::endl;
}