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
	  ../native/Global/Global.cpp \
	  ../native/Bytes/Bytes.cpp \
	  ../native/File/File.cpp \
	  ../native/MD5/MD5.cpp 

LIBS = ../libs/Base64/Base64.cpp \
		../libs/MD5/md5.cpp \
		../libs/File/File.cpp

LDFLAG = -lpthread

TARGET = jsm
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}


clean:
	rm -fr jsm *.o jsm.dSYM

