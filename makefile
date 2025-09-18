CXX = g++
CXXFLAGS = -Wall -g -Iincludes
TESTFLAGS = -lgtest -lgmock -pthread 

TARGET = skip_list
OBJ = src/main.o

TESTSRC = tests/test_skip_list.cpp
TESTTARGET = test_skip_list

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

all: $(TARGET)

test: $(TESTSRC)
	$(CXX) $(TESTSRC) $(CXXFLAGS) -o $(TESTTARGET) $(TESTFLAGS)

clean:
	rm -f $(TESTTARGET)
	rm -f $(TARGET)
	rm -f $(OBJ)

.PHONY: all test clean
