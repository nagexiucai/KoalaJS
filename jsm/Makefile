CFLAG = -g -I ../
SRCS = jsm.cpp \
	  ../TinyJS/TinyJS.cpp \
	  ../native/String/String.cpp \
	  ../native/Math/Math.cpp \
	  ../native/JSON/JSON.cpp \
	  ../native/VM/VM.cpp \
	  ../native/Array/Array.cpp 

TARGET = jsm
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS}


clean:
	rm -fr jsm *.o jsm.dSYM

