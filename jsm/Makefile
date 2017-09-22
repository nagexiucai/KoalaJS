CFLAG = -g -I ../ 
SRCS = jsm.cpp \
	  ../TinyJS/TinyJS.cpp \
	  ../native/String/String.cpp \
	  ../native/Math/Math.cpp \
	  ../native/JSON/JSON.cpp \
	  ../native/VM/VM.cpp \
	  ../native/Base64/Base64.cpp \
	  ../native/Array/Array.cpp \
	  ../native/Thread/Thread.cpp \
	  ../native/MD5/MD5.cpp 

LIBS = ../libs/Base64/Base64.cpp \
		../libs/MD5/md5.cpp

TARGET = jsm
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} -lpthread

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS}


clean:
	rm -fr jsm *.o jsm.dSYM

