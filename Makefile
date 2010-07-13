all: _Caper_ _Pyrex_

test: _Caper_ _Pyrex_
	nosetests -w Pyrex

clean:
	rm -f CaperGCC/caper
	find . \( -name \*.o -o -name \*.so -o -name \*.pyc \) -exec rm -f {} \;

_Caper_:
	cd CaperGCC && make all

_Pyrex_:
	cd Pyrex && make all
