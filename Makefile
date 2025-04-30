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

NUMACTL_DIR = $(CURDIR)/numactl/install
NUMA_LIB = $(NUMACTL_DIR)/lib

local_numactl:
	@cd numactl && \
		./autogen.sh && \
		./configure --prefix=$(NUMACTL_DIR) && \
		make && \
		make install
	@echo "numactl installed in $(NUMACTL_DIR)"

$(TARGET) $(SHARED_LIB): $(local_numactl)

# Clean target
clean:
	rm -f $(OBJ) $(TARGET) $(SHARED_LIB)
	rm -rf $(NUMACTL_DIR)
	@cd numactl && make clean
