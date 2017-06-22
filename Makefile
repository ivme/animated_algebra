BUILD_DIR = build
CXX_FLAGS = -std=c++11
CC = clang++

PRIVACY_OFF = true
ifeq (${PRIVACY_OFF}, true)
	CXX_FLAGS += -DPRIVACY_OFF -g
endif

OBJECTS = ${BUILD_DIR}/algebra.o ${BUILD_DIR}/graphics.o ${BUILD_DIR}/matrix.o ${BUILD_DIR}/image.o ${BUILD_DIR}/viewer.o ${BUILD_DIR}/renderer.o ${BUILD_DIR}/p_rect.o ${BUILD_DIR}/scene.o ${BUILD_DIR}/aa_controller.o ${BUILD_DIR}/animation.o ${BUILD_DIR}/animator.o ${BUILD_DIR}/action.o ${BUILD_DIR}/quad_factor_animator.o ${BUILD_DIR}/stack_action.o
HEADERS = stack_action.h quad_factor_animator.h p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h aa_controller.h animator.h action.h animation.h

all: ${OBJECTS} ${BUILD_DIR}/test

directories:
	mkdir ${BUILD_DIR}

${BUILD_DIR}/stack_action.o: stack_action.cpp stack_action.h action.h scene.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/aa_controller.o: aa_controller.cpp aa_controller.h quad_factor_animator.h animator.h animation.h p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h stack_action.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/quad_factor_animator.o: quad_factor_animator.cpp quad_factor_animator.h animator.h animation.h p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h stack_action.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/renderer.o: renderer.cpp p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h algebra.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/p_rect.o: p_rect.cpp p_rect.h renderer.h scene.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/animator.o: animator.cpp animator.h image.h matrix.h graphics.h viewer.h animation.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/action.o: action.cpp action.h scene.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/scene.o: scene.cpp scene.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/viewer.o: viewer.cpp animation.h image.h matrix.h graphics.h viewer.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${BUILD_DIR}/animation.o: animation.cpp animation.h image.h matrix.h graphics.h viewer.h
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