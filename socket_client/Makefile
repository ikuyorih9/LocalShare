CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = socket_client
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
LIBS = -lws2_32

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
