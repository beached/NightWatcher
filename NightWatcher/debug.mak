#Generated by VisualGDB (http://visualgdb.com)
#DO NOT EDIT THIS FILE MANUALLY UNLESS YOU ABSOLUTELY NEED TO
#USE VISUALGDB PROJECT PROPERTIES DIALOG INSTEAD

BINARYDIR := Debug

#Additional flags
PREPROCESSOR_MACROS := DEBUG
INCLUDE_DIRS := 
LIBRARY_DIRS := 
LIBRARY_NAMES := 
ADDITIONAL_LINKER_INPUTS := 
MACOS_FRAMEWORKS := 
LINUX_PACKAGES := 

CFLAGS := -ggdb -ffunction-sections -O0 -Wall -pedantic -Wunused -Wno-unknown-pragmas -Wunused-function -lstdc++
CXXFLAGS := -ggdb -ffunction-sections -fno-exceptions -fno-rtti -O0 -Wall -pedantic -Wunused -std=c++0x -Wno-unknown-pragmas -Wunused-function -lstdc++
ASFLAGS := 
LDFLAGS := -Wl,-gc-sections
COMMONFLAGS := 

START_GROUP := -Wl,--start-group
END_GROUP := -Wl,--end-group

#Additional options detected from testing the toolchain
USE_DEL_TO_CLEAN := 1
CP_NOT_AVAILABLE := 1

ADDITIONAL_MAKE_FILES := mcu.mak
GENERATE_BIN_FILE := 1
GENERATE_IHEX_FILE := 1
GENERATE_MAP_FILE := 1
