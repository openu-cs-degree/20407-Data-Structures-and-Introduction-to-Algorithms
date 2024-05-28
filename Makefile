CXX=g++-13
CXXFLAGS=-std=c++2b -Werror -Wall -Wextra -Wpedantic 
TARGET=main

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)