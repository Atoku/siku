STYLEFLAGS = -pedantic -Wall
INCLUDEFLAGS = -I..

AM_CXXFLAGS = $(SIMD_FLAGS) $(PROFILE_FLAGS) \
$(STYLEFLAGS) $(FPCHECK_FLAGS) ${BOOST_CPPFLAGS} $(INCLUDEFLAGS)

#noinst_PROGRAMS = testvct2d testpnt2d testsegment2d testpoly2d
noinst_PROGRAMS = testsegment2d testpoly2d

#testvct2d_SOURCES = testvct2d.cc

#testpnt2d_SOURCES = testpnt2d.cc

testsegment2d_SOURCES = testsegment2d.cc
testsegment2d_DEPENDENCIES = ../segment2d.cc ../segment2d.hh
testsegment2d_LDADD = ../libgeometry.a

testpoly2d_SOURCES = testpoly2d.cc
testpoly2d_DEPENDENCIES =  ../segment2d.cc ../segment2d.hh ../polygon2d.hh ../polygon2d.cc
testpoly2d_LDADD = ../libgeometry.a
