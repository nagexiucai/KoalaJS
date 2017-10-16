CFLAG = -g -I./ -I../
COMPILER_SRCS = Compiler.cpp Bytecode.cpp Var.cpp bc.cpp 

JSVM_SRCS = Bytecode.cpp Var.cpp jsvm.cpp TinyJS.cpp \
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
	  ../native/MD5/MD5.cpp 
#	  ../native/VM/VM.cpp \
#	  ../native/String/String.cpp \
#	  ../native/Thread/Thread.cpp \
#	  ../native/Array/Array.cpp \

#LDFLAG = -lpthread

LIBS = ../libs/Base64/Base64.cpp \
		../libs/MD5/md5.cpp \
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
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${COMPILER} ${COMPILER_SRCS} ${LIBS} ${LDFLAG}
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${VM} ${VM_SRCS} ${LIBS} ${LDFLAG}

clean:
	rm -fr *.bcode
	rm -fr ${COMPILER} *.o ${COMPILER}.dSYM
	rm -fr ${VM} *.o ${VM}.dSYM

