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

