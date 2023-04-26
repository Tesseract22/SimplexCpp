CXX=g++ -std=c++2a 
INCLUDES=-Iincludes
TEST_UTILS=-Itests
DEBUG=-g -O0
RELEASE=-O3
OBJ_FLASG=-c
INSTALL_PATH=/usr/local/include/SimplexLP/

LUA_LIB=-llua5.4 -lluacpp
LUA_FLAG=${LUA_LIB}
# https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
# SAN = -fsanitize=address,null -fno-omit-frame-pointer
all: release simplex test
release: bin/simplex
simplex: bin/simplex-debug
lib: static/SimplexLP.hpp
install: src/* includes/*
	rm -r -f ${INSTALL_PATH}
	mkdir ${INSTALL_PATH}
	cp -r includes/* ${INSTALL_PATH}
test: bin/test
	$^
bin/test: tests/* includes/*
	${CXX} tests/test.cc ${INCLUDES} ${TEST_UTILS} ${DEBUG} -o $@


bin/simplex-debug: src/* includes/*
	${CXX} src/* ${INCLUDES} ${RELEASE} -o $@ 
bin/simplex: src/* includes/*
	${CXX} src/* ${INCLUDES} ${DEBUG} -o $@ 
static/SimplexLP.hpp: includes/*
	./header.py includes $@

.DEFAULT_GOAL := release
.PHONY: clean simplex lib

clean:
	rm bin/*
