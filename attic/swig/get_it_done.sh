#!/bin/bash
rm *.o
rm *.bundle
rm apriori_wrap.c
rm Makefile
swig -ruby apriori.i
ruby extconf.rb
make
ruby test_swig.rb
