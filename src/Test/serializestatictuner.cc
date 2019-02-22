#include "csolver.h"
#include "searchtuner.h"
#include "tunable.h"
#include <stdlib.h>

int main(int argc, char **argv) {
	SearchTuner *elem_bin = new SearchTuner();
	SearchTuner *elem_onehot = new SearchTuner();
	SearchTuner *elem_unary = new SearchTuner();
	elem_bin->setTunable(NAIVEENCODER, &NaiveEncodingDesc, BINARYINDEX);
	elem_onehot->setTunable(NAIVEENCODER, &NaiveEncodingDesc, ONEHOT);
	elem_unary->setTunable(NAIVEENCODER, &NaiveEncodingDesc, UNARY);

	elem_bin->serialize("binarytuner.conf");
	elem_onehot->serialize("onehottuner.conf");
	elem_unary->serialize("unarytuner.conf");
	elem_bin->setTunable(ENCODINGGRAPHOPT, &onoff, 1);
	elem_onehot->setTunable(ENCODINGGRAPHOPT, &onoff, 1);
	elem_unary->setTunable(ENCODINGGRAPHOPT, &onoff, 1);
	elem_bin->serialize("circuitbinarytuner.conf");
	elem_onehot->serialize("circuitonehottuner.conf");
	elem_unary->serialize("circuitunarytuner.conf");
	delete elem_bin;
	delete elem_onehot;
	delete elem_unary;
}
