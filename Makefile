CXX = g++

SRC_PATH = Src
BUILD_PATH = Build
BIN_PATH = bin
BIN_NAME = LinuxTabletConfig

SOURCES = $(shell find $(SRC_PATH) -name '*.cpp' | sort -k 1nr | cut -f2-)
OBJECTS = $(SOURCES:$(SRC_PATH)/%.cpp=$(BUILD_PATH)/%.o)
DEPS = $(OBJECTS:.o=.d)

COMPILE_FLAGS = -std=c++11
INCLUDES = -I Include `pkg-config --cflags gtk+-3.0 xrandr`
LIBS = `pkg-config --libs gtk+-3.0 xrandr` -lX11 -lXi -lXmu

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: debug
debug: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS) -g
debug: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@$(RM) -r $(BIN_PATH)
	@$(RM) -r $(BUILD_PATH)

.PHONY: all
all: $(BIN_NAME)

$(BIN_NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(BIN_PATH)/$@

-include $(DEPS)

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.PHONY: install
install: 
	cp bin/LinuxTabletConfig /bin/LinuxTabletConfig
