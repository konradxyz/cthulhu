CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		Cthulhu.o

LIBS =

TARGET =	Cthulhu

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

Test: test/Test.o
	$(CXX) -o $@ $^ -lboost_unit_test_framework
all:	$(TARGET) Test

clean:
	rm -f $(OBJS) $(TARGET)
