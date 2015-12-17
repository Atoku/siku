STYLEFLAGS = -pedantic -Wall

AM_CXXFLAGS = $(SIMD_FLAGS) $(PROFILE_FLAGS) $(STYLEFLAGS) $(FPCHECK_FLAGS) ${BOOST_CPPFLAGS} $(INCLUDEFLAGS)

noinst_LIBRARIES = libgeometry.a
libgeometry_a_SOURCES = \
        vector2d.hh point2d.hh \
        segment2d.hh segment2d.cc

#	geometry.hh geometry.cc
#	matrix3d.hh
#	point3d.hh
# polygon2d.hh
# vector2d.hh
# vector3d.hh

SUBDIRS = tests
