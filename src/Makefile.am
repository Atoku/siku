STYLEFLAGS = -pedantic -Wall
SHAPELIBDIR = ../deps/shapelib

# float point correctness check
if FPCHECK
  FPCHECK_FLAGS = -DSIKU_FP_CHECK
else
  FPCHECK_FLAGS =
endif

# Convenience libraries
SUBDIRS = geometry

# SIMD_FLAGS is from ax_ext.m4 for sse/mmx flags
#
AM_CXXFLAGS = $(SIMD_FLAGS) $(PROFILE_FLAGS) $(STYLEFLAGS) $(PYTHON_CXXFLAGS) $(FPCHECK_FLAGS) ${BOOST_CPPFLAGS}
AM_CFLAGS   = $(SIMD_FLAGS) $(PROFILE_FLAGS) $(STYLEFLAGS) $(PYTHON_CFLAGS)
AM_LDFLAGS  = $(BOOST_PROGRAM_OPTIONS_LIB) $(PYTHON_LDFLAGS) ${BOOST_LDFLAGS} ${BOOST_DATE_TIME_LIB}

bin_PROGRAMS = siku
siku_SOURCES = siku.cc siku.hh \
	auxutils.hh auxutils.cc \
	contact_detect.hh contact_detect.cc \
	contact_force.hh contact_force.cc \
	coordinates.hh coordinates.cc \
	diagnostics.hh diagnostics.cc \
	dynamics.hh dynamics.cc \
	element.hh element.cc \
	errors.hh \
	forces_mass.hh forces_mass.cc \
	globals.hh globals.cc \
	highio.hh highio.cc \
	info.hh \
	lowio.hh lowio.cc \
        material.hh \
	mesh.hh mesh.cc \
	modeltime.hh modeltime.cc \
	mproperties.hh mproperties.cc \
	monitoring.hh monitoring.cc \
	nmc_reader.hh nmc_reader.cc \
	options.cc options.hh \
	planet.hh \
	position.hh position.cc \
	scheduler.hh scheduler.cc \
	sikupy.hh sikupy.cc \
	vecfield.cc vecfield.hh


siku_CXXFLAGS = -I$(SHAPELIBDIR) $(AM_CXXFLAGS)
siku_LDFLAGS = $(AM_LDFLAGS)
siku_LDADD = $(SHAPELIBDIR)/libshp.a geometry/libgeometry.a
