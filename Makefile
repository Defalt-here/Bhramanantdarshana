# Makefile for Bhramanantdarshana — C++17, no third-party libraries.
#
# Common targets:
#   make all              — build library, demos, tests, benchmarks, CLI
#   make library          — build/libqsim.a only
#   make tests            — build/run_all (test runner)
#   make benchmarks       — build/bench_*
#   make demos            — build/demo_*
#   make qsim_cli         — build/qsim_cli (stdin → simulator bridge)
#   make run-tests        — build + run tests; refreshes tests/unit_test_log.txt
#   make run-benchmarks   — build + run all benchmarks (refresh CSVs in
#                           benchmarks/results/)
#   make cross-validate   — build qsim_cli, then run cross_validation/qiskit_compare.py
#   make compare-benchmarks — head-to-head timing vs Qiskit on the same workloads
#   make clean            — remove build/

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
AR       := ar

BUILD     := build
SRC_DIR   := src
INC_DIR   := include
DEMO_DIR  := demos
TEST_DIR  := tests
BENCH_DIR := benchmarks
CLI_DIR   := cli

LIB_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
LIB_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD)/%.o,$(LIB_SRCS))
LIB      := $(BUILD)/libqsim.a

DEMO_SRCS := $(wildcard $(DEMO_DIR)/demo_*.cpp)
DEMO_BINS := $(patsubst $(DEMO_DIR)/%.cpp,$(BUILD)/%,$(DEMO_SRCS))

TEST_SRCS  := $(wildcard $(TEST_DIR)/test_*.cpp) $(TEST_DIR)/run_all.cpp
TEST_BIN   := $(BUILD)/run_all

BENCH_SRCS := $(wildcard $(BENCH_DIR)/bench_*.cpp)
BENCH_BINS := $(patsubst $(BENCH_DIR)/%.cpp,$(BUILD)/%,$(BENCH_SRCS))

CLI_BIN    := $(BUILD)/qsim_cli

.PHONY: all library demos tests benchmarks qsim_cli clean \
        run-tests run-benchmarks cross-validate compare-benchmarks

all: library demos tests benchmarks qsim_cli

# ---- library --------------------------------------------------------------
library: $(LIB)

$(LIB): $(LIB_OBJS) | $(BUILD)
	$(AR) rcs $@ $^

$(BUILD)/%.o: $(SRC_DIR)/%.cpp $(wildcard $(INC_DIR)/*.h) | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ---- demos ----------------------------------------------------------------
demos: $(DEMO_BINS)

$(BUILD)/%: $(DEMO_DIR)/%.cpp $(LIB) | $(BUILD)
	$(CXX) $(CXXFLAGS) $< $(LIB) -o $@

# ---- tests ----------------------------------------------------------------
tests: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS) $(LIB) $(TEST_DIR)/test_harness.h | $(BUILD)
	$(CXX) $(CXXFLAGS) -I$(TEST_DIR) $(TEST_SRCS) $(LIB) -o $@

# ---- benchmarks -----------------------------------------------------------
benchmarks: $(BENCH_BINS)

$(BUILD)/%: $(BENCH_DIR)/%.cpp $(LIB) | $(BUILD)
	$(CXX) $(CXXFLAGS) $< $(LIB) -o $@

# ---- CLI ------------------------------------------------------------------
qsim_cli: $(CLI_BIN)

$(CLI_BIN): $(CLI_DIR)/qsim_cli.cpp $(LIB) | $(BUILD)
	$(CXX) $(CXXFLAGS) $< $(LIB) -o $@

# ---- runner targets -------------------------------------------------------
run-tests: tests
	./$(TEST_BIN)
	@echo "log written to tests/unit_test_log.txt"

run-benchmarks: benchmarks
	mkdir -p $(BENCH_DIR)/results
	bash $(BENCH_DIR)/run_all.sh

cross-validate: qsim_cli
	python3 cross_validation/qiskit_compare.py cross_validation/circuits/

# Side-by-side timing comparison against Qiskit (gate throughput, norm drift,
# CHSH). Requires qiskit (see cross_validation/requirements.txt).
compare-benchmarks: qsim_cli benchmarks
	python3 benchmarks/compare_with_qiskit.py

# ---- housekeeping ---------------------------------------------------------
$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
