# ── Paths ─────────────────────────────────────────────────────────────────────
ROOT    := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
SRC     := $(ROOT)/src
HPP     := $(ROOT)/hpp
UTEST   := $(ROOT)/utest
BUILD   := $(ROOT)/build
GTDIR   := $(ROOT)/thirdparty/googletest
GTINC   := $(GTDIR)/googletest/include
GTSRC   := $(GTDIR)/googletest/src
GMINC   := $(GTDIR)/googlemock/include
GMSRC   := $(GTDIR)/googlemock/src

# ── Toolchain ─────────────────────────────────────────────────────────────────
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -I$(HPP) -I$(UTEST) \
            -I$(GTINC) -I$(GMINC) \
            -I$(GTDIR)/googletest -I$(GTDIR)/googlemock
LDFLAGS  :=

# ── Source files ──────────────────────────────────────────────────────────────
# Library sources (all except main.cpp)
LIB_SRCS := $(filter-out $(SRC)/main.cpp, $(wildcard $(SRC)/*.cpp))
LIB_OBJS := $(patsubst $(SRC)/%.cpp, $(BUILD)/obj/%.o, $(LIB_SRCS))

# Application
APP_SRCS := $(LIB_SRCS) $(SRC)/main.cpp
APP_OBJS := $(patsubst $(SRC)/%.cpp, $(BUILD)/obj/%.o, $(APP_SRCS))

# Test sources
TEST_SRCS := $(wildcard $(UTEST)/test_*.cpp)
TEST_OBJS := $(patsubst $(UTEST)/%.cpp, $(BUILD)/obj/%.o, $(TEST_SRCS))

# GTest / GMock compiled as a single translation unit each
GTEST_OBJ := $(BUILD)/obj/gtest-all.o
GMOCK_OBJ := $(BUILD)/obj/gmock-all.o
GTMAIN_OBJ:= $(BUILD)/obj/gtest_main.o

# ── Output binaries ───────────────────────────────────────────────────────────
APP      := $(BUILD)/obj/relay_calculator
TEST_BIN := $(BUILD)/obj/run_tests

# ── Default target ────────────────────────────────────────────────────────────
.PHONY: all
all: app test

# ── Application binary ────────────────────────────────────────────────────────
.PHONY: app
app: $(APP)

$(APP): $(APP_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# ── Test binary ───────────────────────────────────────────────────────────────
.PHONY: test
test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(LIB_OBJS) $(GTEST_OBJ) $(GMOCK_OBJ) $(GTMAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# ── GTest / GMock objects ─────────────────────────────────────────────────────
$(GTEST_OBJ): $(GTSRC)/gtest-all.cc | $(BUILD)/obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(GMOCK_OBJ): $(GMSRC)/gmock-all.cc | $(BUILD)/obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(GTMAIN_OBJ): $(GTSRC)/gtest_main.cc | $(BUILD)/obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Library object rule ───────────────────────────────────────────────────────
$(BUILD)/obj/%.o: $(SRC)/%.cpp | $(BUILD)/obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Test object rule ──────────────────────────────────────────────────────────
$(BUILD)/obj/%.o: $(UTEST)/%.cpp | $(BUILD)/obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Directory creation ────────────────────────────────────────────────────────
$(BUILD)/obj:
	mkdir -p $@

# ── Clean ─────────────────────────────────────────────────────────────────────
.PHONY: clean
clean:
	rm -rf $(BUILD)/obj
