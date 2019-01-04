/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   basictuner.h
 * Author: hamed
 *
 * Created on December 17, 2018, 2:02 PM
 */

#ifndef BASICTUNER_H
#define BASICTUNER_H
#include "mymemory.h"
#include "classlist.h"
#include "cppvector.h"
#include "hashtable.h"

#define TUNERUNSETVALUE -1
#define AUTOTUNERFACTOR 0.3

class Problem {
public:
	Problem(const char *problem);
	char *getProblem() {return problem;}
	inline int getResult(){return result;}
	inline int getProblemNumber(){return problemnumber;}
	inline void setResult(int res){result = res;}
	inline void setProblemNumber(int probNum){problemnumber = probNum;}
	inline long long getBestTime() {return besttime ;} 
	inline void setBestTime(long long time) {besttime = time;}
	~Problem();
	CMEMALLOC;
private:
	int problemnumber;
	int result;
	char *problem;
	long long besttime;
};

class TunerRecord {
public:
	TunerRecord(SearchTuner *_tuner) : tuner(_tuner), tunernumber(-1), isduplicate(false) {}
	TunerRecord(SearchTuner *_tuner, int _tunernumber) : tuner(_tuner), tunernumber(_tunernumber), isduplicate(false) {}
	SearchTuner *getTuner() {return tuner;}
	void inline addProblem(Problem * problem){problems.push(problem);}
	TunerRecord *changeTuner(SearchTuner *_newtuner);
	void updateTuner(SearchTuner *_newtuner) {tuner = _newtuner;}
	long long getTime(Problem *problem);
	void setTime(Problem *problem, long long time);
	inline void setTunerNumber(int numb){tunernumber = numb;}
	inline int getTunerNumber(){return tunernumber;}
	inline uint problemsSize() {return problems.getSize();}
	inline void setDuplicate(bool _duplicate) { isduplicate = _duplicate;}
	inline bool isDuplicate() {return isduplicate;}
	inline Problem *getProblem(uint index){return problems.get(index);}
        void print();
	void printProblemsInfo();
	CMEMALLOC;
private:
	SearchTuner *tuner;
	Vector<Problem *> problems;
	Hashtable<Problem *, long long, uint64_t> timetaken;
	int tunernumber;
	friend void clearVector(Vector<TunerRecord *> *tunerV);
	bool isduplicate;
};

class BasicTuner {
public:
	BasicTuner(uint _budget, uint _timeout);
	void addProblem(const char *filename);
	void addTuner(SearchTuner *tuner);
	void printData();
	virtual ~BasicTuner();
	virtual void tune() = 0;
	CMEMALLOC;
protected:
	long long evaluate(Problem *problem, TunerRecord *tuner);
	/**
         * returns the index of the tuner which is subtune of
         * the newTuner 
         * @param newTuner
         * @return 
         */
        int subTunerIndex(SearchTuner *newTuner);
	bool tunerExists(TunerRecord *tunerRec);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
	void updateTimeout(Problem *problem, long long metric);
	Vector<TunerRecord *> allTuners;
	Vector<TunerRecord *> explored;
	Vector<Problem *> problems;
	Vector<TunerRecord *> tuners;
	uint budget;
	uint timeout;
	int execnum;
};

#endif /* BASICTUNER_H */

