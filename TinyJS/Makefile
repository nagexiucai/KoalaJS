CFLAG = -g -I./ -I../
SRCS = jsm.cpp TinyJS.cpp \
	  ../native/ClassLoader.cpp \
	  ../native/String/String.cpp \
	  ../native/Math/Math.cpp \
	  ../native/JSON/JSON.cpp \
	  ../native/VM/VM.cpp \
	  ../native/Base64/Base64.cpp \
	  ../native/Thread/Thread.cpp \
	  ../native/Global/Global.cpp \
	  ../native/Bytes/Bytes.cpp \
	  ../native/File/File.cpp \
	  ../native/Socket/TCP.cpp \
	  ../native/Socket/DNS.cpp \
	  ../native/System/System.cpp \
	  ../native/Dir/Dir.cpp \
	  ../native/DNS/DNSC.cpp \
	  ../native/Array/Array.cpp \
	  ../native/MD5/MD5.cpp 

LIBS = ../libs/Base64/Base64.cpp \
		../libs/MD5/md5.cpp \
		../libs/DNS/query.cc \
    ../libs/DNS/response.cc \
		../libs/DNS/dnsc.cc \
		 ../libs/DNS/request.cc \
		../libs/File/File.cpp

LDFLAG = -lpthread

TARGET = jsm
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}


clean:
	rm -fr jsm *.o jsm.dSYM

