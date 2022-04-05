#include "Parser.hpp"

Parser::Parser()
{

}

Parser::Parser(std::string file)
{
    open(file);
}

Parser::~Parser()
{
    stream.close();
}

bool Parser::getLine(bool discardComments, bool outputComments)
{
    /*std::getline(stream,line);
    std::cout << line << "\n";
    bool success = ((!stream.eof())&&(!stream.fail()));*/
    bool success = std::getline(stream,line).good();
    if ((discardComments) && (success))
    {
        while ((success) && ((line[line.size()-1] == ':') || (line == "")))
        {
            if (outputComments)
            {
                std::cout << line << "\n";
            }
            success = std::getline(stream,line).good();
        }
    }
    return success;
}

std::string Parser::nextLine()
{
    if (getLine())
    {
        return line;
    }
    return "";
}

void Parser::open(std::string file)
{
    stream.close();
    char* pathTemp = new char[200];
    getcwd(pathTemp,200);
    std::string path = std::string(pathTemp);
    delete pathTemp;
    for (unsigned int i = 0; i < path.size(); i++)
    {
        if (path[i] == '\\')
        {
            path[i] = '/';
        }
    }
    path += "/"+file+".txt";
    stream.open(path.c_str());
}
