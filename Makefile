
ifneq ($(CROSS_COMPILE),)
$(info CROSS_COMPILE=$(CROSS_COMPILE))
endif

CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)g++


CFLAG = -Wall -g -I./ -I./jsvm -I./utils

VM_DIR = jsvm
NATIVE_DIR = native
LIBS_DIR = utils

TINYJS = $(VM_DIR)/*.cpp

NATIVE = $(NATIVE_DIR)/ClassLoader.cpp \
	  $(NATIVE_DIR)/Math/Math.cpp \
	  $(NATIVE_DIR)/Global/Global.cpp \
	  $(NATIVE_DIR)/Bytes/Bytes.cpp \
	  $(NATIVE_DIR)/System/System.cpp \
	  $(NATIVE_DIR)/JSON/JSON.cpp \
	  $(NATIVE_DIR)/String/String.cpp \
	  $(NATIVE_DIR)/VM/VM.cpp \
	  $(NATIVE_DIR)/Array/Array.cpp \
	  $(NATIVE_DIR)/Time/Time.cpp 

LIBS = $(LIBS_DIR)/File/File.cpp

LDFLAG = -lpthread -ldl

VM = koala

TARGET = build

all: sh

sh: lib
	$(CXX) $(CFLAG) -o $(VM) koala.cpp -L./$(TARGET) -lKoalaJS $(LDFLAG)
	rm -fr *.dSYM

lib:
	mkdir -p $(TARGET)/include
	$(CXX) $(CFLAG) -c $(TINYJS) $(NATIVE) $(LIBS)
	rm -f $(TARGET)/libKoalaJS.a
	$(AR) cq $(TARGET)/libKoalaJS.a *.o
	rm -f *.o
	cp $(NATIVE_DIR)/*.h $(VM_DIR)/*.h $(TARGET)/include

clean:
	rm -fr $(TARGET)
	rm -fr $(VM)
	rm -fr $(VM)-arm
	rm -fr *.o *.dSYM *.bcode
