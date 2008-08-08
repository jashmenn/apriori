#!/bin/bash
rm *.o
rm *.bundle
rm wrap_apriori.c
rm Makefile
swig -ruby apriori.i
ruby extconf.rb
make
# ruby testsum.rb
