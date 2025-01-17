#include "csolver.h"
#include "serializetuner.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
	if (argc != 5) {
		printf("You only specify the name of the file ...");
		exit(-1);
	}
	char buffer[512];
	CSolver *solver = CSolver::deserialize(argv[1]);
	uint timeout;
	sscanf(argv[2], "%u", &timeout);
	SerializeTuner *tuner = new SerializeTuner(argv[3]);
	solver->setTuner(tuner);
	solver->setSatSolverTimeout(timeout);
	int sat = solver->solve();
	long long metric = solver->getElapsedTime();
	ofstream myfile;
	myfile.open (argv[4], ios::out | ios::trunc);
	myfile << metric << endl;
	myfile << sat << endl;
	myfile.close();
	//serialize out the tuner we used
	snprintf(buffer, sizeof(buffer), "%sused", argv[3]);
	tuner->serializeUsed(buffer);

	delete solver;
	delete tuner;
	return 0;
}
