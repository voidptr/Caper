SRCDIR=../Caper
UISRCDIR=../CaperUI
BMSRCDIR=../CaperBenchmarks
CXXFLAGS=-fPIC -I$(SRCDIR)

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
	$(SRCDIR)/SequenceReader.o

UIOBJ=$(UISRCDIR)/Arguments.o \
	$(UISRCDIR)/InteractiveArguments.o \
	$(UISRCDIR)/IndexGenomeArguments.o \
	$(UISRCDIR)/IndexMappingsArguments.o \
	$(UISRCDIR)/Caper.o \
	$(UISRCDIR)/Commands.o \
	$(UISRCDIR)/XGetopt.o

BMOBJ=$(BMSRCDIR)/Arguments.o \
	$(BMSRCDIR)/CaperBenchmarks.o \
	$(BMSRCDIR)/Commands.o \
	$(BMSRCDIR)/XGetopt.o

all: CaperUI CaperBenchmarks #short caper.so

clean:
	rm -f $(SRCDIR)/*.o $(UISRCDIR)/*.o $(BMSRCDIR)/*.o

CaperUI: $(UISRCDIR)/CaperUI.o $(OBJS) $(UIOBJ)
	g++ -O3 -o Caper $(UISRCDIR)/CaperUI.o $(OBJS) $(UIOBJ)

CaperBenchmarks: $(BMSRCDIR)/Main.o $(OBJS) $(BMOBJ)
	g++ -O3 -o CaperBenchmarks $(BMSRCDIR)/Main.o $(OBJS) $(BMOBJ)

