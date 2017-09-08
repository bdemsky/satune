include common.mk

PHONY += directories
MKDIR_P = mkdir -p
OBJ_DIR = bin

CPP_SOURCES := $(wildcard *.cc) $(wildcard AST/*.cc) $(wildcard ASTTransform/*.cc) $(wildcard Translator/*.cc) $(wildcard ASTAnalyses/*.cc) $(wildcard Tuner/*.cc) $(wildcard Collections/*.cc) $(wildcard Backend/*.cc) $(wildcard Encoders/*.cc) $(wildcard Serialize/*.cc)

C_SOURCES := $(wildcard *.c) $(wildcard AST/*.c) $(wildcard ASTTransform/*.c) $(wildcard Translator/*.c) $(wildcard ASTAnalyses/*.c) $(wildcard Tuner/*.c) $(wildcard Collections/*.c) $(wildcard Backend/*.c) $(wildcard Encoders/*.c) $(wildcard Serialize/*.c)

HEADERS := $(wildcard *.h) $(wildcard AST/*.h) $(wildcard ASTTransform/*.h) $(wildcard Translator/*.h) $(wildcard ASTAnalyses/*.h) $(wildcard Tuner/*.h) $(wildcard Collections/*.h) $(wildcard Backend/*.h) $(wildcard Encoders/*.h) $(wildcard Serialize/*.h)

OBJECTS := $(CPP_SOURCES:%.cc=$(OBJ_DIR)/%.o) $(C_SOURCES:%.c=$(OBJ_DIR)/%.o)

CFLAGS := -Wall -g -O0
CFLAGS += -IAST -IASTTransform -IASTAnalyses -ITranslator -ICollections -IBackend -I. -IEncoders -ITuner -ISerialize
LDFLAGS := -ldl -lrt -rdynamic
SHARED := -shared

# Mac OSX options
ifeq ($(UNAME), Darwin)
LDFLAGS := -ldl
SHARED := -Wl,-undefined,dynamic_lookup -dynamiclib
endif

MARKDOWN := ../docs/Markdown/Markdown.pl

all: directories ${OBJ_DIR}/$(LIB_SO)

directories: ${OBJ_DIR}

${OBJ_DIR}:
	${MKDIR_P} ${OBJ_DIR}
	${MKDIR_P} ${OBJ_DIR}/AST
	${MKDIR_P} ${OBJ_DIR}/ASTAnalyses
	${MKDIR_P} ${OBJ_DIR}/ASTTransform
	${MKDIR_P} ${OBJ_DIR}/Translator
	${MKDIR_P} ${OBJ_DIR}/Tuner
	${MKDIR_P} ${OBJ_DIR}/Collections
	${MKDIR_P} ${OBJ_DIR}/Backend
	${MKDIR_P} ${OBJ_DIR}/Encoders
	${MKDIR_P} ${OBJ_DIR}/Serialize

debug: CFLAGS += -DCONFIG_DEBUG
debug: all

test: all
	make -C Test

PHONY += docs
docs: $(C_SOURCES) $(HEADERS)
	doxygen

${OBJ_DIR}/$(LIB_SO): $(OBJECTS)
	$(CXX) -g $(SHARED) -o ${OBJ_DIR}/$(LIB_SO) $+ $(LDFLAGS)

${OBJ_DIR}/%.o: %.cc
	$(CXX) -fPIC -c $< -o $@ $(CFLAGS) -Wno-unused-variable

${OBJ_DIR}/%.o: %.c
	$(CC) -fPIC -c $< -o $@ $(CFLAGS) -Wno-unused-variable

-include $(OBJECTS:%=$OBJ_DIR/.%.d)

PHONY += clean
clean:
	rm -f *.o *.so
	rm -rf $(OBJ_DIR)

PHONY += mrclean
mrclean: clean
	rm -rf ../docs

PHONY += tags
tags:
	ctags -R

tabbing:
	uncrustify -c C.cfg --no-backup *.cc */*.cc
	uncrustify -c C.cfg --no-backup *.h */*.h

wc:
	wc */*.cc */*.h *.cc *.h

.PHONY: $(PHONY)
