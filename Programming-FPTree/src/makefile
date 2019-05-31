CXX := g++
CC := gcc
HEADER_DIR := ../include

FPTREE_HEADER_DIR := $(HEADER_DIR)/fptree
BIN_DIR := bin
BUILD_DIR := build

CXXFLAGS := -g -O2 -std=c++11
CFLAGS := -fPIC -std=c99 -O2 -msse4.2 -mpclmul -march=native -funroll-loops -Wstrict-overflow -Wstrict-aliasing -Wall -Wextra -pedantic -Wshadow
LEVELDB := -lleveldb
LPTHREAD := -lpthread
PMEM := -lpmem

MAIN := main.o
YCSB := ycsb.o
LYCSB := lycsb.o
FPTREE := fptree.o
UTILITY := utility.o
PALLOCATOR := pallocator.o
CLHASH := clhash.o

CREATE := $(CREATE_FILE)
ALL := $(BIN_DIR)/main
TEST1 := $(BIN_DIR)/lycsb
TEST2 := $(BIN_DIR)/ycsb

#============================TODO: all below============================
$(BIN_DIR)/main: $(BUILD_DIR)/$(MAIN) $(BUILD_DIR)/$(FPTREE) $(BUILD_DIR)/$(UTILITY) $(BUILD_DIR)/$(PALLOCATOR) $(BUILD_DIR)/$(CLHASH)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) $^ -o $@ $(PMEM)

$(BIN_DIR)/lycsb: $(BUILD_DIR)/$(LYCSB)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) $< -o $@ $(LEVELDB) $(LPTHREAD)

$(BIN_DIR)/ycsb: $(BUILD_DIR)/$(YCSB) $(BUILD_DIR)/$(FPTREE) $(BUILD_DIR)/$(UTILITY) $(BUILD_DIR)/$(PALLOCATOR) $(BUILD_DIR)/$(CLHASH)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) $^ -o $@ $(LEVELDB) $(LPTHREAD) $(PMEM)

$(BUILD_DIR)/$(LYCSB): lycsb.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c -o $@ $< $(LEVELDB) $(LPTHREAD)

$(BUILD_DIR)/$(YCSB): ycsb.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c -o $@ $< $(LEVELDB) $(LPTHREAD)

$(BUILD_DIR)/$(MAIN): main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c -o $@ $<

$(BUILD_DIR)/$(FPTREE): fptree.cpp
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c -o $@ $< $(PMEM)

$(BUILD_DIR)/$(UTILITY): utility.cpp
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c -o $@ $<

$(BUILD_DIR)/$(PALLOCATOR): p_allocator.cpp
	$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c -o $@ $< $(PMEM)

$(BUILD_DIR)/$(CLHASH): clhash.c
	$(CC) $(CFLAGS) -I $(HEADER_DIR) -c clhash.c -o $@

$(CREATE_FILE):
	echo "123"
	mkdir -p po

all : $(CREATE)

test1: $(TEST1)

test2: $(TEST2)

clean :
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)

cleand : 
	rm -rf ../data/*
