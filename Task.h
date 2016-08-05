/****************************************************************************
*																			*
*	File:		Task.h														*
*																			*
*	Author:		Branch Vincent												*
*																			*
*	Date:		Jun 6, 2016													*
*																			*
*	Purpose:	This file defines the Task class.							*
*																			*
****************************************************************************/

#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include "Parameters/Parameters.h"

using namespace std;
using namespace params;

/****************************************************************************
*																			*
*	Definition of Task class												*
*																			*
****************************************************************************/

// Notes
// - Fix traffic

class Task
{
//	Public member functions

	public:

	//	Constructor
		
		Task(int tp, float prevArrTime, int phase);

	//	Inspectors

//		int getType() const {return type;}
		const int& getType() {return type;}
		const int& getPriority() {return priority;}
		const float& getArrTime() {return arrTime;}
		const float& getSerTime() {return serTime;}
		const float& getDepTime() {return depTime;}
		const float& getExpTime() {return expTime;}
        const float& getBegTime() {return begTime;}
        const float& getQueTime() {return queTime;}
        const float& getSerLeft() {return serLeft;}
//        const float getPercLeft() {return serLeft/serTime;}
        const int getOpNum() {return opNum;}

	//	Mutators
	
		void setArrTime(float t) {arrTime = t;}
		void setSerTime(float t) {serTime = t;}
		void setDepTime(float t) {depTime = t;}
        void setSerLeft(float t) {serLeft = t;}
        void setBegTime(float t) {begTime = t;}
        void setQueTime(float t) {queTime = t;}
        void setOpNum(int n) {opNum = n;}

	//	Other member functions

		void output(ostream& out) const 
			{cout << "(Pr " << priority << ", Tp " << type << ", Arr " << arrTime;
			cout << ", Ser " << serTime <<  ", Dep " << depTime << ", Exp " << expTime << ")";}
			
//	Private member functions

	private:
		
	//	Used by constructor
	
		int getPriority(int phase);
		float genArrTime(float prevArrTime, int phase, int seed);
		float adjArrForTraff(float prevArrTime, float interArrTime, int phase);
		float genSerTime(int seed);
		float genExpTime(int phase, int seed);
		float genRandNum(char distType, int seed, float arg1, float arg2 = 0);
        int initOpNum();
	
//	Data members

	private:
		int type;			// type
		int priority;		// priority level
		float arrTime;		// arrival time (min)
		float serTime; 		// service time (min)
		float depTime;		// depature time (min)
		float expTime;		// expiration time (min)
		float begTime;		// begin service time (min)
        float queTime;      // enter queue time (min)
        float serLeft;      // service time left
        int opNum;          // operator id number
};

//	Operators

ostream& operator<<(ostream& out, const Task& t) {t.output(out); return out;}

/****************************************************************************
*																			*
*	Function:	Task														*
*																			*
*	Purpose:	To construct a task of the specified type using the 		*
*				specified previous arrival time and distribution seed		*
*																			*
****************************************************************************/

Task::Task(int tp, float prevArrTime, int phase) :
    type(tp),
    priority(getPriority(phase)),
    arrTime(genArrTime(prevArrTime, phase, rand())),
    serTime(genSerTime(rand())),
    depTime(INFINITY),
    expTime(genExpTime(phase, rand())),
    begTime(0),
    queTime(arrTime),
    serLeft(serTime),
    opNum(initOpNum())
{
//	Check type

	if (tp < 0 || tp > 8)
	{
		cerr << "Error:  Incompatible task type. Exiting..." << endl;
		exit(1);
	}
	
//	Check phase

	else if (phase < 0 || phase > 2)
	{
		cerr << "Error:  Incompatible phase. Exiting..." << endl;
		exit(1);
	}
}

/****************************************************************************
*																			*
*	Function:	getPriority													*
*																			*
*	Purpose:	To return the priority based on the task type and specified *
*				phase			 											*
*																			*
****************************************************************************/

int Task::getPriority(int phase)
{	
//	Priority array (task types vs. phases)

						//	P0,	P1,	P2
	int prty[9][3] = 	{	{4, 3, 	4},		// Communicating
							{5, 4, 	5}, 	// Exception handling
							{3,	0, 	2},		// Paperwork
							{0, 5, 	0},		// Maintenance of way
							{0, 5, 	0},		// Temp speed restriction
							{0, 5, 	0},		// Signal response management
							{2, 2, 	1},		// Monitoring inside
							{1, 1, 	3},		// Monitoring outisde
							{0, 5, 	0}		// Planning ahead
					 	};	
					
	return prty[type][phase];
}

