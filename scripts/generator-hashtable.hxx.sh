#!/bin/sh

git clone https://github.com/preshing/CompareIntegerMaps.git

cd CompareIntegerMaps

out=include/hashtable.hxx

cat hashtable.h > $out
cat util.h >> $out
cat hashtable.cpp | sed 's/^#include ".*"//g' >> $out
