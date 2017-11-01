CFLAG = -g  -I../build/include -I../

LDFLAG = -lpthread -L../build -lKoalaJS

all:
	g++ ${CFLAG} -o demoClass demoClass.cpp ${LDFLAG}
	g++ ${CFLAG} -o demoJSFunc demoJSFunc.cpp ${LDFLAG}

clean:
	rm -fr demoClass demoJSFunc *.o *.dSYM

