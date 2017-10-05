CFLAG = -g -I../
SRCS = Compiler.cpp bc.cpp \
		../libs/File/File.cpp 

LDFLAG = -lpthread

TARGET = bc

all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

clean:
	rm -fr ${TARGET} *.o ${TARGET}.dSYM