/****************************************************************************
*																			*
*	Function:	genArrTime													*
*																			*
*	Purpose:	To generate an arrival time based on the specified task		*
*				type, previous arrival time, and distribution seed			*
*																			*
****************************************************************************/

float Task::genArrTime(float prevArrTime, int phase, int seed)
{	
//	Exponential distribution parameters (task types vs. phases)

							//	P0,		P1,		P2
	float lambda[9][3] = 	{	{1/3., 	0.2, 	1/3.},		// Communicating
								{0, 	.1/30, 	.1/30}, 	// Exception handling
								{1/3., 	1/30., 	1/3.},		// Paperwork
								{0, 	.05/30, .5/30},		// Maintenance of way
								{0, 	1/30., 	0},			// Temp speed restriction
								{1/30., 0.1, 	1/15.},		// Signal response management
								{1/15., 0.1, 	0},			// Monitoring inside
								{0.1, 	0.2, 	0.1},		// Monitoring outisde
								{1/30., 1/15., 1/30.}		// Planning ahead
					 	 	};	

//	Generate random interarrival time and adjust for traffic

	float interArrTime = genRandNum('E', seed, lambda[type][phase]);
	float arrival = adjArrForTraff(prevArrTime, interArrTime, phase);

	return arrival;
}

/****************************************************************************
*																			*
*	Function:	adjArrForTraffic											*
*																			*
*	Purpose:	To adjust the arrival time based on the current traffic		* 
*				level														*
*																			*
****************************************************************************/

float Task::adjArrForTraff(float prevArrTime, float interArrTime, int phase)
{
//	Tasks affected by traffic level (task types vs. phases)

								//	P0, P1,	P2
	bool affByTraff[9][3] = 	{	{0,	1,	0},		// Communicating
									{0, 1, 	0}, 	// Exception handling
									{0, 1, 	1},		// Paperwork
									{0, 1, 	0},		// Maintenance of way
									{0, 1, 	0},		// Temp speed restriction
									{0, 1, 	0},		// Signal response management
									{0, 0, 	0},		// Monitoring inside
									{0, 1, 	0},		// Monitoring outisde
									{0, 1, 	0}		// Planning ahead
					 		 	};	

    float newArrTime = prevArrTime + interArrTime;
    
	if (isinf(newArrTime)) return INFINITY;
    
//	Adjust arrival time, if applicable

	if (affByTraff[type][phase] && TRAFFIC_ON) 
	{
        float budget = interArrTime;
        float currTime = prevArrTime;
        int currHour = currTime/60;
        float traffLevel = TRAFFIC[currHour];
        float timeToAdj = (currHour + 1) * 60 - currTime;
        float adjTime = timeToAdj * traffLevel;
    
    //  If time is left in budget, proceed
        
        while (budget > adjTime)
        {
        //  Decrement budget
            
            budget -= adjTime;
            
        //	Calculate new values
            
            currTime += timeToAdj;
            currHour++;
            
            if (currHour >= TRAFFIC.size())
            {
                if (DEBUG_ON) cout << "OVERFLOW" << endl;
                return INFINITY;
            }
            
            traffLevel = TRAFFIC[currHour];
            timeToAdj = (currHour + 1) * 60 - currTime;
            adjTime = timeToAdj * traffLevel;
        }
        
        newArrTime = currTime + budget/traffLevel;
    }
    
	return newArrTime;
}

/****************************************************************************
*																			*
*	Function:	genSerTime													*
*																			*
*	Purpose:	To generate a service time based on the specified task type	* 
*				and distribution seed										*
*																			*
****************************************************************************/

