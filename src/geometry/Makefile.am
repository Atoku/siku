STYLEFLAGS = -pedantic -Wall

AM_CXXFLAGS = $(SIMD_FLAGS) $(PROFILE_FLAGS) $(STYLEFLAGS) $(FPCHECK_FLAGS) ${BOOST_CPPFLAGS} $(INCLUDEFLAGS)

noinst_LTLIBRARIES = libgeometry.la
libgeometry_la_SOURCES = \
        object2d.hh

#        vector2d.hh \
#	geometry.hh geometry.cc
#	matrix3d.hh
#	point3d.hh
# point2d.hh
# polygon2d.hh
# vector2d.hh
# vector3d.hh

SUBDIRS = tests
