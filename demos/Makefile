CFLAG = -g -I../native -I../TinyJS -I../ -I./
VM_CFLAG = -g  -I../build/include

LIBS = ../libs/File/File.cpp

SRCS = demoClass.cpp \
	  ../TinyJS/TinyJS.cpp 

VM_SRCS = demoClass.cpp 

LDFLAG = -lpthread
VM_LDFLAG = -lpthread -L../build -lTinyJS

TARGET = demoClass
VM_TARGET = demoClass-vm

all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}
	g++ ${VM_CFLAG} -o ${VM_TARGET} ${VM_SRCS} ${VM_LDFLAG}

clean:
	rm -fr ${TARGET} ${VM_TARGET} *.o *.dSYM

