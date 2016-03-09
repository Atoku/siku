from distutils.core import setup, Extension

module = Extension('ExtBords',
                   include_dirs = ['/usr/local/include'],
                   libraries = ['pthread'],
                   sources = ['ExtBordsmodule.cpp']
                   )

setup (name = 'ExtBords',
       version = '1.0',
       description = 'Extension for border generation',
       author='kgill-leebr',
       ext_modules = [module]
       )
