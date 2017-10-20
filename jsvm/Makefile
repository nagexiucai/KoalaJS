CFLAG = -g -I./ -I../
COMPILER_SRCS = Compiler.cpp Bytecode.cpp Var.cpp jsbc.cpp 

JSVM_SRCS = Bytecode.cpp Var.cpp jsvm.cpp TinyJS.cpp Compiler.cpp CodeCache.cpp \
	  ../native/ClassLoader.cpp \
	  ../native/Math/Math.cpp \
	  ../native/Base64/Base64.cpp \
	  ../native/Global/Global.cpp \
	  ../native/Bytes/Bytes.cpp \
	  ../native/Socket/TCP.cpp \
	  ../native/Socket/DNS.cpp \
	  ../native/System/System.cpp \
	  ../native/File/File.cpp \
	  ../native/JSON/JSON.cpp \
	  ../native/String/String.cpp \
	  ../native/VM/VM.cpp \
	  ../native/Thread/Thread.cpp \
	  ../native/DNS/DNSC.cpp \
	  ../native/MD5/MD5.cpp 

LDFLAG = -lpthread

LDFLAGARM = --static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc

LIBS = ../libs/Base64/Base64.cpp \
		../libs/MD5/md5.cpp \
		../libs/DNS/query.cc \
    ../libs/DNS/response.cc \
		../libs/DNS/dnsc.cc \
		 ../libs/DNS/request.cc \
		../libs/File/File.cpp

COMPILER = jsbc
VM = jsvm

all:
	g++ ${CFLAG} -o ${VM} ${JSVM_SRCS} ${LIBS} ${LDFLAG}
	g++ ${CFLAG} -o ${COMPILER} ${COMPILER_SRCS} ${LIBS} ${LDFLAG}

bc:
	g++ ${CFLAG} -o ${COMPILER} ${COMPILER_SRCS} ${LIBS} ${LDFLAG}

vm:
	g++ ${CFLAG} -o ${VM} ${JSVM_SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG}  -o ${COMPILER} ${COMPILER_SRCS} ${LIBS} ${LDFLAGARM}
	arm-none-linux-gnueabi-g++ ${CFLAG}  -o ${VM} ${JSVM_SRCS} ${LIBS} ${LDFLAGARM}

clean:
	rm -fr *.bcode
	rm -fr ${COMPILER} *.o ${COMPILER}.dSYM
	rm -fr ${VM} *.o ${VM}.dSYM

