CFLAG = -g -I../
SRCS = demoClass.cpp \
		../libs/File/File.cpp \
	  ../TinyJS/TinyJS.cpp 

LDFLAG = -lpthread

TARGET = demoClass

all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

clean:
	rm -fr ${TARGET} *.o ${TARGET}.dSYM

