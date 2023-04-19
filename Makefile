CXX=g++ -std=c++2a 
INCLUDES=-I /usr/local/include/LuaCpp -I /usr/include/lua5.4/ -I includes
DEBUG=-g -O0
RELEASE=-O3
OBJ_FLASG=-c
ENTRY=entry/main.cc
ENTRY_OBJ=objs/entry.o
INSTALL_PATH=/usr/local/include/SimplexLP/

LUA_LIB=-llua5.4 -lluacpp
LUA_FLAG=${LUA_LIB}
# https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
# SAN = -fsanitize=address,null -fno-omit-frame-pointer

release: bin/simplex
simplex: bin/simplex-debug
lib: static/SimplexLP.hpp
install: src/* includes/*
	mkdir ${INSTALL_PATH} || rm -r ${INSTALL_PATH}
	cp -r includes/* ${INSTALL_PATH}


bin/simplex-debug: src/* includes/*
	${CXX} src/* ${INCLUDES} ${LUA_FLAG} ${RELEASE} -o $@ 
bin/simplex: src/* includes/*
	${CXX} src/* ${INCLUDES} ${LUA_FLAG} ${DEBUG} -o $@ 
static/SimplexLP.hpp: includes/*
	./header.py includes $@

.DEFAULT_GOAL := release
.PHONY: clean simplex lib

clean:
	rm bin/*
