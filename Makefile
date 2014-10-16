<<<<<<< HEAD


#assumes that setup.sh has been already run






CC = g++
CCFLAGS = -g -I gen -I . -std=gnu++0x -Wall
FLEX = flex
BISON = bison

cthulhu: cthulhu.cpp bin/parser.o bin/Absyn.o bin/Lexer.o bin/Parser.o bin/Printer.o \
		bin/typechecker.o bin/Skeleton.o bin/operators.o bin/interpreter.o
	${CC} ${CCFLAGS} $^ -o cthulhu


bin/typechecker.o: typechecker/typechecker.cpp typechecker/typechecker.h ast/ast.h gen/Absyn.H gen/Skeleton.H
	${CC} ${CCFLAGS} -c typechecker/typechecker.cpp -o bin/typechecker.o

bin/interpreter.o: interpreter/interpreter.cpp interpreter/interpreter.h ast/ast.h
	${CC} ${CCFLAGS} -c $< -o $@

bin/Skeleton.o: gen/Skeleton.C gen/Skeleton.H gen/Absyn.H
	${CC} ${CCFLAGS} -c gen/Skeleton.C -o bin/Skeleton.o

bin/operators.o: typechecker/operators.cpp typechecker/operators.h gen/Absyn.H ast/ast.h
	${CC} ${CCFLAGS} -c $< -o $@

bin/parser.o: parser/parser.cpp parser/parser.h
	${CC} ${CCFLAGS} -c parser/parser.cpp -o bin/parser.o
        
bin/Absyn.o: gen/Absyn.C gen/Absyn.H
	${CC} ${CCFLAGS} -c gen/Absyn.C -o bin/Absyn.o

bin/Lexer.C: gen/cthulhu.l
	${FLEX} -o bin/Lexer.C gen/cthulhu.l

bin/Parser.C: gen/cthulhu.y
	${BISON} gen/cthulhu.y -o bin/Parser.C

bin/Lexer.o: bin/Lexer.C gen/Parser.H
	${CC} ${CCFLAGS} -c bin/Lexer.C  -o bin/Lexer.o

bin/Parser.o: bin/Parser.C gen/Absyn.H
	${CC} ${CCFLAGS} -c bin/Parser.C -o bin/Parser.o

bin/Printer.o: gen/Printer.C gen/Printer.H gen/Absyn.H
	${CC} ${CCFLAGS} -c gen/Printer.C -o bin/Printer.o






clean:
	rm -rf bin/* cthulhu

clean_gen:
	rm -rf gen/*
=======
CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++0x -I. -lboost_log

GEN_OBJS=gen/Absyn.o gen/Lexer.o gen/Parser.o gen/Printer.o gen/Skeleton.o
ANALYZER_OBJS=analyzer/operators.o analyzer/parser.o analyzer/typechecker.o

OBJS =		cthulhu.o $(ANALYZER_OBJS) interpreter/interpreter.o

LIBS =

TARGET =	cthulhu

$(TARGET):	$(OBJS) $(GEN_OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(GEN_OBJS) $(LIBS)

test/memory_test.o: test/memory_test.cpp utils/memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ test/memory_test.cpp -lboost_unit_test_framework	
Test: test/Test.o test/memory_test.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework

all:	$(TARGET) Test

clean:
	rm -f $(OBJS) $(TARGET) Test
>>>>>>> memory
