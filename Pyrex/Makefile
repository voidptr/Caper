all:
	cd ../CaperGCC && make all
	python setup.py build_ext -i

tests:
	nosetests

clean:
	cd ../CaperGCC && make clean
	cd ../Pyrex