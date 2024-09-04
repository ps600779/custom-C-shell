CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

TARGET = myprogram
SRCS = main.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp headers.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
