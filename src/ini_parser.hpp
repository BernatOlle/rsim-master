#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <string>
#include <unordered_map>
#include <list>
#include <iostream>
#include <fstream>
#include <algorithm>


struct section
{
    std::string name;
    std::unordered_map<std::string, std::string> keyvalues;
};

class config
{
public:
    config(const std::string& filename);

    section* get_section(const std::string& sectionname);
    std::list<section>& get_sections();

    std::string get_value(const std::string& sectionname, const std::string& keyname);

private:
    void parse(const std::string& filename);

    std::list<section> sections;
};

#endif // INI_PARSER_H
