CXX = g++
CXXFLAGS = -std=c++17 

SRC = ./src/*.cpp ./test/*.cpp
OBJ = $(SRC:.cc=.o)
EXEC = restcpp

ifeq ($(OS),Windows_NT) 
	LBLIBS = -lwsock32
else
	LBLIBS = -pthread
endif


all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(SRC) -o $@  $(CXXFLAGS) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)