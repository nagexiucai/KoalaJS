CFLAG = -g -I./ -I../
COMPILER_SRCS = Compiler.cpp Bytecode.cpp Var.cpp bc.cpp \
		../libs/File/File.cpp 

JSVM_SRCS = Bytecode.cpp Var.cpp jsvm.cpp TinyJS.cpp \
	  ../native/Math/Math.cpp \
	  ../native/Bytes/Bytes.cpp \
		../libs/File/File.cpp 

#LDFLAG = -lpthread

COMPILER = bc
VM = jsvm

all:
	g++ ${CFLAG} -o ${VM} ${JSVM_SRCS} ${LIBS} ${LDFLAG}

bc:
	g++ ${CFLAG} -o ${COMPILER} ${COMPILER_SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${COMPILER} ${COMPILER_SRCS} ${LIBS} ${LDFLAG}
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${VM} ${VM_SRCS} ${LIBS} ${LDFLAG}

clean:
	rm -fr ${COMPILER} *.o ${COMPILER}.dSYM
	rm -fr ${VM} *.o ${VM}.dSYM

