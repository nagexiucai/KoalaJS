CFLAG = -g -I../
SRCS = demoClass.cpp \
	  ../TinyJS/TinyJS.cpp \

TARGET = demoClass
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS}

clean:
	rm -fr ${TARGET} *.o ${TARGET}.dSYM

