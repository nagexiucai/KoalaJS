CFLAG = -g -I./ -I./jsvm -I./utils

VM_DIR = jsvm
NATIVE_DIR = native
LIBS_DIR = utils

TINYJS = $(VM_DIR)/Bytecode.cpp $(VM_DIR)/TinyJS.cpp $(VM_DIR)/Var.cpp $(VM_DIR)/Compiler.cpp $(VM_DIR)/CodeCache.cpp

NATIVE = $(NATIVE_DIR)/ClassLoader.cpp \
	  $(NATIVE_DIR)/Math/Math.cpp \
	  $(NATIVE_DIR)/Base64/JSBase64.cpp \
	  $(NATIVE_DIR)/Global/Global.cpp \
	  $(NATIVE_DIR)/Bytes/Bytes.cpp \
	  $(NATIVE_DIR)/Socket/TCP.cpp \
	  $(NATIVE_DIR)/Socket/DNS.cpp \
	  $(NATIVE_DIR)/System/System.cpp \
	  $(NATIVE_DIR)/File/JSFile.cpp \
	  $(NATIVE_DIR)/JSON/JSON.cpp \
	  $(NATIVE_DIR)/String/String.cpp \
	  $(NATIVE_DIR)/VM/VM.cpp \
	  $(NATIVE_DIR)/Thread/Thread.cpp \
	  $(NATIVE_DIR)/DNS/JSDNSC.cpp \
	  $(NATIVE_DIR)/Dir/Dir.cpp \
	  $(NATIVE_DIR)/Array/Array.cpp \
	  $(NATIVE_DIR)/MD5/JSMD5.cpp

LIBS = $(LIBS_DIR)/Base64/Base64.cpp \
		$(LIBS_DIR)/MD5/md5.cpp \
		$(LIBS_DIR)/DNS/query.cc \
    $(LIBS_DIR)/DNS/response.cc \
		$(LIBS_DIR)/DNS/dnsc.cc \
		$(LIBS_DIR)/DNS/request.cc \
		$(LIBS_DIR)/File/File.cpp

LDFLAG = -lpthread

LDFLAGARM = --static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc

COMPILER = jsbc
VM = jsh

TARGET = build
ARM = arm-none-linux-gnueabi-

all: lib sh

sh:
	g++ $(CFLAG) -o $(VM) jsh.cpp $(LDFLAG) -L./$(TARGET) -lTinyJS
	rm -fr *.dSYM

lib: 
	mkdir -p $(TARGET)/include
	g++ $(CFLAG) -c $(TINYJS) $(NATIVE) $(LIBS)
	ar cq $(TARGET)/libTinyJS.a *.o
	rm -f *o
	cp $(NATIVE_DIR)/*.h $(VM_DIR)/*.h $(TARGET)/include

arm: 
	mkdir -p $(TARGET)/include
	$(ARM)g++ $(CFLAG) -c $(TINYJS) $(LIBS) $(NATIVE) 
	$(ARM)ar cq $(TARGET)/libTinyJS-arm.a *.o
	rm -f *o
	$(ARM)g++ $(CFLAG)  -o $(VM)-arm jsh.cpp $(LDFLAGARM) -L./$(TARGET) -lTinyJS-arm
	cp $(NATIVE_DIR)/*.h $(VM_DIR)/*.h $(TARGET)/include

clean:
	rm -fr $(TARGET)
	rm -fr $(VM)
	rm -fr $(VM)-arm
	rm -fr *.dSYM