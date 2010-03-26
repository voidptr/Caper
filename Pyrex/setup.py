#! /usr/bin/env python

import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

sysconfig.get_config_vars("CFLAGS") # Set gcc's flags
sysconfig._config_vars["CFLAGS"] = "-fno-strict-aliasing -DNDBUG -g -Wall"

caper_path = os.path.join("..", "Caper")
caper_ui_path = os.path.join("..", "CaperUI")
zlib_path = os.path.join("..","zlib123")

caperObjs = ["BowtieMappingUtilities.o",
		"FASequenceEngine.o",
		"Mapping.o",
		"MappingCache.o", 
		"MappingEngine.o",
		"MappingsIndexer.o",
		"MappingsPreparer.o",
		"MappingUtilities.o",
		"MappingUtilitiesFactory.o",
		"MapviewMappingUtilities.o",
        "Path.o",
        "SAMMappingUtilities.o",
        "Sequence.o",
		"SequenceEngine.o", 
		"SequenceIndexer.o",
		"Utilities.o"]
       
#caperUIObjs = ["Arguments.o",
#	"Caper.o",
#	"Commands.o",
#	"IndexGenomeArguments.o",
#	"IndexMappingsArguments.o",
#	"InteractiveArguments.o",
#	"XGetopt.o"]
	
zlibObjs = ["adler32.o",
	"compress.o",
	"crc32.o",
	"deflate.o",
	"gzio.o",
	"infback.o",
	"inffast.o",
	"inflate.o",
	"inftrees.o",
	"trees.o",
	"uncompr.o",
	"zutil.o"]

caperObjs = [ os.path.join(caper_path, o) for o in caperObjs ]
#caperUIObjs = [ os.path.join(caper_ui_path,o) for o in caperUIObjs ]
zlibObjs = [ os.path.join(zlib_path, o) for o in zlibObjs ]
   
extraObjs = caperObjs  + zlibObjs
 
setup(
	name = "caper",
#        py_modules = ['screed'],
	ext_modules=[
		Extension(
			"caper", 
			["caper.pyx"],
			language="c++", # [RCK] per the pyrex/cython c++ wrapper tutorial:  http://wiki.cython.org/WrappingCPlusPlus
			extra_objects=extraObjs, 
			pyrex_cplus=True, # [RCK] does this do something different than language="c++"?
			include_dirs=[caper_path, caper_ui_path])
],
	cmdclass = {'build_ext': build_ext}
)
