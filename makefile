###################################################################
# sgInputC <- sgDevice, sgInputToFunction, (Pack)
# sgDevice <- Global 
# sgInputToFunction  <- Pack, Global
# Pack <- Global, LinkedList
# Global
###################################################################

COMPILER_FLAGS_DEBUG=-g -DDEBUG
LINKER_FLAGS_DEBUG=-g
OBJECTS_DIR_DEBUG = ./obj/debug
OBJECTS_DIR_RELEASE = ./obj/release


PATH_GLOBAL=c_global
PATH_SGINPUTC=sgInputC
PATH_SGSCRIPT=sgScript
CC=gcc
COMPILER_FLAGS=-Wall -fpic -std=c99 -Winline -fgnu89-inline -I$(PATH_GLOBAL)
SGINPUTC_OBJECTS=sgInputC.o sgDevice.o sgInputToFunction.o Pack.o
SGSCRIPT_OBJECTS=sgScriptLib.o sgScriptObj.o Pack.o SymbolTable.o
LINKER_FLAGS=-shared -std=c99 -Winline -fgnu89-inline
OUTPUT_DIR = ./output

# use a command line argument to control debug/release:
ifeq ($(DEBUG),1)
	OBJECTS_DIR = $(OBJECTS_DIR_DEBUG)
	COMPILER_FLAGS += $(COMPILER_FLAGS_DEBUG)
	LINKER_FLAGS += $(LINKER_FLAGS_DEBUG)
	OUTPUT_SUFFIX = DEBUG
else
	OBJECTS_DIR = $(OBJECTS_DIR_RELEASE)
	OUTPUT_SUFFIX =
endif

all: sgInputC sgScriptLib

sgScriptLib: $(PATH_SGSCRIPT)/sgScriptLib.c $(PATH_SGSCRIPT)/sgScriptLib.h sgScriptObj
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGSCRIPT)/$@.c
	$(CC) $(LINKER_FLAGS) $(SGSCRIPT_OBJECTS:%=$(OBJECTS_DIR)/%) -o $(OUTPUT_DIR)/$@$(OUTPUT_SUFFIX).pd_linux

sgInputC: $(PATH_SGINPUTC)/sgInputC.c $(PATH_SGINPUTC)/sgInputC.h sgDevice Pack sgInputToFunction
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGINPUTC)/$@.c
	$(CC) $(LINKER_FLAGS) $(SGINPUTC_OBJECTS:%=$(OBJECTS_DIR)/%) -o $(OUTPUT_DIR)/$@$(OUTPUT_SUFFIX).pd_linux

# sgInput: $(OBJECTS_DIR)/$(OBJECTS)
	#$(CC) $(LINKER_FLAGS) $(OBJECTS:%=$(OBJECTS_DIR)/%) -o $@C.pd_linux

sgScriptObj: $(PATH_SGSCRIPT)/sgScriptObj.c $(PATH_SGSCRIPT)/sgScriptObj.c SymbolTable Global
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGSCRIPT)/$@.c


SymbolTable: $(PATH_SGSCRIPT)/SymbolTable.c $(PATH_SGSCRIPT)/SymbolTable.c  Global
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGSCRIPT)/$@.c

sgDevice: $(PATH_SGINPUTC)/sgDevice.c $(PATH_SGINPUTC)/sgDevice.h Global
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGINPUTC)/$@.c

sgInputToFunction: $(PATH_SGINPUTC)/sgInputToFunction.c $(PATH_SGINPUTC)/sgInputToFunction.h Pack Global
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGINPUTC)/$@.c


Pack: $(PATH_GLOBAL)/Pack.c $(PATH_GLOBAL)/Pack.h Global
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_GLOBAL)/$@.c

Global: $(PATH_GLOBAL)/Global.h $(PATH_GLOBAL)/LinkedList.h
	
# %.o: %.c %.h 
	# $(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$*.o $<

clean:
	rm $(OBJECTS_DIR_DEBUG)/*.o $(OBJECTS_DIR_RELEASE)/*.o $(OUTPUT_DIR)/*.pd_linux