float Task::genSerTime(int seed)
{
//	Generate service time, based on type

	switch (type)
	{
		case 0:	return genRandNum('E', seed, 1/0.133);				// Communicating
		case 1:	return genRandNum('L', seed, 0.980297, 1.389685);	// Exception handling
		case 2: return genRandNum('U', seed, 0.05, 2);				// Paperwork	
		case 3: return genRandNum('U', seed, 0.167, 1);				// Maintenance of way
		case 4:	return genRandNum('U', seed, 0.1, 0.5);				// Temp speed restriction
		case 5:	return genRandNum('U', seed, 0.1, 0.5);				// Signal response management
		case 6:	return genRandNum('E', seed, 1/0.133);				// Monitoring inside
		case 7:	return genRandNum('E', seed, 1/0.1);				// Monitoring outside
		case 8:	return genRandNum('E', seed, 1/0.33);				// Planning ahead
	
	//	Error message
		
		default:
		{
			cerr << "Error:  Incompatible task type. Exiting..." << endl;
			exit(1);
		}
	}
}

/****************************************************************************
*																			*
*	Function:	genExpTime													*
*																			*
*	Purpose:	To generate an expiration time based on the specified task	*
*				type, ..., and distribution seed			*
*																			*
****************************************************************************/

float Task::genExpTime(int phase, int seed)
{
//	Exponential distribution parameters (task types vs. phases)						* returns infinity for lda = 0

//	High traffic
									//	P0,	P1,				P2
	float highTraffLambda[9][3] = 	{	{0,	0.086333333,	0},				// Communicating
										{0,	0.055166667,	0.055166667}, 	// Exception handling
										{0,	0,				0},				// Paperwork
										{0,	0.184,			0},				// Maintenance of way
										{0,	0.184,			0},				// Temp speed restriction
										{0,	0.184,			0.184},			// Signal response management
										{0,	0,				0},				// Monitoring inside
										{0,	0,				0},				// Monitoring outisde
										{0,	0.1795,			0}				// Planning ahead
							 	 	};

//	Low and normal traffic
								//	P0,	P1,				P2
	float otherLambda[9][3] =	{	{0,	0.107166667,	0},				// Communicating
									{0,	0.044666667,	0.044666667}, 	// Exception handling
									{0,	0,				0},				// Paperwork
									{0,	0.184,			0},				// Maintenance of way
									{0,	0.184,			0},				// Temp speed restriction
									{0,	0.184,			0.184},			// Signal response management
									{0,	0,				0},				// Monitoring inside
									{0,	0,				0},				// Monitoring outisde
									{0,	0.166,			0}				// Planning ahead
						 	 	};		
	
	float lambda;
	float expiration = 0;
	int hour = arrTime/60;
	
//	Set lambda
	
	if (hour >= TRAFFIC.size())
		return arrTime;
	else if (TRAFFIC[hour] == 2)
		lambda = highTraffLambda[type][phase];
	else
		lambda = otherLambda[type][phase];
	
//	Get random number

	while (expiration <= serTime)
		expiration = genRandNum('E', seed++, lambda);

	return arrTime + expiration;
}

/****************************************************************************
*																			*
*	Function:	genRandNum													*
*																			*
*	Purpose:	To generate a random number based on the specified 			*
*				distribution type and seed									*
*																			*
****************************************************************************/

float Task::genRandNum(char distType, int seed, float arg1, float arg2)
{
//	Initialize generator

	default_random_engine gen(seed);
	
//	Return random number, based on distribution type

	switch (distType)
	{
	//	Exponential
	
		case 'E': 
		{
			exponential_distribution<float> dist(arg1);			// args = lambda
			return dist(gen);
		}
		
	//	Lognormal
	
		case 'L': 
		{
			lognormal_distribution<double> dist(arg1, arg2);	// args = mean, stddev
			return dist(gen);
		}
		
	//	Uniform
	
		case 'U': 
		{	
			uniform_real_distribution<float> dist(arg1, arg2);	// args = min, max
			return dist(gen);
		}
		
	//	Error message
	
		default:
		{
			cerr << "Error:  Incompatible distribution type. Exiting..." << endl;
			exit(1);
		}
	}
}

/****************************************************************************
*																			*
*	Function:	initOpNum													*
*																			*
*	Purpose:	To initialize the operator number                           *
*																			*
****************************************************************************/

int Task::initOpNum()
{
    int opNum[9] = 	{	0,          // Communicating
                        0,          // Exception handling
                        0,          // Paperwork
                        0,          // Maintenance of way
                        0,          // Temp speed restriction
                        NUM_OPS,	// Signal response management
                        NUM_OPS,	// Monitoring inside
                        0,          // Monitoring outisde
                        NUM_OPS     // Planning ahead
                    };
				
    return opNum[type];
}

#endif