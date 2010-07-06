## note: to generate a full set of dependencies, use 'make depend'.

SRCDIR=../Caper
UISRCDIR=../CaperUI
ZLIBSRCDIR=../zlib123
CXXFLAGS=-g -fPIC -I$(SRCDIR) -I$(ZLIBSRCDIR)
CFLAGS=-g -fPIC

all: Caper

default: all

clean:
	rm -f $(SRCDIR)/*.o $(UISRCDIR)/*.o $(ZLIBSRCDIR)/*.o Caper

#zlib .o's
adler32.o: $(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
compress.o: $(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
crc32.o: $(ZLIBSRCDIR)/crc32.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
deflate.o: $(ZLIBSRCDIR)/deflate.h \
	$(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
example.o: $(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
gzio.o: $(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
inffast.o: $(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h \
	$(ZLIBSRCDIR)/inftrees.h \
	$(ZLIBSRCDIR)/inflate.h \
	$(ZLIBSRCDIR)/inffast.h
inflate.o: $(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h \
	$(ZLIBSRCDIR)/inftrees.h \
	$(ZLIBSRCDIR)/inflate.h \
	$(ZLIBSRCDIR)/inffast.h
infback.o: $(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h \
	$(ZLIBSRCDIR)/inftrees.h \
	$(ZLIBSRCDIR)/inflate.h \
	$(ZLIBSRCDIR)/inffast.h
inftrees.o: $(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h \
	$(ZLIBSRCDIR)/inftrees.h
minigzip.o: $(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
trees.o: $(ZLIBSRCDIR)/deflate.h \
	$(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h \
	$(ZLIBSRCDIR)/trees.h
uncompr.o: $(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h
zutil.o: $(ZLIBSRCDIR)/zutil.h \
	$(ZLIBSRCDIR)/zlib.h \
	$(ZLIBSRCDIR)/zconf.h

ZLIBOBJ=$(ZLIBSRCDIR)/adler32.o \
	$(ZLIBSRCDIR)/compress.o \
	$(ZLIBSRCDIR)/crc32.o \
	$(ZLIBSRCDIR)/deflate.o \
	$(ZLIBSRCDIR)/gzio.o \
	$(ZLIBSRCDIR)/infback.o \
	$(ZLIBSRCDIR)/inffast.o \
	$(ZLIBSRCDIR)/inflate.o \
	$(ZLIBSRCDIR)/inftrees.o \
	$(ZLIBSRCDIR)/trees.o \
	$(ZLIBSRCDIR)/uncompr.o \
	$(ZLIBSRCDIR)/zutil.o

UIOBJ=$(UISRCDIR)/Arguments.o \
	$(UISRCDIR)/InteractiveArguments.o \
	$(UISRCDIR)/IndexGenomeArguments.o \
	$(UISRCDIR)/IndexMappingsArguments.o \
	$(UISRCDIR)/Caper.o \
	$(UISRCDIR)/XGetopt.o

Caper: $(UISRCDIR)/CaperUI.o $(UIOBJ) $(ZLIBOBJ)
	g++ -Wall -O3 -o Caper $(UISRCDIR)/CaperUI.o $(UIOBJ) \
	$(ZLIBOBJ)

depend:
	touch makefile.deps
	makedepend -fmakefile.deps ../Caper/*.{h,cpp} ../CaperUI/*.{h,cpp} 

-include makefile.deps
