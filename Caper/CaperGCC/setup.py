#! /usr/bin/env python

import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

sysconfig.get_config_vars("CFLAGS") # Set gcc's flags
sysconfig._config_vars["CFLAGS"] = "-fno-strict-aliasing -DNDBUG -g -Wall"

c_header_path = os.path.join("..", "Caper")

objs = ["Arguments.o",
       "MapviewMappingsPreparer.o", "BowtieMappingEngine.o",
       "MappingCache.o", "MapviewMappingUtilities.o",
       "BowtieMappingsPreparer.o", "Mapping.o", "Pair.o",
       "BowtieMappingUtilities.o", "MappingEngine.o",
       "Sequence.o", "Caper.o", "MappingsPreparer.o",
       "SequenceEngine.o", "Commands.o", "MappingUtilities.o",
       "SequenceReader.o", "FASequenceEngine.o",
       "MapviewMappingEngine.o", "XGetopt.o" ]

objs = [ os.path.join(c_header_path, o) for o in objs ]
print objs
    
setup(
	name = "caper",
#        py_modules = ['screed'],
	ext_modules=[
		Extension("caper", ["caper.pyx"], extra_objects=objs, pyrex_cplus=True, include_dirs=[c_header_path])
],
	cmdclass = {'build_ext': build_ext}
)
