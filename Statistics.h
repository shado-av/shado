/****************************************************************************
*																			*
*	File:		Statistics.h												*
*																			*
*	Author:		Branch Vincent												*
*																			*
*	Date:		Jun 28, 2016												*
*																			*
*	Purpose:	This file defines the Statistics class.						*
*																			*
****************************************************************************/

#ifndef STATISTICS_H
#define STATISTICS_H

#include <iostream>
#include <fstream>
#include <string>
#include "Statistic.h"
//#include "PyPlot.h"

using namespace std;
using namespace params;

/****************************************************************************
*																			*
*	Definition of Statistics class											*
*																			*
****************************************************************************/

class Statistics
{
//	Public member functions

	public:
		
	//	Constructor
	
		Statistics(int xDim = NUM_TASK_TYPES,
                   int yDim = NUM_INTS,
                   int zDim = NUM_REPS);
		
	//	Inspectors

//		int getInterval(int i) {return interval[i];}
		float getUtil(int i) {return util.getData(i, currRep);}
//		float& getserTime(int i, int j) {return serTime[currRep][i][j];}
//		float& getwaitTime(int i, int j) {return waitTime[i][j];}
//		float& getNumTasksIn(int i, int j) {return numTasksIn[currRep][i][j];} 
//		float& getNumTasksOut(int i, int j) {return numTasksOut[i][j];}

	//	Mutators

        void incUtil(int i, int j, float val) {
//			cout << "\t\t\tutil(" << j << ", " << currRep << "): " << util.getData(j, currRep);
//			cout << " + " << val << " = ";
			util.incData(i, j, currRep, val);
//			cout << util.getData(j, currRep) << endl;
//			float utl = util.getData(j, currRep);
//			cout << "Util(" << j << ") = " << utl << endl;
			if (util.getData(j, currRep) > 1.01)
			{
				cout << "UTILIZATION OVERFLOW" << endl;
				cout << util.getData(j, currRep) << endl;
				cout << "Exiting" << endl;
			}
		}
	
		void calcPenalty(int rep);

        void incSerTime(int i, int j, float val) {serTime.incData(i, j, currRep, val);}
		void incWaitTime(int i, int j, float val) {waitTime.incData(i, j, currRep, val);}
		void incNumTasksIn(int i, int j, float val) {numTasksIn.incData(i, j, currRep, val);}
		void incNumTasksOut(int i, int j, float val) {numTasksOut.incData(i, j, currRep, val);}
		void incNumTasksExp(int i, int j, float val) {numTasksExp.incData(i, j, currRep, val);}
		void endRep();

//		void setUtil(const int& i, const int& j, const int& val) {util[i][j] = val;}
//		void setSerTime(const int& i, const int& j, const int& val) {serTime[i][j] = val;}
//		void setWaitTime(const int& i, const int& j, const int& val) {waitTime[i][j] = val;}
//		void setNumTasksIn(const int& i, const int& j, const int& val) {numTasksIn[i][j] = val;}
//		void setNumTasksOut(const int& i, const int& j, const int& val) {numTasksOut[i][j] = val;}
				
	//	Other member functions

//        void plot(string opName);
        void output(ostream& out) const {outputSim(out); return;}
		void outputIdle(ostream& out) const;
		void outputBusy(ostream& out) const;

//	Private member functions

	private:
        void outputRep(ostream& out, int rep) const;
        void outputSim(ostream& out) const;
        void endSim();
		void outputByRep(ostream& out, Statistic stat) const;
		void outputByRep2(ostream& out, Statistic stat) const;


//	Data members

	private:
		int currRep;			// current run counter
		vector<int> interval;	// time interval
		Statistic util;			// utilization
		Statistic serTime;      // average service time
		Statistic waitTime;		// average wait time in queue
		Statistic numTasksIn;	// number of tasks in
		Statistic numTasksOut;	// number of tasks out
		Statistic numTasksExp;	// number of tasks expired
		vector<float> busyPenalty;
		vector<float> idlePenalty;
};

//	Operators

ostream& operator<<(ostream& out, const Statistics& stats) {stats.output(out); return out;}

/****************************************************************************
*																			*
*	Function:	Statistics													*
*																			*
*	Purpose:	To construct a new Statistics								*
*																			*
****************************************************************************/

