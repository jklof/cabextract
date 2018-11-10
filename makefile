FLAGS = -Wall -O -std=c++11

GTEST_INC  = -I googletest/googletest/include
GTEST_LIBS = -L googletest/lib/ -l gtest -lgtest_main -lpthread
GTEST_EXE  = cabextract_gtest
GTEST_SRCS = tests.cc

MSPACK_INC  =
MSPACK_LIBS = -lmspack

CAB_SOLIB = libcabextract.so
CAB_HDRS  = cabextract.hh
CAB_SRCS  = cabextract.cc
CAB_LIBS  = -L . -lcabextract

CABREAD_EXE  = cabread
CABREAD_SRCS = cabread.cc

all: $(CABREAD_EXE) $(GTEST_EXE) $(CAB_SOLIB)

$(CABREAD_EXE): $(CAB_HDRS) $(CABREAD_SRCS) $(CAB_SOLIB)
	g++ -o $(CABREAD_EXE) $(FLAGS) $(CABREAD_SRCS) $(CAB_LIBS) $(MSPACK_LIBS)

$(GTEST_EXE): $(GTEST_SRCS) $(CAB_HDRS) $(CAB_SRCS) $(CAB_SOLIB)
	g++ -o $(GTEST_EXE) $(FLAGS) $(GTEST_SRCS) $(GTEST_INC) $(GTEST_LIBS) $(CAB_LIBS) $(MSPACK_LIBS)

$(CAB_SOLIB) : $(CAB_HDRS) $(CAB_SRCS)
	g++ $(FLAGS) -fPIC $(CAB_SRCS) -shared -o $(CAB_SOLIB) $(MSPACK_LIBS)


runtests: $(GTEST_EXE)
	./$(GTEST_EXE)

clean:
	rm -rf $(GTEST_EXE) $(CABREAD_EXE) $(CAB_SOLIB)

realclean: clean
	rm -rf googletest

