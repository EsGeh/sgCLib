###################################################################
# sgInputC <- sgDevice, sgInputToFunction, (Pack)
# sgDevice <- Global 
# sgInputToFunction  <- Pack, Global
# Pack <- Global, LinkedList
# Global
###################################################################
PATH_GLOBAL=c_global
PATH_SGINPUTC=sgInputC
PATH_SGSCRIPT=sgScript
CC=gcc
COMPILER_FLAGS=-Wall -g -fpic -std=c99 -Winline -fgnu89-inline -I$(PATH_GLOBAL)
SGINPUTC_OBJECTS=sgInputC.o sgDevice.o sgInputToFunction.o Pack.o
SGSCRIPT_OBJECTS=sgScriptLib.o sgScriptObj.o Pack.o SymbolTable.o
LINKER_FLAGS=-shared -g -std=c99 -Winline -fgnu89-inline
OBJECTS_DIR=./obj

all: sgInputC sgScriptLib

sgScriptLib: $(PATH_SGSCRIPT)/sgScriptLib.c $(PATH_SGSCRIPT)/sgScriptLib.h sgScriptObj
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGSCRIPT)/$@.c
	$(CC) $(LINKER_FLAGS) $(SGSCRIPT_OBJECTS:%=$(OBJECTS_DIR)/%) -o $@.pd_linux

sgInputC: $(PATH_SGINPUTC)/sgInputC.c $(PATH_SGINPUTC)/sgInputC.h sgDevice Pack sgInputToFunction
	$(CC) -c $(COMPILER_FLAGS) -o $(OBJECTS_DIR)/$@.o $(PATH_SGINPUTC)/$@.c
	$(CC) $(LINKER_FLAGS) $(SGINPUTC_OBJECTS:%=$(OBJECTS_DIR)/%) -o $@.pd_linux

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
	rm $(OBJECTS_DIR)/*.o *.pd_linux