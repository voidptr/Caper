A simple Python binding for caper, and a bridge to pygr.

Requirements
------------

You'll need Pyrex, pygr, and screed.  We suggest using virtualenv, too.

Building
--------

 % make

Testing
-------

Run

 % make tests

or equivalently build the indexes manually,

 % bash data/build-indexes.sh

and then run

 % nosetests

to run the tests in 'tests.py'.
