#include "csolver.h"
#include "comptuner.h"
#include "searchtuner.h"
#include "dirent.h"

uint getNumberOfRuns(){
	uint runs = 0;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("./")) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if(strstr(ent->d_name, "result") != NULL){
				runs++;
			}
		}
		closedir (dir);
	} else {
		perror ("Unable to open the directory\n");
		exit(1);
	}
	return runs;
}

int main(int argc, char **argv) {
	uint numruns = getNumberOfRuns();
	printf("Number of Runs: %u\n", numruns);
	CompTuner *multituner = new CompTuner(0, 0);
	multituner->readData(numruns);
	multituner->findBestTwoTuners();
	delete multituner;
	return 0;
}
