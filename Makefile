
ifneq ($(CROSS_COMPILE),)
$(info CROSS_COMPILE=$(CROSS_COMPILE))
endif

CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)g++

CFLAG_DEBUG = -Wall -fPIC -g -I./ -I./jsvm -I./utils -DKOALA_DEBUG
CFLAG = -Wall -fPIC -g -I./ -I./jsvm -I./utils

VM_DIR = jsvm
NATIVE_DIR = native
LIBS_DIR = utils

KOALAJS = $(VM_DIR)/*.cpp

NATIVE = $(NATIVE_DIR)/ClassLoader.cpp \
	  $(NATIVE_DIR)/Math/Math.cpp \
	  $(NATIVE_DIR)/Global/Global.cpp \
	  $(NATIVE_DIR)/Bytes/Bytes.cpp \
	  $(NATIVE_DIR)/System/System.cpp \
	  $(NATIVE_DIR)/JSON/JSON.cpp \
	  $(NATIVE_DIR)/String/String.cpp \
	  $(NATIVE_DIR)/VM/VM.cpp \
	  $(NATIVE_DIR)/Array/Array.cpp \
	  $(NATIVE_DIR)/Thread/Thread.cpp \
	  $(NATIVE_DIR)/Timer/Timer.cpp \
	  $(NATIVE_DIR)/Time/Time.cpp

LIBS = $(LIBS_DIR)/File/File.cpp

LDFLAG = -lpthread -ldl

VM = koala
VM_DEBUG = kdb

TARGET = build

all: lib
	$(CXX) $(CFLAG) -o $(VM) koala.cpp -L./$(TARGET) -lKoalaJS $(LDFLAG)
	$(CXX) $(CFLAG_DEBUG) -o $(VM_DEBUG) koala.cpp $(KOALAJS) $(NATIVE) $(LIBS)  $(LDFLAG)
	rm -fr *.dSYM

lib:
	mkdir -p $(TARGET)/include
	$(CXX) $(CFLAG) -c $(KOALAJS) $(NATIVE) $(LIBS)
	rm -f $(TARGET)/libKoalaJS.a
	$(AR) cq $(TARGET)/libKoalaJS.a *.o
	rm -f *.o
	cp $(NATIVE_DIR)/*.h $(VM_DIR)/*.h $(TARGET)/include

clean:
	rm -fr $(TARGET)
	rm -fr $(VM) $(VM_DEBUG)
	rm -fr *.o *.dSYM *.bcode

install:
	mkdir -p /usr/local/include/koala
	mkdir -p /usr/local/lib/koala
	mkdir -p /usr/local/bin
	cp koala /usr/local/bin
	cp kdb /usr/local/bin
	cp build/libKoalaJS.a /usr/local/lib
	cp build/include/*.h /usr/local/include/koala
	cp -r classes /usr/local/koala
