SRCDIR=../Caper
UISRCDIR=../CaperUI
CXXFLAGS=-fPIC -g -I$(SRCDIR)

OBJS=$(SRCDIR)/BowtieMappingEngine.o \
	$(SRCDIR)/BowtieMappingsPreparer.o \
	$(SRCDIR)/BowtieMappingUtilities.o \
	$(SRCDIR)/FASequenceEngine.o \
	$(SRCDIR)/Mapping.o \
	$(SRCDIR)/MappingCache.o \
	$(SRCDIR)/MappingEngine.o \
	$(SRCDIR)/MappingsPreparer.o \
	$(SRCDIR)/MappingUtilities.o \
	$(SRCDIR)/MapviewMappingEngine.o \
	$(SRCDIR)/MapviewMappingsPreparer.o \
	$(SRCDIR)/MapviewMappingUtilities.o \
	$(SRCDIR)/Sequence.o \
	$(SRCDIR)/SequenceEngine.o \
	$(SRCDIR)/SequenceReader.o \
	$(UISRCDIR)/Arguments.o \
	$(UISRCDIR)/Caper.o \
	$(UISRCDIR)/Commands.o \
	$(UISRCDIR)/XGetopt.o 

all: CaperUI #short caper.so

clean:
	rm -f $(SRCDIR)/*.o $(UISRCDIR)/*.o #caper.c caper.so data/cache/*

#tests: all
#	bash data/build-indexes.sh
#	nosetests

CaperUI: $(UISRCDIR)/CaperUI.o $(OBJS)
	g++ -o Caper $(UISRCDIR)/CaperUI.o $(OBJS)

#short:  $(SRCDIR)/short.o $(OBJS)
#	g++ -o short $(SRCDIR)/short.o $(OBJS)

#caper.so: short caper.pyx
#	python setup.py build_ext -i
