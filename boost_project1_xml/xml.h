#pragma once
#define _CRT_SECURE_NO_WARNINGS
//[debug_settings_includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <iostream>
#include <memory>
#include <string>

#include "C:\Users\Zhema\Documents\cpp_projects\NoteBook\NoteBook\Note.h"

namespace pt = boost::property_tree; 

class xml
{
public:
    void put_to_xml_file(const Note& n)
    {
        node.put("<xmlattr>.type", static_cast<int>(n.Get_type()));  
        node.put("title", n.Get_title()); 
        node.put("date", n.Get_date()); 
        head.add_child("Notes.note", node); 
    }
    pt::ptree get_node() const
    {
        return node;
    }
    pt::ptree get_head() const
    {
        return head;
    }

    void write_to_xml(const std::string& filename)
    {
        pt::write_xml(filename, head);
    }

    void read_from_xml(const std::string& filename, pt::ptree& load_tree) const
    {
        pt::read_xml(filename, load_tree);
    }

    void get_data_from_xml(pt::ptree& load_tree, std::vector<Note*>& nodes)
    {
        if (load_tree.front().first == "Notes")
        {

            BOOST_FOREACH(auto & p, load_tree.get_child("Notes"))
            {
                pt::ptree node;
                std::tie(std::ignore, node) = p;
                Note_Type type;
                Note* ptr = nullptr;
                type = static_cast<Note_Type>(node.get("<xmlattr>.type", -1));
                switch (type)
                {
                case Note_Type::Note:
                    ptr = new Note(node.get("title", ""), node.get<time_t>("date", 0), node.get("content", ""));
                    break;
                case Note_Type::Reminder:
                    ptr = new Reminder(node.get("title", ""), node.get<time_t>("date", 0), node.get("content", ""));
                    break;
                case Note_Type::MemorableDate:
                    ptr = new MemorableDate(node.get("title", ""), node.get<time_t>("date", 0), 0, node.get("content", ""));
                    break;
                default:
                    break;
                }

                if (ptr)
                    nodes.push_back(ptr);
            }
        }
    }

private:
    pt::ptree node;
    pt::ptree head;
};






//main.cpp

/* 
     Note n("first", 12113); 
    Note n2("second", 12114);
    pt::ptree head;
    xml exempl;
    exempl.put_to_xml_file(n);
    exempl.put_to_xml_file(n2);
    exempl.write_to_xml("test.xml");

    pt::ptree loadtree;
    std::vector<Note*> nodes; 
    exempl.read_from_xml("test_copy.xml", loadtree);
    exempl.get_data_from_xml("test_copy.xml", loadtree, nodes);
*/