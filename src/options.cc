/*! \file options.cc
 *
 *  \brief Class Options to read command line: implementation
 */

/*
 * Siku: Discrete element method sea-ice model: options.cc
 *       Options reading, settings initializer
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

#include <iostream>
#include <cstdlib>

using namespace std;

extern "C"
{
#include <getopt.h>
}

#include "config.h"
#include "options.hh"

//--------------------------------------------------------------------
Options::Options (int argc, char *argv[])
{
  int c;

  static struct option long_options[] = {
    {"help",    no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {"verbose", no_argument, 0, 'v'},
    {NULL, 0, NULL, 0}
  };

  int option_index = 0;

  while ((c = getopt_long (argc, argv, "hvV",
			   long_options, &option_index)) != -1)
    {
      switch (c)
	{

	case 'h':
          info();
          usage();
          exit( EXIT_SUCCESS );
	  break;

	case 'V':
	  cout << PACKAGE_STRING << "\n";
          exit( EXIT_SUCCESS );
	  break;

        case 'v':
          verbose = true;
          break;

	case '?':
	  break;

	default:
          cout << "?? getopt returned character code 0??" << c << "\n";
        }
    }

  if (optind < argc) pythonfname = argv[optind];

  info();
}

//--------------------------------------------------------------------
void Options::usage()
{
  cout << "Usage: " << PACKAGE << " [OPTIONS] [PYTHON-FILE]\n"
    "\n"
    "Run " << PACKAGE << " using configuration/script"
    " PYTHON-FILE (default is " << pythonfname << ")\n"
    "\n"
    "Options:\n"
    "     -h, --help           print this help, then exit\n"
    "     -V, --version        print version number, then exit\n"
    "\n"
    "     -v, --verbose        verbose output\n"
    "\n"
    "Report bugs to <" << PACKAGE_BUGREPORT << ">\n";
}

//--------------------------------------------------------------------
void Options::info()
{
  cout <<
    PACKAGE_STRING <<
    " Copyright (C) 2013-2014 UAF,\n"
    "Author: Anton Kulchitsky.\n"
    "\n"
    "License GPLv3+: GNU GPL version 3"
    " or later <http://gnu.org/licenses/gpl.html>"
    "\n\n"
    "This is free software; you are free to change and redistribute it.\n"
    "There is NO warranty;\n"
    "not even for MERCHANTABILITY"
    " or FITNESS FOR A PARTICULAR PURPOSE.\n\n";
}
