#!/bin/sh
mkdir -p include
out=include/jsonxx.hxx

cat jsonxx.h > $out
cat jsonxx.cc | sed 's/^#include ".*"//g' >> $out
