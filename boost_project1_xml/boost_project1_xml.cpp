// boost_project1_xml.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
//[debug_settings_includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp> 
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <iomanip>

//headers
#include "xml.h"
#include "TaskInfo.h"
#include "C:\Users\Zhema\Documents\cpp_projects\NoteBook\NoteBook\Note.h"

//rapidXML
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>

namespace rpxml = rapidxml;
namespace pt = boost::property_tree; 
//]
//[debug_settings_data
struct debug_settings
{
    std::string m_file;               // log filename
    int m_level;                      // debug level
    std::set<std::string> m_modules;  // modules where logging is enabled
    void load(const std::string& filename);
    void save(const std::string& filename);
};
//]
//[debug_settings_load
void debug_settings::load(const std::string& filename)
{
    // Create empty property tree object
    pt::ptree tree;

    // Parse the XML into the property tree.
    pt::read_xml(filename, tree);

    // Use the throwing version of get to find the debug filename.
    // If the path cannot be resolved, an exception is thrown.
    m_file = tree.get<std::string>("debug.filename");

    // Use the default-value version of get to find the debug level.
    // Note that the default value is used to deduce the target type.
    m_level = tree.get("debug.level", 0);

    // Use get_child to find the node containing the modules, and iterate over
    // its children. If the path cannot be resolved, get_child throws.
    // A C++11 for-range loop would also work.
    BOOST_FOREACH(pt::ptree::value_type & v, tree.get_child("debug.modules")) 
    {
        // The data function is used to access the data stored in a node.
        m_modules.insert(v.second.data()); 
    }
}
//]
//[debug_settings_save
void debug_settings::save(const std::string& filename)
{
    // Create an empty property tree object.
    pt::ptree tree;

    // Put the simple values into the tree. The integer is automatically
    // converted to a string. Note that the "debug" node is automatically
    // created if it doesn't exist.
    tree.put("debug.filename", m_file);
    tree.put("debug.level", m_level);

    // Add all the modules. Unlike put, which overwrites existing nodes, add
    // adds a new node at the lowest level, so the "modules" node will have
    // multiple "module" children.
    BOOST_FOREACH(const std::string & name, m_modules)
        tree.add("debug.modules.module", name);


    // Write property tree to XML file
    pt::write_xml(filename, tree);
}
//]

std::string trim(const std::string& s)
{
    const std::string delim("\t\n\b\r");
    std::size_t ind = s.find_first_not_of(delim);
    if (ind == std::string::npos)
        return std::string();
    size_t last = s.find_last_not_of(delim);
    return std::string(s, ind, last - ind);
}

void read_tasks(pt::ptree& tree, std::string const& indent = "")
{
    for (pt::ptree::value_type &v: tree)
    {      
        std::cout << indent << v.first << '\t' << trim(v.second.data()) << '\n';

        if (v.first == "task")
        {
            for (const auto& attr: v.second.get_child("<xmlattr>"))
            {
                std::cout << attr.first << "\t" << attr.second.data() << '\n';
            }
            std::string comment = v.second.get("<xmlcomment>","");
            std::cout << "comment:\t" << ((comment.empty()) ? "None" : comment) << '\n';
            std::cout << "Name: " << std::quoted(v.second.get("name", "")) << '\t';
            std::cout << "Source: " << std::quoted(v.second.get("source", "")) << '\t';
            std::cout << "Destination: " << std::quoted(v.second.get("destination", "")) << '\t';
            std::cout << "Filter: " << std::quoted(v.second.get("filter", "")) << '\n';
            std::cout << "----------------\n";
        }
        else
        {
            if(!v.second.empty())
                read_tasks(v.second, indent + "  ");
        }
    }
}

void rapid_xml_search(rpxml::xml_node<>* node, std::string const& indent = "")
{
    std::cout << indent << node->name() << "\t " << node->value() << std::endl;
    for (rapidxml::xml_attribute<>* attr = node->first_attribute(); attr; attr = attr->next_attribute()) 
    {
        std::cout << indent << " name: " << attr->name() << " \tvalue: " << attr->value() << std::endl;
    }

    for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
    {
        rapid_xml_search(child, indent + "  ");
    }
}


int main()
{
    {
        pt::ptree emthy_tree;
        pt::read_xml("settings.xml", emthy_tree);
        read_tasks(emthy_tree);
        std::cout << std::endl; 

        std::vector<TaskInfo> tasks;
        taskParse tp;
        tp.load_tasks("settings.xml", tasks);
        //tp.save_tasks("settings_tasks.xml", tasks);
     
        auto res = tp.save_tasks_async("settings_tasks.xml", tasks);
        res.wait(); //exeption 
    
    }

    ///rapid xml
    rpxml::file<> xml_file("settings.xml"); 

    rpxml::xml_document<> xml_doc; 

    xml_doc.parse<0>(xml_file.data()); 
    rpxml::xml_node<>* root = xml_doc.first_node(); 
   // rapid_xml_search(root);

    //// logging
    try
    { 
        debug_settings ds; 
        ds.load("debug_settings.xml"); 
        ds.save("debug_settings_out.xml"); 
        std::cout << "Success\n"; 
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n"; 
    }

    //////////
    {
        Note n("first", 12113);
        Note n2("second", 12114);
        pt::ptree head;
        xml exempl;
        exempl.put_to_xml_file(n);
        exempl.put_to_xml_file(n2);
        exempl.write_to_xml("test.xml");
        pt::write_json("test.json", exempl.get_head());
        pt::write_ini("test.ini", exempl.get_node());

        pt::ptree loadtree;
        std::vector<Note*> nodes;
        exempl.read_from_xml("test_copy.xml", loadtree);
        exempl.get_data_from_xml(loadtree, nodes);

        for (size_t i = 0; i < nodes.size(); i++)
        {
            delete nodes[i];
        }
    }

    return 0;
}
