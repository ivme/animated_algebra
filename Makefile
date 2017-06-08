BUILD_DIR = build
CXX_FLAGS = -std=c++11
CC = g++

OBJECTS = ${BUILD_DIR}/algebra.o ${BUILD_DIR}/graphics.o ${BUILD_DIR}/matrix.o ${BUILD_DIR}/image.o ${BUILD_DIR}/viewer.o ${BUILD_DIR}/renderer.o ${BUILD_DIR}/p_rect.o ${BUILD_DIR}/scene.o ${BUILD_DIR}/aa_controller.o
HEADERS = p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h aa_controller.h

all: ${OBJECTS} ${BUILD_DIR}/test

directories:
	mkdir ${BUILD_DIR}

${BUILD_DIR}/aa_controller.o: aa_controller.cpp aa_controller.h p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/renderer.o: renderer.cpp p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/p_rect.o: p_rect.cpp p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/scene.o: scene.cpp scene.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/viewer.o: viewer.cpp animation.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/image.o: image.cpp image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/matrix.o: matrix.cpp matrix.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/graphics.o: graphics.cpp graphics.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/algebra.o: algebra.cpp algebra.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<


${BUILD_DIR}/test: test.cpp ${HEADERS} ${OBJECTS}
	$(CC) ${CXX_FLAGS} -o $@ $< ${OBJECTS}

clean:
	rm -rf ${BUILD_DIR}