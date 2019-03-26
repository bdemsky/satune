#include "csolver.h"
#include "searchtuner.h"
#include "tunable.h"
#include <stdlib.h>

int main(int argc, char **argv) {
	{
		SearchTuner *elem_bin = new SearchTuner();
		SearchTuner *elem_onehot = new SearchTuner();
		SearchTuner *elem_unary = new SearchTuner();
		elem_bin->setTunable(NAIVEENCODER, &NaiveEncodingDesc, BINARYINDEX);
		elem_onehot->setTunable(NAIVEENCODER, &NaiveEncodingDesc, ONEHOT);
		elem_unary->setTunable(NAIVEENCODER, &NaiveEncodingDesc, UNARY);

		elem_bin->serialize("binarytuner.conf");
		elem_onehot->serialize("onehottuner.conf");
		elem_unary->serialize("unarytuner.conf");
		elem_bin->setTunable(ENCODINGGRAPHOPT, &offon, 1);
		elem_onehot->setTunable(ENCODINGGRAPHOPT, &offon, 1);
		elem_unary->setTunable(ENCODINGGRAPHOPT, &offon, 1);
		elem_bin->serialize("circuitbinarytuner.conf");
		elem_onehot->serialize("circuitonehottuner.conf");
		elem_unary->serialize("circuitunarytuner.conf");
		elem_bin->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_onehot->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_unary->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_bin->serialize("circuitbinarytunernodecompose.conf");
		elem_onehot->serialize("circuitonehottunernodecompose.conf");
		elem_unary->serialize("circuitunarytunernodecompose.conf");
		delete elem_bin;
		delete elem_onehot;
		delete elem_unary;
	}
	{
		SearchTuner *elem_bin = new SearchTuner();
		SearchTuner *elem_onehot = new SearchTuner();
		SearchTuner *elem_unary = new SearchTuner();
		elem_bin->setTunable(NAIVEENCODER, &NaiveEncodingDesc, BINARYINDEX);
		elem_onehot->setTunable(NAIVEENCODER, &NaiveEncodingDesc, ONEHOT);
		elem_unary->setTunable(NAIVEENCODER, &NaiveEncodingDesc, UNARY);
		elem_bin->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_onehot->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_unary->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_bin->serialize("binarytunernodecompose.conf");
		elem_onehot->serialize("onehottunernodecompose.conf");
		elem_unary->serialize("unarytunernodecompose.conf");
		delete elem_bin;
		delete elem_onehot;
		delete elem_unary;
	}


	{
		SearchTuner *elem_bin = new SearchTuner();
		SearchTuner *elem_onehot = new SearchTuner();
		SearchTuner *elem_unary = new SearchTuner();
		elem_bin->setTunable(NAIVEENCODER, &NaiveEncodingDesc, BINARYINDEX);
		elem_onehot->setTunable(NAIVEENCODER, &NaiveEncodingDesc, ONEHOT);
		elem_unary->setTunable(NAIVEENCODER, &NaiveEncodingDesc, UNARY);
		elem_bin->setTunable(ORDERINTEGERENCODING, &offon, 1);
		elem_onehot->setTunable(ORDERINTEGERENCODING, &offon, 1);
		elem_unary->setTunable(ORDERINTEGERENCODING, &offon, 1);

		elem_bin->serialize("binarytunerint.conf");
		elem_onehot->serialize("onehottunerint.conf");
		elem_unary->serialize("unarytunerint.conf");
		elem_bin->setTunable(ENCODINGGRAPHOPT, &offon, 1);
		elem_onehot->setTunable(ENCODINGGRAPHOPT, &offon, 1);
		elem_unary->setTunable(ENCODINGGRAPHOPT, &offon, 1);
		elem_bin->serialize("circuitbinarytunerint.conf");
		elem_onehot->serialize("circuitonehottunerint.conf");
		elem_unary->serialize("circuitunarytunerint.conf");
		elem_bin->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_onehot->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_unary->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_bin->serialize("circuitbinarytunernodecomposeint.conf");
		elem_onehot->serialize("circuitonehottunernodecomposeint.conf");
		elem_unary->serialize("circuitunarytunernodecomposeint.conf");
		delete elem_bin;
		delete elem_onehot;
		delete elem_unary;
	}
	{
		SearchTuner *elem_bin = new SearchTuner();
		SearchTuner *elem_onehot = new SearchTuner();
		SearchTuner *elem_unary = new SearchTuner();
		elem_bin->setTunable(NAIVEENCODER, &NaiveEncodingDesc, BINARYINDEX);
		elem_onehot->setTunable(NAIVEENCODER, &NaiveEncodingDesc, ONEHOT);
		elem_unary->setTunable(NAIVEENCODER, &NaiveEncodingDesc, UNARY);
		elem_bin->setTunable(ORDERINTEGERENCODING, &offon, 1);
		elem_onehot->setTunable(ORDERINTEGERENCODING, &offon, 1);
		elem_unary->setTunable(ORDERINTEGERENCODING, &offon, 1);
		elem_bin->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_onehot->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_unary->setTunable(DECOMPOSEORDER, &onoff, 0);
		elem_bin->serialize("binarytunernodecomposeint.conf");
		elem_onehot->serialize("onehottunernodecomposeint.conf");
		elem_unary->serialize("unarytunernodecomposeint.conf");
		delete elem_bin;
		delete elem_onehot;
		delete elem_unary;
	}
}
