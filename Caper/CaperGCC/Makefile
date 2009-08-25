SRCDIR=../Caper
CXXFLAGS=-fPIC

OBJS=$(SRCDIR)/Arguments.o \
       $(SRCDIR)/MapviewMappingsPreparer.o $(SRCDIR)/BowtieMappingEngine.o \
       $(SRCDIR)/MappingCache.o $(SRCDIR)/MapviewMappingUtilities.o \
       $(SRCDIR)/BowtieMappingsPreparer.o $(SRCDIR)/Mapping.o $(SRCDIR)/Pair.o \
       $(SRCDIR)/BowtieMappingUtilities.o $(SRCDIR)/MappingEngine.o \
       $(SRCDIR)/Sequence.o $(SRCDIR)/Caper.o $(SRCDIR)/MappingsPreparer.o \
       $(SRCDIR)/SequenceEngine.o $(SRCDIR)/Commands.o $(SRCDIR)/MappingUtilities.o \
       $(SRCDIR)/SequenceReader.o $(SRCDIR)/FASequenceEngine.o \
       $(SRCDIR)/MapviewMappingEngine.o $(SRCDIR)/XGetopt.o

all: Caper short

Caper: $(SRCDIR)/Main.o $(OBJS)
	g++ -o Caper $(SRCDIR)/Main.o $(OBJS)

short:  $(SRCDIR)/short.o $(OBJS)
	g++ -o short $(SRCDIR)/short.o $(OBJS)
