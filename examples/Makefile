TARGET1 := nfp
TARGET2 := fuzz
TARGET3 := isvalid 
TARGET4 := writeGolden

SRCS1 := nfp.cpp
SRCS2 := fuzz.cpp
SRCS3 := isvalid.cpp
SRCS4 := writeGolden.cpp

#precompiled headers
HEADERS1 := 
HEADERS2 :=
OBJS1    := ${SRCS1:.cpp=.o} 
DEPS1    := ${SRCS1:.cpp=.dep} 
OBJS2    := ${SRCS2:.cpp=.o}
DEPS2    := ${SRCS2:.cpp=.dep}
OBJS3    := ${SRCS3:.cpp=.o}
DEPS3    := ${SRCS3:.cpp=.dep}
OBJS4    := ${SRCS4:.cpp=.o}
DEPS4    := ${SRCS4:.cpp=.dep}


OBJS	:= ${OBJS1} ${OBJS2} ${OBJS3} ${OBJS4}

CXXFLAGS += -fpic -I.
LDFLAGS += 
.PHONY: all release debug clean distclean 

all: release
release: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}
debug: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}
info: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}
profile: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}
hardcore: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}
asan: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}

${TARGET1}: ${OBJS1}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS1}: %.o: %.cpp %.dep ${GCH}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS1}: %.dep: %.cpp Makefile 
	${CXX} ${CXXFLAGS} -MM $< > $@ 

${GCH1}: %.gch: ${HEADERS1} 
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

${TARGET2}: ${OBJS2}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS2}: %.o: %.cpp %.dep ${GCH2}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS2}: %.dep: %.cpp Makefile
	${CXX} ${CXXFLAGS} -MM $< > $@

${GCH2}: %.gch: ${HEADERS2}
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

${TARGET3}: ${OBJS3}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS3}: %.o: %.cpp %.dep ${GCH3}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS3}: %.dep: %.cpp Makefile
	${CXX} ${CXXFLAGS} -MM $< > $@

${GCH3}: %.gch: ${HEADERS3}
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

${TARGET4}: ${OBJS4}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS4}: %.o: %.cpp %.dep ${GCH4}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS4}: %.dep: %.cpp Makefile
	${CXX} ${CXXFLAGS} -MM $< > $@

${GCH4}: %.gch: ${HEADERS4}
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

install:
	mkdir -p ${DESTDIR}/${PREFIX}
	cp ${TARGET} ${DESTDIR}/${PREFIX}

uninstall:
	rm ${DESTDIR}/${PREFIX}/${TARGET}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${CUO} ${GCH} ${TARGET1} ${TARGET2} ${TARGET3}

distclean: clean

