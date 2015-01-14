CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++0x -I.

GEN_OBJS=gen/Absyn.o gen/Lexer.o gen/Parser.o gen/Printer.o gen/Skeleton.o
ANALYZER_OBJS=analyzer/operators.o analyzer/parser.o analyzer/typechecker.o
THUNK_OBJS=interpreter/thunk/thunk_marker.o interpreter/thunk/thunk_generator.o
SEQ_INTERPRETER_OBJS=interpreter/seq_interpreter/seq_interpreter.o

OBJS =		cthulhu.o $(ANALYZER_OBJS) interpreter/interpreter.o $(THUNK_OBJS) $(SEQ_INTERPRETER_OBJS)

LIBS =

TARGET =	cthulhu

all: $(TARGET)

# $1 - directory, $2, -name $3 - headers
define obj
$(info $1/$2)
OBJS += $(1)/$(2)
$(1)/$(2).o: $(1)/$(2).cpp $(1)/$(2).h $3
	$(CXX) -O2 $(CXXFLAGS) -c -o $1/$2.o $1/$2.cpp
endef

$(eval $(call obj,operations,operations))


$(TARGET):	$(OBJS) $(GEN_OBJS)
	echo $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(GEN_OBJS) $(LIBS)

test/memory_test.o: test/memory_test.cpp utils/memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ test/memory_test.cpp -lboost_unit_test_framework	
Test: test/Test.o test/memory_test.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework

all:	$(TARGET) Test

clean:
	rm -f $(OBJS) $(TARGET) Test

