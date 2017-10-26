CFLAG = -g -I./ -I../
SRCS = jsm.cpp TinyJS.cpp \
	  ../native/ClassLoader.cpp \
	  ../native/String/String.cpp \
	  ../native/Math/Math.cpp \
	  ../native/JSON/JSON.cpp \
	  ../native/VM/VM.cpp \
	  ../native/Base64/JSBase64.cpp \
	  ../native/Thread/Thread.cpp \
	  ../native/Global/Global.cpp \
	  ../native/Bytes/Bytes.cpp \
	  ../native/File/JSFile.cpp \
	  ../native/Socket/TCP.cpp \
	  ../native/Socket/DNS.cpp \
	  ../native/System/System.cpp \
	  ../native/Dir/Dir.cpp \
	  ../native/DNS/JSDNSC.cpp \
	  ../native/Array/Array.cpp \
	  ../native/MD5/JSMD5.cpp 

LIBS = ../utils/Base64/Base64.cpp \
		../utils/MD5/md5.cpp \
		../utils/DNS/query.cc \
    ../utils/DNS/response.cc \
		../utils/DNS/dnsc.cc \
		 ../utils/DNS/request.cc \
		../utils/File/File.cpp

LDFLAG = -lpthread

TARGET = jsm
all:
	g++ ${CFLAG} -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}

arm:
	arm-none-linux-gnueabi-g++ ${CFLAG} --static -o ${TARGET} ${SRCS} ${LIBS} ${LDFLAG}


clean:
	rm -fr jsm *.o jsm.dSYM

