BUILD_DIR := bin
OBJ_DIR := obj

ASSEMBLY := testbed
EXTENSION := 
COMPILER_FLAGS := -g -fdeclspec -fPIC
INCLUDE_FLAGS := -Iengine/src -I$(VULKAN_SDK)\include
LINKER_FLAGS := -L./$(BUILD_DIR)/ -lengine -Wl,-rpath,.
DEFINES := -D_DEBUG -DKIMPORT

SRC_FILES := $(shell find $(ASSEMBLY) -name *.c)	#.c files
DIRS := $(shell find $(ASSEMBLY) -type d)			#directories with .h files
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)			#compiled .o objs

all: scaffold compile link

.PHONY: scaffold
scaffold:	#create build dir
	@echo Scaffolding folder structure...
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRS))
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES)	#link
	@echo Linking $(ASSEMBLY)...
	@clang $(OBJ_FILES) -o $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile
compile:	#compile .c files
	@echo Compiling...

.PHONY: clean
clean:	#clean build dir
	rm -rf $(BUILD_DIR)\$(ASSEMBLY)
	rm -rf $(OBJ_DIR)\$(ASSEMBLY)

$(OBJ_DIR)/%.c.o: %.c	#compile .c to .o obj
	@echo $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)
