CXX = g++

SRC_PATH = Src
BUILD_PATH = Build
BIN_NAME = LinuxTabletConfig

SOURCES = $(shell find $(SRC_PATH) -name '*.cpp' | sort -k 1nr | cut -f2-)
OBJECTS = $(SOURCES:$(SRC_PATH)/%.cpp=$(BUILD_PATH)/%.o)
DEPS = $(OBJECTS:.o=.d)

COMPILE_FLAGS = -std=c++11
INCLUDES = -I Include `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJECTS))

.PHONY: clean
clean:
	@$(RM) $(BIN_NAME)
	@$(RM) -r $(BUILD_PATH)

.PHONY: all
all: $(BIN_NAME)

$(BIN_NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $@

-include $(DEPS)

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.PHONY: install
install: 
	cp LinuxTabletConfig /bin/LinuxTabletConfig
