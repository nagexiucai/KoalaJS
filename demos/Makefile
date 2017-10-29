CFLAG = -g  -I../build/include

SRCS = demoClass.cpp 

LDFLAG = -lpthread -L../build -lKoalaJS

TARGET = demoClass

all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LDFLAG}

clean:
	rm -fr ${TARGET} *.o *.dSYM

