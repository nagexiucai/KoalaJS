CFLAG = -g -I./ -I../

TINYJS = Bytecode.cpp TinyJS.cpp Var.cpp Compiler.cpp CodeCache.cpp

NATIVE = ../native/ClassLoader.cpp \
	  ../native/Math/Math.cpp \
	  ../native/Base64/JSBase64.cpp \
	  ../native/Global/Global.cpp \
	  ../native/Bytes/Bytes.cpp \
	  ../native/Socket/TCP.cpp \
	  ../native/Socket/DNS.cpp \
	  ../native/System/System.cpp \
	  ../native/File/JSFile.cpp \
	  ../native/JSON/JSON.cpp \
	  ../native/String/String.cpp \
	  ../native/VM/VM.cpp \
	  ../native/Thread/Thread.cpp \
	  ../native/DNS/JSDNSC.cpp \
	  ../native/Dir/Dir.cpp \
	  ../native/Array/Array.cpp \
	  ../native/MD5/JSMD5.cpp

LIBS = ../libs/Base64/Base64.cpp \
		../libs/MD5/md5.cpp \
		../libs/DNS/query.cc \
    ../libs/DNS/response.cc \
		../libs/DNS/dnsc.cc \
		 ../libs/DNS/request.cc \
		../libs/File/File.cpp

LDFLAG = -lpthread

LDFLAGARM = --static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc

COMPILER = jsbc
VM = jsvm

TARGET = build
ARM = arm-none-linux-gnueabi-

all: 
	mkdir -p $(TARGET)/include
	g++ $(CFLAG) -c $(TINYJS) $(NATIVE) $(LIBS)
	ar cq $(TARGET)/libTinyJS.a *.o
	rm -f *o
	g++ $(CFLAG) -o $(TARGET)/$(VM) jsvm.cpp $(LDFLAG) -L./$(TARGET) -lTinyJS
	g++ $(CFLAG) -o $(TARGET)/$(COMPILER) jsbc.cpp $(LDFLAG) -L./$(TARGET) -lTinyJS
	rm -fr $(TARGET)/*.dSYM
	cp ../native/*.h *.h $(TARGET)/include

bc:
	g++ $(CFLAG) -o $(TARGET)/$(COMPILER) jsbc.cpp $(LDFLAG) -L./$(TARGET) -lTinyJS

arm: 
	mkdir -p $(TARGET)/include
	$(ARM)g++ $(CFLAG) -c $(TINYJS) $(LIBS) $(NATIVE) 
	$(ARM)ar cq $(TARGET)/libTinyJS-arm.a *.o
	rm -f *o
	$(ARM)g++ $(CFLAG)  -o $(TARGET)/$(VM)-arm jsvm.cpp $(LDFLAGARM) -L./$(TARGET) -lTinyJS-arm
	cp ../native/*.h *.h $(TARGET)/include

clean:
	rm -fr $(TARGET)
	rm -fr *.bcode 
