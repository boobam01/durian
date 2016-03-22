#!/bin/sh
lib=libntllm-1.4
out=libntlm-1.4.hxx
cat $lib/strverscmp.c | sed 's/^#[[:space:]]\?include ".*"//g' > $out
cat $lib/check-version.h >> $out
cat $lib/byteswap.h >> $out
cat $lib/des.h >> $out
cat $lib/md4.h >> $out
cat $lib/check-version.c | sed 's/^#include ".*"//g' >> $out
cat $lib/des.c | sed 's/^#[[:space:]]\?include ".*"//g' >> $out
cat $lib/md4.c | sed 's/^#[[:space:]]\?include ".*"//g' >> $out
# remove ntlm_check_version
sed -i.bak 's|extern const char \*ntlm_check_version|//extern const char *ntlm_check_version|g' $lib/ntlm.h
cat $lib/ntlm.h | sed 's/^#[[:space:]]\?include ".*"//g' >> $out
cat $lib/smbencrypt.c | sed 's/^#[[:space:]]\?include ".*"//g' >> $out
# change getString to getStringA
sed -i.bak s/GetString/GetStrinG/g $lib/smbutil.c
cat $lib/smbutil.c | sed 's/^#[[:space:]]\?include ".*"//g' >> $out
