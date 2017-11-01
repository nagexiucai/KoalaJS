CFLAG = -g  -I../build/include -I../

LDFLAG = -lpthread -L../build -lKoalaJS

all:
	mkdir -p build
	g++ ${CFLAG} -o build/demoClass demoClass.cpp ${LDFLAG}
	g++ ${CFLAG} -o build/demoJSFunc demoJSFunc.cpp ${LDFLAG}

clean:
	rm -fr build *.o *.dSYM

