CXX=g++
CXX_FLAGS=-std=c++2a -g -O0 -Iincludes -Wall -Wextra -Werror
# SAN = -fsanitize=address,null -fno-omit-frame-pointer

release: bin/simplex-release
simplex: bin/simplex



bin/simplex: src/* includes/*
	${CXX} ${CXX_FLAGS} src/* -o $@

bin/simplex-release: src/* 
	${CXX} -std=c++2a src/* -o $@

.DEFAULT_GOAL := simplex
.PHONY: clean simplex 

clean:
	rm simplex simplex-release