Statistics::Statistics(int xDim, int yDim, int zDim) :
    currRep(0),
    interval(yDim),
	util("Utilization", xDim, yDim, zDim),
	serTime("Service Time", xDim, yDim, zDim),
	waitTime("Wait Time", xDim, yDim, zDim),
	numTasksIn("Number In", xDim, yDim, zDim),
	numTasksOut("Number Out", xDim, yDim, zDim),
	numTasksExp("Number Expired", xDim, yDim, zDim),
	busyPenalty(zDim, 0),
	idlePenalty(zDim, 0)
{
//  Set time intervals
    
	for (int i = 1; i < interval.size(); i++)
		interval[i] = interval[i-1] + INT_SIZE;
}

/****************************************************************************
*																			*
*	Function:	endRep														*
*																			*
*	Purpose:	To output the stats for the current replication and move to *
*               the next replication                                        *
*																			*
****************************************************************************/

void Statistics::endRep()
{
//  Output statistics for current replication
    
    if (false) //DEBUG_ON)
    {
        string file = OUTPUT_PATH + "/rep" + to_string(currRep) + ".csv";
        ofstream fout(file);
        if (!fout)
        {
            cerr << "Error: Cannot open " << file << ". Exiting..." << endl;
            exit(1);
        }
        outputRep(fout, currRep);
    }
	
//	Calculate penalty
	
	calcPenalty(currRep);

//	Move to next replication and end simulation, if applicable
    
    if (++currRep == NUM_REPS) endSim();
}

/****************************************************************************
*																			*
*	Function:	outputRep													*
*																			*
*	Purpose:	To output the stats for the specified replication			*
*																			*
****************************************************************************/

void Statistics::outputRep(ostream& out, int rep) const
{
//	Output header
    
    out << "Statistic, Task, ";
    
    for (int i = 0; i < interval.size(); i++)
        out << interval[i] << " min, ";
    out << "Sum" << endl;
    
//	Output stats
    
    util.outputRep(out, rep);
    serTime.outputRep(out, rep);
    waitTime.outputRep(out, rep);
    numTasksIn.outputRep(out, rep);
    numTasksOut.outputRep(out, rep);
    numTasksExp.outputRep(out, rep);
    
    return;
}

/****************************************************************************
*																			*
*	Function:	outputSim													*
*																			*
*	Purpose:	To output the stats averaged across all replications        *
*																			*
****************************************************************************/

void Statistics::outputSim(ostream& out) const
{
//	Output header
	
    out << "Statistic, Task";
    
    for (int i = 0; i < interval.size(); i++)
        out << ", " << interval[i] << " min";
    out << "," << endl;
//    out << ", Sum" << endl;
    
//  Output arrays
	
    util.outputSim(out);
    serTime.outputSim(out);
    waitTime.outputSim(out);
    numTasksIn.outputSim(out);
    numTasksOut.outputSim(out);
    numTasksExp.outputSim(out);
	
//	Utilization
	
	out << "Data by Rep";
	outputByRep(out, util);
	outputByRep2(out, waitTime);
	outputByRep2(out, numTasksExp);
	
	return;
}

void Statistics::outputByRep(ostream& out, Statistic stat) const
{
	out << ", Time (min)";
	for (int i = 0; i < NUM_REPS; i++)
		out << ", Rep " << i;
	out << endl << stat.getName();
	
	for (int i = 0; i < NUM_INTS; i++)
	{
		out << ", " << i * INT_SIZE;
		for (int j = 0; j < NUM_REPS; j++)
			out << ", " << stat.getData(i, j);
		out << "," << endl;
	}
	
    return;
}

