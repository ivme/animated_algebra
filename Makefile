BUILD_DIR = build
CXX_FLAGS = -std=c++11
CC = g++

all: ${BUILD_DIR}/viewer.o ${BUILD_DIR}/image.o ${BUILD_DIR}/matrix.o ${BUILD_DIR}/algebra.o ${BUILD_DIR}/test

directories:
	mkdir ${BUILD_DIR}

${BUILD_DIR}/%.o: %.cpp %.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<


${BUILD_DIR}/test: test.cpp ${BUILD_DIR}/algebra.o ${BUILD_DIR}/matrix.o ${BUILD_DIR}/image.o ${BUILD_DIR}/viewer.o
	$(CC) ${CXX_FLAGS} -o $@ $^

clean:
	rm -rf ${BUILD_DIR}