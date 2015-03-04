CXXFLAGS =	 -O2 -Wall -fmessage-length=0 -std=c++0x -I.
CCX = g++
GEN_OBJS=gen/Absyn.o gen/Lexer.o gen/Parser.o gen/Printer.o gen/Skeleton.o
ANALYZER_OBJS=analyzer/operators.o analyzer/parser.o analyzer/typechecker.o
THUNK_OBJS=interpreter/thunk/thunk_marker.o interpreter/thunk/thunk_generator.o
SEQ_INTERPRETER_OBJS=interpreter/seq_interpreter/seq_interpreter.o

OBJS =

LIBS =

TARGET =	cthulhu_test cthulhu cthulhu_par

all: $(TARGET)

# $1 - directory, $2, -name $3 - headers
define obj
$(info $1/$2)
OBJS += $(1)/$(2).o
$(1)/$(2).o: $(1)/$(2).cpp $(1)/$(2).h $3
	$(CXX) $(CXXFLAGS) -c -o $1/$2.o $1/$2.cpp
endef

# $1 - directory, $2, -name $3 - headers
define test_obj
$(info $1/$2)
TEST_OBJS += $(1)/$(2).o
$(1)/$(2).o: $(1)/$(2).cpp $3
	$(CXX) $(CXXFLAGS) -c -o $1/$2.o $1/$2.cpp
endef

$(eval $(call obj,operations,operations))
$(eval $(call obj,operations,executor))
$(eval $(call obj,asm,value,asm/program.h asm/futurecontext.h))

$(eval $(call obj,asm,instruction,asm/value.h asm/context.h asm/program.h asm/futurecontext.h))
$(eval $(call obj,asm,executor,asm/context.h asm/contextbase.h))
$(eval $(call obj,asm,program,asm/context.h))
$(eval $(call obj,asm,context,asm/contextbase.h asm/value.h))

$(eval $(call obj,asm,futurecontext,asm/contextbase.h asm/value.h))
$(eval $(call obj,asm,contextbase, asm/instruction.h))


$(eval $(call test_obj,test,test, utils/logging.h asm/context.h asm/program.h test/test.h asm/instruction.h asm/updaters.h))

cthulhu_test: test/test.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework -lboost_log -lpthread

cthulhu.o: cthulhu.cpp test/test.h
	$(CXX) $(CXXFLAGS) -c -o $@ cthulhu.cpp


cthulhu: cthulhu.o $(OBJS) test/test.h
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_log -lpthread


cthulhu_par.o: cthulhu_par.cpp test/test.h
	$(CXX) $(CXXFLAGS) -c -o $@ cthulhu_par.cpp


cthulhu_par: cthulhu_par.o $(OBJS) test/test.h
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_log -lpthread


clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) cthulhu.o cthulhu_par.o

