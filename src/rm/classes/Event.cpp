/****************************************************************************
*																			*
*	File:		Event.cpp												    *
*																			*
*	Author:		Branch Vincent												*
*																			*
*	Purpose:	This file defines the Event class.	   					    *
*																			*
****************************************************************************/

#include <iostream>
#include <string>
// #include <ctime>
#include "event.h"
#include "task.h"
#include "agent.h"
#include "utility.h"

using namespace std;

vector<string> valid_types = {"arrival", "departure", "expiration", "none"};

/****************************************************************************
*																			*
*	Function:	Event       												*
*																			*
*	Purpose:	To construct an event                                       *
*																			*
****************************************************************************/

Event::Event(string type, DateTime time, Task& task)
	//, Agent& agent)
	: type(type),
	time(time), task(task)//, agent(agent)
{
	ASSERT(util::contains(valid_types, type), "Invalid event type");
}

/****************************************************************************
*																			*
*	Function:	process       												*
*																			*
*	Purpose:	To process an event                                       	*
*																			*
****************************************************************************/

void Event::process()
{
	if (type == "arrival")
	{
		task.start(time);
		// task.type.team.arrivingTasks.pop_front();
	}
	else if (type == "departure")
	{
		task.finish(time);
	}
}

bool Event::before(const Event& event) const {return time < event.time;}

/****************************************************************************
*																			*
*	Function:	output       												*
*																			*
*	Purpose:	To output an event                                       	*
*																			*
****************************************************************************/

void Event::output(ostream& out) const
{
	// out << "Type: " << type << endl;
	// out << "Time: " << time << endl;
	// out << "Agent: " << agent << endl;
}


//	Operators

ostream& operator<<(ostream& out, const Event& e) {e.output(out); return out;}
bool operator<(const Event& e1, const Event& e2) {return e1.before(e2);}
bool operator>(const Event& e1, const Event& e2) {return !e1.before(e2);}