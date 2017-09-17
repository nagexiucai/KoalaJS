CFLAG = -g -I../
SRCS = demoNativeClass.cpp \
	  ../TinyJS/TinyJS.cpp \

TARGET = demoNativeClass
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS}

clean:
	rm -fr ${TARGET} *.o ${TARGET}.dSYM

