TARGET1 := nfp
TARGET2 := fuzz
TARGET3 := isvalid 
TARGET4 := writeGolden
TARGET5 := wkt_to_svg

SRCS1 := nfp.cpp
SRCS2 := fuzz.cpp
SRCS3 := isvalid.cpp
SRCS4 := writeGolden.cpp
SRCS5 := wkt_to_svg.cpp 

#precompiled headers
OBJS1    := ${SRCS1:.cpp=.o} 
DEPS1    := ${SRCS1:.cpp=.dep} 
OBJS2    := ${SRCS2:.cpp=.o}
DEPS2    := ${SRCS2:.cpp=.dep}
OBJS3    := ${SRCS3:.cpp=.o}
DEPS3    := ${SRCS3:.cpp=.dep}
OBJS4    := ${SRCS4:.cpp=.o}
DEPS4    := ${SRCS4:.cpp=.dep}
OBJS5    := ${SRCS5:.cpp=.o}
DEPS5    := ${SRCS5:.cpp=.dep}


OBJS	:= ${OBJS1} ${OBJS2} ${OBJS3} ${OBJS4} ${OBJS5}

CXXFLAGS += -fpic -I.
LDFLAGS += 
.PHONY: all release debug clean distclean 

all: release
release: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}
debug: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}
info: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}
profile: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}
hardcore: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}
asan: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}

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

${TARGET5}: ${OBJS5}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS5}: %.o: %.cpp %.dep ${GCH5}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS5}: %.dep: %.cpp Makefile
	${CXX} ${CXXFLAGS} -MM $< > $@

${GCH5}: %.gch: ${HEADERS5}
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

install:
	mkdir -p ${DESTDIR}/${PREFIX}
	cp ${TARGET} ${DESTDIR}/${PREFIX}

uninstall:
	rm ${DESTDIR}/${PREFIX}/${TARGET}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${CUO} ${GCH} ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4} ${TARGET5}
distclean: clean

