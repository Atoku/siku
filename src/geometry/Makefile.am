STYLEFLAGS = -pedantic -Wall

AM_CXXFLAGS = $(SIMD_FLAGS) $(PROFILE_FLAGS) $(STYLEFLAGS) $(FPCHECK_FLAGS) ${BOOST_CPPFLAGS} $(INCLUDEFLAGS)

SUBDIRS = tests

noinst_LIBRARIES = libgeometry.a
libgeometry_a_SOURCES = \
	geometry.hh geometry.cc \
	geom_types.hh \
	matrix2d.hh \
	matrix3d.hh \
	polygon2d.hh polygon2d.cc \
	polyhedron3d.hh polyhedron3d.cc \
	pnt2d.hh \
	pnt3d.hh\
        segment2d.hh segment2d.cc \
	segment3d.hh segment3d.cc \
        vec2d.hh \
	vec3d.hh 


