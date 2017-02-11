#include <iostream>
#include <string>
 #include "Task.h"
#include "Agent.h"
#include "TaskType.h"
// #include "test/Task.h"
#include "Distribution.h"
// #include "test/Distribution.h"
// #include "Parameters.h"
// #include "Flags.h"
// #include "Simulation.h"
#include "Shift.h"
#include "Utility.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
using boost::property_tree::ptree;

using namespace std;

float util::seed = rand();
default_random_engine util::randNumGen = default_random_engine(util::seed);
vector<float> util::TRAFFIC = {1,1,1};

int main()
{
    ptree data;
    read_xml("/Users/Branch/Documents/Research/fra/shado/params.xml", data);

   // Distribution t;
   // Task t1(2, 1, 3, 40);
   // TaskType t(data.get_child("task"));
   // Shift t;
   Agent t(data.get_child("team.agents.agent"));
   // Team t(pt.get_child("teadm"));
   // Parameters t("params.xml");

    // if (static_cast<bool>(s))
    //     cout << "TRUE" << endl;
   // Simulation t("params.xml");
    // for (int i = 0; i < 1; i++)
    //     cout << t.genTask(0) << endl;

    cout << t << endl;

    return 0;
}