void Statistics::outputByRep2(ostream& out, Statistic stat) const
{

//	Initialize variables
	
	Matrix2D<float> avg(NUM_TASK_TYPES + 1, vector<float>(NUM_REPS, 0));
	Matrix2D<float> dev(NUM_TASK_TYPES + 1, vector<float>(NUM_REPS, 0));
	
	int N = 0;
	float mean = 0;
	float devSum = 0;
	float delta;
	
//  Calculate mean and std dev across all replications
	
	for (int i = 0; i < NUM_TASK_TYPES + 1; i++)
	{
		for (int j = 0; j < NUM_REPS; j++)
		{
		//  Reset variables
			
			N = 0;
			mean = 0;
			devSum = 0;
			
		//  Calculate values using Welford's algorithm
			
			for (int k = 0; k < NUM_INTS; k++)
			{
				N++;
				delta = stat.getData(i, k, j) - mean;
				mean += delta/N;
				devSum += delta * (stat.getData(i, k, j) - mean);
			}
			
		//  Store values
			
			avg[i][j] = mean;
			dev[i][j] = sqrt(devSum / (N - 1));
		}
	}
	
	out << ", Task";
	for (int i = 0; i < NUM_REPS; i++)
		out << ", Rep " << i;
	out << endl << stat.getName();
	
	for (int i = 0; i < NUM_TASK_TYPES + 1; i++)
	{
		if (i != NUM_TASK_TYPES)
			out << ", " << i;
		else
			out << ",Sum";
		
		for (int j = 0; j < NUM_REPS; j++)
			out << ", " << avg[i][j];
		out << endl << ",";
		
		for (int j = 0; j < NUM_REPS; j++)
			out << ", " << dev[i][j];
		out << endl;
	}
	
	return;
}

/****************************************************************************
*																			*
*	Function:	endSim														*
*																			*
*	Purpose:	To average the stats across all replications                *
*																			*
****************************************************************************/

void Statistics::endSim()
{
//	Calculate mean and std dev across all replications
    
    util.avgData();
    serTime.avgData();
    waitTime.avgData();
    numTasksIn.avgData();
    numTasksOut.avgData();
    numTasksExp.avgData();
    
    return;
}

/****************************************************************************
*																			*
*	Function:	plot                                                        *
*																			*
*	Purpose:	To plot the utilization										*
*																			*
****************************************************************************/

//void Statistics::plot(string opName)
//{
//    int lastRow = util[0].size() - 1;
//    int lastCol = util[0][0].size() - 1;
//
//    vector<float> time;
//    vector<float> util;
//    vector<float> err;
//
//    for (int i = 0; i < interval.size(); i++)
//    {
//        time.push_back(interval[i] + INT_SIZE/2);
//        util.push_back(stats[i][NUM_REPS]);
//        err.push_back(stats[i][NUM_REPS + 1]);
//    }
//
////  Plot using matplotlib
//
//    PyPlot plt;
//    plt.set_title(opName + " Utilization for " + to_string(NUM_REPS) + " Reps");
//    plt.set_xlabel("Time (min)");
//    plt.set_ylabel("Utilization");
//    plt.set_xlim(0, END_TIME);
//    plt.set_ylim(0, 1.1);
//    plt.set_axis();
//    plt.plot_bar(time, util, err);
//    for (int i = 0; i < NUM_HOURS; i++)
//    {
//        string hr = to_string(60 * (i + 1));
//        plt.run_cmd("plt.plot((" + hr + ", " + hr + "), (0, 1.10), 'm--')");
//    }
//    plt.save_fig(OUTPUT_PATH + "/" + opName + "_Util.pdf");
//
//    return;
//}

/****************************************************************************
*																			*
*	Function:	calcPenalty                                                 *
*																			*
*	Purpose:	To calculate the penalty									*
*																			*
****************************************************************************/

void Statistics::calcPenalty(int rep)
{
	for (int i = 0; i < interval.size(); i++)
	{
		float currUtil = util.getData(NUM_TASK_TYPES, i, currRep);
		
		if (currUtil < 0.3)
			idlePenalty[rep] += 1 - (10/3) * currUtil;
		else if (currUtil > 0.7)
			busyPenalty[rep] += (10/3) * currUtil - (7/3);
	}
	
	return;
}

void Statistics::outputIdle(ostream& out) const
{
//	Output most idle trip
	
	float max = idlePenalty[0];
	float maxIndex = 0;
	
	for (int i = 1; i < idlePenalty.size(); i++)
	{
	//		cout << idlePenalty[i] << " ";
		
		if (idlePenalty[i] > max)
		{
			max = idlePenalty[i];
			maxIndex = i;
		}
	}

	outputRep(out, maxIndex);
}

void Statistics::outputBusy(ostream& out) const
{
	//	Output most idle trip
	
	float max = busyPenalty[0];
	float maxIndex = 0;
	
	for (int i = 1; i < busyPenalty.size(); i++)
	{
		//		cout << busyPenalty[i] << " ";
		
		if (busyPenalty[i] > max)
		{
			max = busyPenalty[i];
			maxIndex = i;
		}
	}
	
	outputRep(out, maxIndex);
}

#endif