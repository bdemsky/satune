BASE := ..

OBJECTS := $(patsubst %.c, %, $(wildcard *.c))

include $(BASE)/common.mk

DEPS := $(join $(addsuffix ., $(dir $(OBJECTS))), $(addsuffix .d, $(notdir $(OBJECTS))))

CPPFLAGS += -I$(BASE) -I$(BASE)/AST -I$(BASE)/Collections

all: $(OBJECTS)

-include $(DEPS)

%: %.c
	$(CC) -MMD -MF $(@D)/.$(@F).d -o $@ $< $(CPPFLAGS) -L$(BASE)/bin/ -l_cons_comp

clean::
	rm -f $(OBJECTS) $(DEPS)
