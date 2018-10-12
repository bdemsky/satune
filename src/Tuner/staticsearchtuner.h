#ifndef STATICSEARCHTUNER_H
#define STATICSEARCHTUNER_H
#include "searchtuner.h"
#define TUNEFILE "tune.conf"

class StaticSearchTuner : public SearchTuner {
public:
	StaticSearchTuner();
	~StaticSearchTuner();
	int nextStaticTuner();
	StaticSearchTuner *copyUsed();

	CMEMALLOC;
private:
	bool graphEncoding;
	ElementEncodingType naiveEncoding;
};

#endif
