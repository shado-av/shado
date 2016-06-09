/****************************************************************************
*																			*
*	File:		Time.h														*
*																			*
*	Author:		Branch Vincent												*
*																			*
*	Date:		Jun 7, 2016													*
*																			*
*	Purpose:	This file defines the time class.							*
*																			*
****************************************************************************/

#ifndef TIME_H
#define TIME_H

#include <iostream>
#include <string>

using namespace std;

// Enumerated type

enum phase {P1, P2, P3};

/****************************************************************************
*																			*
*	Definition of Time class												*
*																			*
****************************************************************************/

class Time
{
//	Public member functions

	public:

	//	Inspectors

		int get() const {return simTime;}
		int& get() {return simTime;}
		
	//	Mutators
	
		void add(int t) {simTime += t;}

//	Private member functions

	//private:
	
	//	Constructors
	
		Time() : simTime(), simPhase() {} 

//	Data members

	private:
		int simTime;		// Simulation time
		phase simPhase;		// Phase
};

/****************************************************************************
*																			*
*	Function:	add															*
*																			*
*	Purpose:	To add specified time to the simulation time				*
*																			*
****************************************************************************/

void Time::add(int t)
{
	simTime += t;
	if (simTime >
}

#endif