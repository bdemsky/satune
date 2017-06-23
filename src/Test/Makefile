BASE := ..

OBJECTS := $(patsubst %.c, ../bin/%, $(wildcard *.c))

include $(BASE)/common.mk

DEPS := $(join $(addsuffix ., $(dir $(OBJECTS))), $(addsuffix .d, $(notdir $(OBJECTS))))

CPPFLAGS += -I$(BASE) -I$(BASE)/AST -I$(BASE)/Collections

all: $(OBJECTS) ../bin/run.sh

-include $(DEPS)

../bin/%: %.c
	$(CC) -MMD -MF $(@D)/.$(@F).d -o ../bin/$@ $< $(CPPFLAGS) -L$(BASE)/bin/ -l_cons_comp

../bin/run.sh: run.sh
	cp run.sh ../bin/run.sh

clean::
	rm -f $(OBJECTS) $(DEPS) ../bin/run.sh
