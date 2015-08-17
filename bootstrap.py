#! /usr/bin/env python
#
# Copyright (c) 2006-2014 Anton Kulchitsky  mailto:atoku0@gmail.com
#

import sys, os, tempfile, shutil
from optparse import OptionParser
from string import Template
import datetime

from config import *

CONFIGURE_AC_TPL = "configure.ac.tpl" # Template for configure.ac
DOXYGEN_TPL = "src/Doxygen.tpl"

#-------------------------------------------------------------------------

def info():
    s = '''\
    This is a bootstrapping script for %s program.

    Copyright (C) 2010-2014 UAF, 
                  designed and crafted by Anton Kulchitsky

    It is intended to prepare all sources and documentation.
    To run it successfully; you need installed fresh enough:

        Python, GNU Autotools

    '''  % (NAME)
    sys.stdout.write( s )


def addsuffix( s, sfx ):
    '''
    To each object in string s adds suffix sfx. For example:
    addsuffix( "one two three", "c" ) will return
    "one.c two.c three.c"
    '''
    return ' '.join ( [ '%s.%s' % (obj,sfx) for obj in s.split() ] )


def getoptions():
    '''
    Read all user options
    '''
    parser = OptionParser()


    parser.add_option("-q", "--quiet", dest="quiet", default=False,
                      action="store_true",
                      help="Suppress verbose output (default=yes)" )

    return parser.parse_args()

def inform( s, opts ):
    '''
    Outputs the string to the stdout, if there is verbose output only
    '''
    if not opts.quiet:
        sys.stdout.write( "    %s\n" % s )

def makechangelog( options ):
    # generate Changelog
    cmd = "git log > ChangeLog"
    inform( cmd, options )
    os.system( cmd )

def makeautotools( options ):
    '''
    Prepare autotool files and run GNU Autotools to generate configure
    '''

    # We are ready to run autotools
    os.system("autoreconf -v -i")   # adding m4 for macro search

def makedate( options ):
    '''
    Creates file doc/date.tex where defines LaTeX commands:
    \coupidate  for range of dates of COUPiice update
    \lastcoupidate   for the last date program was updated
    \branchname    for the current branch name
    \coupiversion   for the current version
    '''

    inform( 'create date records, versions etc. for developer docs', 
            options )

    # we generate the date file in docs
    # the date based on 'git log' command
    # we parse the date out of 'last date'
    git_info = os.popen( "git log -1 --date=iso src/*.cc src/*.hh doc/*.tex" )
    date_str = [s[8:8+10] for s in git_info.readlines() \
                if s[0:5] == 'Date:' ][0]

    thedate = tuple( map( int, date_str.split( '-' ) ) )
    maindate = datetime.date( *thedate )
    maindate_str = maindate.strftime("%B %d, %Y")

    # add branch name to the date.tex file
    git_branch = os.popen( "git branch" )
    current_branch_str = [ s for s in git_branch.readlines() \
                           if '*' in s ][0][1:].strip()

    fdate = open( "doc/date.tex", "w" )
    fdate.write( '%% bootstrap -d generated:\n'\
                 '\\newcommand{\\coupidate}{June 04, 2013 -- %s}\n' %\
                 maindate_str )
    fdate.write( '\\newcommand{\\lastcoupidate}{%s}\n' % maindate_str )
    fdate.write( '\\newcommand{\\branchname}{%s}\n' % current_branch_str )
    fdate.write( '\\newcommand{\\coupiversion}{%s}\n' % VERSION )
    fdate.close()

def create_configure_ac( options ):
    '''
    Creates confiugure.ac files from the template and fills the gaps
    with current version etc.
    '''
    cnfac = open( CONFIGURE_AC_TPL ).read()
    cnfac = Template( cnfac ).safe_substitute( NAME =        NAME,
                                               VERSION =     VERSION,
                                               MAINPROGRAM = MAINPROGRAM,
                                               EMAIL =       EMAIL )
    fp = open( 'configure.ac', 'w' )
    inform( 'create configure.ac', options )
    fp.write( cnfac )
    fp.close()


def create_doxyfile( options ):
    '''
    Creates Doxygen file from Doxygen.tpl and config.py
    '''
    cnfac = open( DOXYGEN_TPL ).read()
    cnfac = Template( cnfac ).safe_substitute( NAME =        NAME,
                                               VERSION =     VERSION,
                                               BRIEF =       BRIEF,
                                               LOGOPNG =     LOGOPNG,
                                               MAINPROGRAM = MAINPROGRAM,
                                               EMAIL =       EMAIL )
    fp = open( 'src/Doxygen', 'w' )
    inform( 'create src/Doxygen', options )
    fp.write( cnfac )
    fp.close()


def make_bootstrap_config_py( options ):
    '''
    Creates bootstrap_config.py in python/siku directory
    '''
    fpin  = open( 'config.py' )
    fpout = open( 'python/siku/bootstrap_config.py', 'w' )

    fpout.write( '#\n# bootstrap_config.py\n' )
    fpout.write( '# Generated by bootstrap.py, do not change!\n#\n\n' )

    for line in fpin:
        fpout.write( line )

    fpout.write( "DATE = \"" );
    fpout.write( datetime.date.today().strftime("%B %d, %Y") )
    fpout.write( "\"\n")

    fpout.close()
    fpin.close()

def main():

    info()

    sys.stdout.write( "Bootstrapping... Run with --help "
                      "to see different bootstrapping options\n\n" )

    (options, args) = getoptions()

    sys.stdout.write( "* Preparing Documentation\n" )
    makedate( options )
    create_doxyfile( options )

    sys.stdout.write( "* Copying config info to python/siku\n" )
    make_bootstrap_config_py( options )

    sys.stdout.write( "* Preparing GNU Autotools\n" )
    create_configure_ac( options )
    makechangelog( options )

    sys.stdout.write( "* Running GNU Autotools\n" )
    makeautotools( options )

    return 0

if __name__=="__main__":
    sys.exit( main() )

