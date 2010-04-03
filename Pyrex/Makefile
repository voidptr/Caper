all: Caper-Build caper.so

all_all: clean all indexes tests

indexes: all
	bash data/build-indexes.sh

tests: all indexes
	nosetests

clean:
	# remove caper.pyx intermediate files.
	rm -fr *.so caper.cpp
	# remove cached indices etc from screed and caper
	rm -fr data/*_screed data/cache
	# reset C++ source build
	cd ../CaperGCC && make clean

caper.so: ../CaperGCC/Caper caper.cpp
	python setup.py build_ext -i

caper.cpp: caper.pyx
	

Caper-Build:
	cd ../CaperGCC && make all
