CC = gcc
CXX = g++

CCFLAGS = -g
CXXFLAGS = -g
LDFLAGS = -L/home/users/sharmaas/opt/numactl/lib -lnuma  # Link against numa shared library

SRC = numa_api.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = numa_api.exe

# Build target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(TARGET)
