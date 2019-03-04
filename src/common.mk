# A few common Makefile items

CC := gcc
CXX := g++
JAVAC := javac

UNAME := $(shell uname)
JAVA_INC := /usr/lib/jvm/java-1.8.0-openjdk-amd64/include/
LIB_NAME := cons_comp
LIB_SO := lib_$(LIB_NAME).so

CPPFLAGS += -Wall -g -O0

# Mac OSX options
ifeq ($(UNAME), Darwin)
CPPFLAGS += -D_XOPEN_SOURCE -DMAC
endif
