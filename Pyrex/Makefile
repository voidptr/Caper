all: Caper-Build caper.so

all_all: clean all indexes tests

indexes: all
	bash data/build-indexes.sh

test: tests

tests: all indexes
	nosetests

clean:
	# remove caper.pyx intermediate files.
	rm -fr *.so caper.cpp
	# remove cached indices etc from screed and caper
	rm -fr data/*_screed data/cache
	# reset C++ source build
	cd ../CaperGCC && make clean

caper.so: ../CaperGCC/caper caper.pyx
	python setup.py build_ext -i

Caper-Build:
	cd ../CaperGCC && make all
