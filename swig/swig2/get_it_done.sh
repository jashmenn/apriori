#!/bin/bash
rm *.o
rm *.bundle
rm intexp_wrap.c
rm Makefile
swig -ruby intexp.i
ruby extconf.rb
make
ruby test_swig.rb
