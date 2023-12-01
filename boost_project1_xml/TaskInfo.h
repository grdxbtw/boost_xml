#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <iostream>


 //multithreading
#include <queue>
#include <thread>
#include <condition_variable>
#include <future>
#include <mutex>

#include <string>
#include <iomanip>
#include "C:\Users\Zhema\Documents\cpp_projects\NoteBook\NoteBook\Note.h"

//rapidXML
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>


namespace pt = boost::property_tree; 

struct TaskInfo 
{
public:
    enum class TaskType
    {
        TT_Move, TT_Copy, TT_Delete
    };

    std::string srcFolder;		//<source>
    std::string name;			//<name>
    std::string destFolder;		//<destination>
    std::string filter;			//<filter>
    std::string backupFolder;	//<backup>
    TaskType type;				//attribute type
    uint16_t category{0};	 	//attribute cat
    bool rename{ false };				//attribute rename
    bool hasDelete{ false }; 				//presence of the "addel" attribute
};

class taskParse 
{
public:
    static void check_attr(const std::string& name, const std::string& val, TaskInfo& t) 
    {
        if (name == "type")
        {
            if (val == "move")
                t.type = TaskInfo::TaskType::TT_Move;
            if (val == "copy")
                t.type = TaskInfo::TaskType::TT_Copy;
            if (val == "delete")
                t.type = TaskInfo::TaskType::TT_Delete;

            return;
        }
        if (name == "cat")
        {
            t.category = stoi(val); 
            return;
        }
        if (name == "adddel")
        {
            t.hasDelete = true;
            return;
        }
        if (name == "rename")
        {
            t.rename = true;
            return;
        }

    }

    static int load_tasks(const std::string& filename, std::vector<TaskInfo>& tasks) 
    {
        int counter = 0; 
        pt::ptree tree; 
        pt::read_xml(filename, tree);

        BOOST_FOREACH(pt::ptree::value_type& v, tree.get_child("cockpit.functions"))     
        {
            if (v.first == "task")
            {
                TaskInfo t; 
                const std::string val = v.second.get("<xmlattr>.type", "");   

                for (const auto& attr : v.second.get_child("<xmlattr>"))
                {
                    check_attr(attr.first, attr.second.get_value<std::string>(), t);
                }

                t.backupFolder = v.second.get("backup", "");  
                t.name = v.second.get("name", ""); 
                t.srcFolder = v.second.get("source", ""); 
                t.destFolder = v.second.get("destination", ""); 
                t.filter = v.second.get("filter", ""); 
                counter++; 

                tasks.push_back(t);               
            }
        }

        return counter;
    }

    static std::future<void> save_tasks_async(const std::string& filename, const std::vector<TaskInfo>& tasks)
    {
        std::packaged_task<void(const std::string, const std::vector<TaskInfo>&)> task(&taskParse::save_tasks);
        std::future<void> res = task.get_future();
        std::thread thrd(std::move(task), filename, std::cref(tasks));
        thrd.detach();

        return res; //return future result to control task 
    }

    static void save_tasks(const std::string& filename, const std::vector<TaskInfo>& tasks) noexcept
    {
        pt::ptree head;
        for (const auto& t : tasks) 
        {
            pt::ptree tree;
            if (t.type == TaskInfo::TaskType::TT_Move)
                tree.put("<xmlattr>.type", "move");
            if (t.type == TaskInfo::TaskType::TT_Copy)
                tree.put("<xmlattr>.type", "copy");
            if (t.type == TaskInfo::TaskType::TT_Delete)
                tree.put("<xmlattr>.type", "delete");

            if (t.category != 0)
                tree.put("<xmlattr>.cat", t.category);
            if (t.hasDelete)
                tree.put("<xmlattr>.delete", "");
            if (t.rename)
                tree.put("<xmlattr>.delete", "");

            tree.put("name", t.name);   
            tree.put("source", t.srcFolder);  
            tree.put("destination", t.destFolder);  
            tree.put("filter", t.filter);  
            if (!t.backupFolder.empty()) 
                tree.put("backup", t.backupFolder);

            head.add_child("functions.task", tree);
        }
        pt::write_xml(filename, head);
    }

};




