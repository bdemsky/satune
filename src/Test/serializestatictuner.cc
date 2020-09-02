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
		elem_bin->setTunable(PREPROCESS, &onoff, 1);
		elem_bin->setTunable(DECOMPOSEORDER, &onoff, 1);
		elem_bin->setTunable(ORDERINTEGERENCODING, &offon, 0);
		elem_bin->setTunable(ELEMENTOPT, &onoff, 1);
		elem_bin->setTunable(ELEMENTOPTSETS, &onoff, 1);
		elem_bin->setTunable(ENCODINGGRAPHOPT, &offon, 0);
		elem_bin->setVarTunable(0, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
		elem_bin->setVarTunable(1, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
		elem_bin->setVarTunable(2, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
		elem_bin->setTunable(VARIABLEORDER, &boolVarOrderingDesc, REVERSEORDERING);
		elem_bin->setTunable(PROXYVARIABLE, &proxyparameter, 2);
		elem_bin->setVarTunable(0, MUSTREACHGLOBAL, &onoff, 1);
		elem_bin->setVarTunable(1, MUSTREACHGLOBAL, &onoff, 1);
		elem_bin->setVarTunable(2, MUSTREACHGLOBAL, &onoff, 1);
		elem_bin->setVarTunable(0, MUSTREACHLOCAL, &onoff, 1);
		elem_bin->setVarTunable(1, MUSTREACHLOCAL, &onoff, 1);
		elem_bin->setVarTunable(2, MUSTREACHLOCAL, &onoff, 1);
		elem_bin->setVarTunable(0, MUSTREACHPRUNE, &onoff, 1);
		elem_bin->setVarTunable(1, MUSTREACHPRUNE, &onoff, 1);
		elem_bin->setVarTunable(2, MUSTREACHPRUNE, &onoff, 1);
		elem_bin->setVarTunable(0, MUSTEDGEPRUNE, &onoff, 1);
		elem_bin->setVarTunable(1, MUSTEDGEPRUNE, &onoff, 1);
		elem_bin->setVarTunable(2, MUSTEDGEPRUNE, &onoff, 1);
		elem_bin->setTunable(OPTIMIZEORDERSTRUCTURE, &onoff, 1);
		elem_bin->setTunable(MUSTVALUE, &mustValueBinaryIndex, 3);
		elem_bin->serialize("binarytuner.conf");

		elem_onehot->setTunable(NAIVEENCODER, &NaiveEncodingDesc, ONEHOT);
		elem_onehot->setTunable(PREPROCESS, &onoff, 1);
		elem_onehot->setTunable(DECOMPOSEORDER, &onoff, 1);
		elem_onehot->setTunable(ORDERINTEGERENCODING, &offon, 0);
		elem_onehot->setTunable(ELEMENTOPT, &onoff, 1);
		elem_onehot->setTunable(ELEMENTOPTSETS, &onoff, 1);
		elem_onehot->setTunable(ENCODINGGRAPHOPT, &offon, 0);
		elem_onehot->setVarTunable(0, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
		elem_onehot->setVarTunable(1, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
		elem_onehot->setVarTunable(2, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
		elem_onehot->setTunable(VARIABLEORDER, &boolVarOrderingDesc, REVERSEORDERING);
		elem_onehot->setTunable(PROXYVARIABLE, &proxyparameter, 2);
		elem_onehot->setVarTunable(0, MUSTREACHGLOBAL, &onoff, 1);
		elem_onehot->setVarTunable(1, MUSTREACHGLOBAL, &onoff, 1);
		elem_onehot->setVarTunable(2, MUSTREACHGLOBAL, &onoff, 1);
		elem_onehot->setVarTunable(0, MUSTREACHLOCAL, &onoff, 1);
		elem_onehot->setVarTunable(1, MUSTREACHLOCAL, &onoff, 1);
		elem_onehot->setVarTunable(2, MUSTREACHLOCAL, &onoff, 1);
		elem_onehot->setVarTunable(0, MUSTREACHPRUNE, &onoff, 1);
		elem_onehot->setVarTunable(1, MUSTREACHPRUNE, &onoff, 1);
		elem_onehot->setVarTunable(2, MUSTREACHPRUNE, &onoff, 1);
		elem_onehot->setVarTunable(0, MUSTEDGEPRUNE, &onoff, 1);
		elem_onehot->setVarTunable(1, MUSTEDGEPRUNE, &onoff, 1);
		elem_onehot->setVarTunable(2, MUSTEDGEPRUNE, &onoff, 1);
		elem_onehot->setTunable(OPTIMIZEORDERSTRUCTURE, &onoff, 1);
		elem_onehot->setTunable(MUSTVALUE, &mustValueBinaryIndex, 3);
		elem_onehot->serialize("onehottuner.conf");
		elem_onehot->setTunable(ONEHOTATMOSTONE, &OneHotAtMostOneDesc, ONEHOT_COMMANDER);
		elem_onehot->serialize("onehottunercommander.conf");
		elem_unary->setTunable(NAIVEENCODER, &NaiveEncodingDesc, UNARY);
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
                elem_bin->setTunable(PREPROCESS, &onoff, 1);
                elem_bin->setTunable(DECOMPOSEORDER, &onoff, 1);
                elem_bin->setTunable(ELEMENTOPT, &onoff, 1);
                elem_bin->setTunable(ELEMENTOPTSETS, &onoff, 1);
                elem_bin->setTunable(ENCODINGGRAPHOPT, &onoff, 1);
		elem_bin->setVarTunable(0, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
                elem_bin->setVarTunable(1, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
                elem_bin->setVarTunable(2, NODEENCODING, &NodeEncodingDesc, ELEM_UNASSIGNED);
                elem_bin->setTunable(VARIABLEORDER, &boolVarOrderingDesc, REVERSEORDERING);
                elem_bin->setTunable(PROXYVARIABLE, &proxyparameter, 2);
                elem_bin->setVarTunable(0, MUSTREACHGLOBAL, &onoff, 1);
                elem_bin->setVarTunable(1, MUSTREACHGLOBAL, &onoff, 1);
                elem_bin->setVarTunable(2, MUSTREACHGLOBAL, &onoff, 1);
                elem_bin->setVarTunable(0, MUSTREACHLOCAL, &onoff, 1);
                elem_bin->setVarTunable(1, MUSTREACHLOCAL, &onoff, 1);
                elem_bin->setVarTunable(2, MUSTREACHLOCAL, &onoff, 1);
                elem_bin->setVarTunable(0, MUSTREACHPRUNE, &onoff, 1);
                elem_bin->setVarTunable(1, MUSTREACHPRUNE, &onoff, 1);
                elem_bin->setVarTunable(2, MUSTREACHPRUNE, &onoff, 1);
                elem_bin->setVarTunable(0, MUSTEDGEPRUNE, &onoff, 1);
                elem_bin->setVarTunable(1, MUSTEDGEPRUNE, &onoff, 1);
                elem_bin->setVarTunable(2, MUSTEDGEPRUNE, &onoff, 1);
                elem_bin->setTunable(OPTIMIZEORDERSTRUCTURE, &onoff, 1);
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
