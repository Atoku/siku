STYLEFLAGS = -pedantic -Wall
INCLUDEFLAGS = -I..

AM_CXXFLAGS = $(SIMD_FLAGS) $(PROFILE_FLAGS) \
$(STYLEFLAGS) $(FPCHECK_FLAGS) ${BOOST_CPPFLAGS} $(INCLUDEFLAGS)

noinst_PROGRAMS = testvector2d testpoint2d testsegment2d testpoly2d

testvector2d_SOURCES = testvector2d.cc

testpoint2d_SOURCES = testpoint2d.cc

testsegment2d_SOURCES = testsegment2d.cc
testsegment2d_DEPENDENCIES = ../segment2d.cc ../segment2d.hh
testsegment2d_LDADD = ../libgeometry.a

testpoly2d_SOURCES = testpoly2d.cc
testpoly2d_DEPENDENCIES =  ../segment2d.cc ../segment2d.hh ../polygon2d.hh ../polygon2d.cc
testpoly2d_LDADD = ../libgeometry.a
