CC = gcc
CXX = g++

CCFLAGS = -g
CXXFLAGS = -g -fPIC
LDFLAGS = -L${NUMA_LIB} -lnuma  # Link against numa shared library

SRC = numa_api.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = numa_api
SHARED_LIB = libnuma_api.so

# Build target
all: $(TARGET) $(SHARED_LIB)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SHARED_LIB): $(OBJ)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(TARGET) $(SHARED_LIB)
