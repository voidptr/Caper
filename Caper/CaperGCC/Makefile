SRCDIR=../Caper
UISRCDIR=../CaperUI
CXXFLAGS=-fPIC -I$(SRCDIR)

OBJS=\
	$(SRCDIR)/MappingsIndexer.o \
	$(SRCDIR)/FASequenceIndexer.o \
	$(SRCDIR)/BowtieMappingUtilities.o \
	$(SRCDIR)/FASequenceEngine.o \
	$(SRCDIR)/Mapping.o \
	$(SRCDIR)/MappingCache.o \
	$(SRCDIR)/MappingEngine.o \
	$(SRCDIR)/MappingsPreparer.o \
	$(SRCDIR)/SAMMappingUtilities.o \
	$(SRCDIR)/MappingUtilities.o \
	$(SRCDIR)/MappingUtilitiesFactory.o \
	$(SRCDIR)/MapviewMappingUtilities.o \
	$(SRCDIR)/Path.o \
	$(SRCDIR)/Sequence.o \
	$(SRCDIR)/SequenceEngine.o \
	$(SRCDIR)/SequenceIndexer.o \
	$(SRCDIR)/Utilities.o

UIOBJ=$(UISRCDIR)/Arguments.o \
	$(UISRCDIR)/InteractiveArguments.o \
	$(UISRCDIR)/IndexGenomeArguments.o \
	$(UISRCDIR)/IndexMappingsArguments.o \
	$(UISRCDIR)/Caper.o \
	$(UISRCDIR)/Commands.o \
	$(UISRCDIR)/XGetopt.o

all: CaperUI

clean:
	rm -f $(SRCDIR)/*.o $(UISRCDIR)/*.o

CaperUI: $(UISRCDIR)/CaperUI.o $(OBJS) $(UIOBJ)
	g++ -O3 -o Caper $(UISRCDIR)/CaperUI.o $(OBJS) $(UIOBJ)
