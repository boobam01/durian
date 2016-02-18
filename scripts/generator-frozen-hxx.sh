#!/bin/sh

out=frozen.hxx

cat frozen.h > $out
cat frozen.c | sed 's/^#include ".*"//g' >> $out

