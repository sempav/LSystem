CXX=g++
CXXFLAGS= -g -std=c++14 -O2 -pedantic -Wall -Wextra -Weffc++ -Wcast-align \
		  -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
		  -Winit-self -Wlogical-op -Wmissing-include-dirs \
		  -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
		  -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
		  -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused
CXXFLAGS+= -DGLEW_STATIC -DGLM_FORCE_RADIANS
CXXFLAGS+= -MMD -MP
LDLIBS=-lglfw -lGL -lGLU -lGLEW

TARGET=$(BIN_DIR)/main

BUILD_DIR = .
SRC_DIR = .
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)

# Helper macros
make_path = $(addsuffix $(1), $(basename $(subst $(2), $(3), $(4))))
# Takes path to source file and returns path to corresponding object
src_to_obj = $(call make_path,.o, $(SRC_DIR), $(OBJ_DIR), $(1))

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPS=$(OBJECTS:.o=.d)

all: main

main: $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) $(LDLIBS) -o $(TARGET)

clean:
	@rm -vf $(OBJECTS) $(DEPS) $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(DEPS)

test:
	@echo SRC_DIR = $(SRC_DIR)
	@echo BIN_DIR = $(BIN_DIR)
	@echo OBJ_DIR = $(OBJ_DIR)
	@echo
	@echo SOURCES = $(SOURCES)
	@echo OBJECTS = $(OBJECTS)
	@echo DEPS    = $(DEPS)
	@echo
	@echo TARGET  = $(TARGET)

