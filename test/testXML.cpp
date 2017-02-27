#include "../lib/pugixml.h"
#include <iostream>
#include <string.h>

using namespace std;

// int main()
// {
//     pugi::xml_document doc1;
//     pugi::xml_parse_result result1 = doc1.load_file("tree.xml");
//
//     std::cout << "Load result: " << result1.description() << ", mesh name: " << doc1.child("mesh").attribute("name").value() << std::endl;
//
//     pugi::xml_document doc;
//     pugi::xml_parse_result result = doc.load_file("../config/params.xml");
// }

int main()
{
    pugi::xml_document doc;
    if (!doc.load_file("xgconsole.xml")) return -1;

    pugi::xml_node tools = doc.child("Profile").child("Tools");

    // tag::basic[]
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
    {
        std::cout << "Tool:";

        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
        {
            std::cout << " " << attr.name() << "=" << attr.value();
        }

        std::cout << std::endl;
    }
    // end::basic[]

    std::cout << std::endl;

    // tag::data[]
    for (pugi::xml_node tool = tools.child("Tool"); tool; tool = tool.next_sibling("Tool"))
    {
        std::cout << "Tool " << tool.attribute("Filename").value();
        std::cout << ": AllowRemote " << tool.attribute("AllowRemote").as_bool();
        std::cout << ", Timeout " << tool.attribute("Timeout").as_int();
        std::cout << ", Description '" << tool.child_value("Description") << "'\n";
    }
    // end::data[]

    std::cout << std::endl;

    // tag::contents[]
    std::cout << "Tool for *.dae generation: " << tools.find_child_by_attribute("Tool", "OutputFileMasks", "*.dae").attribute("Filename").value() << "\n";

    for (pugi::xml_node tool = tools.child("Tool"); tool; tool = tool.next_sibling("Tool"))
    {
        std::cout << "Tool " << tool.attribute("Filename").value() << "\n";
    }
    // end::contents[]
}
