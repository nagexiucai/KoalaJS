CFLAG = -g -I../native -I../TinyJS -I../
VM_CFLAG = -g  -I../jsvm/build/include

LIBS = ../libs/File/File.cpp

SRCS = demoClass.cpp \
	  ../TinyJS/TinyJS.cpp 

VM_SRCS = demoClass.cpp 

LDFLAG = -lpthread
VM_LDFLAG = -lpthread -L../jsvm/build -lTinyJS
VM_ARM_LDFLAG = -lpthread -L../jsvm/build -lTinyJS-arm

TARGET = demoClass
VM_TARGET = demoClass-vm

all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}
	g++ ${VM_CFLAG} -o ${VM_TARGET} ${VM_SRCS} ${VM_LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}
	arm-none-linux-gnueabi-g++ ${VM_CFLAG} --static -o ${VM_TARGET} ${VM_SRCS} ${VM_ARM_LDFLAG}

clean:
	rm -fr ${TARGET} ${VM_TARGET} *.o *.dSYM

