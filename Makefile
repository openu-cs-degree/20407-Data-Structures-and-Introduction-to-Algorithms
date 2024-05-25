CXX=g++-14
CXXFLAGS=-std=c++2b -Werror -Wall -Wextra -fsanitize=address
TARGET=main

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)