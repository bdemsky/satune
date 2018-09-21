# A few common Makefile items

CC := gcc
CXX := g++-5

UNAME := $(shell uname)

LIB_NAME := cons_comp
LIB_SO := lib_$(LIB_NAME).so

CPPFLAGS += -Wall -g -O0

# Mac OSX options
ifeq ($(UNAME), Darwin)
CPPFLAGS += -D_XOPEN_SOURCE -DMAC
endif
