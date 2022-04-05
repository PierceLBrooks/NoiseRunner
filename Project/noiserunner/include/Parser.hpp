#ifndef PARSER_HPP
#define PARSER_HPP

#include "Tyrant/Framework.hpp"
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

class Parser
{
    public:
        Parser();
        Parser(std::string file);
        virtual ~Parser();
        void open(std::string file);
        bool getLine(bool discardComments = false, bool outputComments = false);
        std::string nextLine();
        std::ifstream stream;
        std::string path, line;
};

#endif
