CXX=g++ -std=c++2a -Iincludes
DEBUG=-g -O0 -Wall -Wextra -Werror
RELEASE=-O3
OBJ_FLASG=-c
ENTRY=entry/main.cc
ENTRY_OBJ=objs/entry.o
# https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
# SAN = -fsanitize=address,null -fno-omit-frame-pointer

release: bin/simplex
simplex: bin/simplex-debug
lib: static/SimplexLP.hpp


bin/simplex-debug: src/* includes/*
	${CXX} ${DEBUG} -o $@ src/*
bin/simplex: src/* includes/*
	${CXX} ${RELEASE} -o $@ src/*
static/SimplexLP.hpp: includes/*
	./header.py includes $@

.DEFAULT_GOAL := release
.PHONY: clean simplex lib

clean:
	rm bin/*
