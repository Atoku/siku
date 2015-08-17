/*! \file errors.hh 
 *
 * \brief Basic definitions for errors and macros for errors and
 * warning reports
 */

/*
 * Siku: Discrete element method sea-ice model: script.cc
 *
 * Copyright (C) 2013 UAF. Author: Anton Kulchitsky
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef ERRORS_HH
#define ERRORS_HH

#include <iostream>
#include <cstdlib>
using namespace std;

/* ----------------------------------------------------------------- */

#define fatal( errcode, ... )                                        \
  {                                                                  \
    fprintf( stderr, __FILE__ ":%d\n", __LINE__ );                   \
    fprintf( stderr, "    Error %d at function %s:\n",               \
             errcode, __func__ );                                    \
    fprintf( stderr, "    " __VA_ARGS__ );                           \
    fprintf( stderr, "\n" );                                         \
    exit( errcode );                                                 \
  }

/* ----------------------------------------------------------------- */

#define warning( ... )                                                \
  {                                                                   \
    fprintf( stderr, __FILE__ ":%d\n", __LINE__ );                    \
    fprintf( stderr, "    Warning at function %s:\n", __func__ );     \
    fprintf( stderr, "    " __VA_ARGS__ );                            \
    fprintf( stderr, "\n" );                                          \
  }

#endif
