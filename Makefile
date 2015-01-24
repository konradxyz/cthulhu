CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++0x -I.
CCX = g++
GEN_OBJS=gen/Absyn.o gen/Lexer.o gen/Parser.o gen/Printer.o gen/Skeleton.o
ANALYZER_OBJS=analyzer/operators.o analyzer/parser.o analyzer/typechecker.o
THUNK_OBJS=interpreter/thunk/thunk_marker.o interpreter/thunk/thunk_generator.o
SEQ_INTERPRETER_OBJS=interpreter/seq_interpreter/seq_interpreter.o

OBJS =

LIBS =

TARGET =	cthulhu

all: $(TARGET)

# $1 - directory, $2, -name $3 - headers
define obj
$(info $1/$2)
OBJS += $(1)/$(2).o
$(1)/$(2).o: $(1)/$(2).cpp $(1)/$(2).h $3
	$(CXX) -O2 $(CXXFLAGS) -c -o $1/$2.o $1/$2.cpp
endef

# $1 - directory, $2, -name $3 - headers
define test_obj
$(info $1/$2)
TEST_OBJS += $(1)/$(2).o
$(1)/$(2).o: $(1)/$(2).cpp $3
	$(CXX) -O2 $(CXXFLAGS) -c -o $1/$2.o $1/$2.cpp
endef

$(eval $(call obj,operations,operations))
$(eval $(call obj,operations,executor))

$(eval $(call test_obj,test,test, utils/logging.h))

$(TARGET):	$(OBJS) $(GEN_OBJS)
	echo $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(GEN_OBJS) $(LIBS)

cthulhu_test: test/test.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework -lboost_log -lpthread

all:	$(TARGET) test

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) test

