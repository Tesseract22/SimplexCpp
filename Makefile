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



bin/simplex-debug: src/*
	${CXX} ${DEBUG} -o $@ $^
bin/simplex: src/*
	${CXX} ${RELEASE} -o $@ $^


.DEFAULT_GOAL := release
.PHONY: clean simplex lib

clean:
	rm bin/*
