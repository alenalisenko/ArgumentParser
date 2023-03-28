#include "lib\ArgParser.h"

#include <iterator>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>

std::vector<std::string> SplitString(const std::string &str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

int main(int argc, char **argv) {
    ArgumentParser::ArgParser parser("My Parser");
    parser.AddStringArgument("param1");

    parser.Parse(SplitString("app --param1=value1"));
    parser.GetStringValue("param1");
}



