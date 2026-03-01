CXX      ?= g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -O2 -Iinclude
TARGET    = kuo
SRCS      = src/main.cpp src/lexer.cpp src/parser.cpp src/codegen.cpp
BUILD_DIR = build

UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXE = .exe
    SEP = \\
else
    RM = rm -f
    EXE =
    SEP = /
endif

.PHONY: all clean install test

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

install: $(TARGET)$(EXE)
ifeq ($(OS),Windows_NT)
	@echo "Windows install: copy $(TARGET)$(EXE) to a folder in PATH manually."
else
	install -m 755 $(TARGET) /usr/local/bin/$(TARGET)
endif

test: $(TARGET)$(EXE)
ifeq ($(OS),Windows_NT)
	@echo "=== Test: hello.kuo ==="
	$(TARGET)$(EXE) examples\hello.kuo
	@echo ""
	@echo "=== Test: fibonacci.kuo ==="
	$(TARGET)$(EXE) examples\fibonacci.kuo
	@echo ""
	@echo "=== Test: functions.kuo ==="
	$(TARGET)$(EXE) examples\functions.kuo
	@echo ""
	@echo "=== Test: control_flow.kuo ==="
	$(TARGET)$(EXE) examples\control_flow.kuo
	@echo ""
else
	@echo "=== Test: hello.kuo ==="
	./$(TARGET) examples/hello.kuo
	@echo ""
	@echo "=== Test: fibonacci.kuo ==="
	./$(TARGET) examples/fibonacci.kuo
	@echo ""
	@echo "=== Test: functions.kuo ==="
	./$(TARGET) examples/functions.kuo
	@echo ""
	@echo "=== Test: control_flow.kuo ==="
	./$(TARGET) examples/control_flow.kuo
	@echo ""
endif
	@echo "All tests passed!"

clean:
	$(RM) $(TARGET)$(EXE)
ifeq ($(OS),Windows_NT)
	$(RM) C:\Windows\Temp\kuo_*.cpp
else
	$(RM) /tmp/kuo_*
endif