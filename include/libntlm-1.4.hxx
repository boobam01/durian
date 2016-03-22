/* Compare strings while treating digits characters numerically.
   Copyright (C) 1997, 2000, 2002, 2004, 2006, 2009-2013 Free Software
   Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jean-François Bignolles <bignolle@ecoledoc.ibp.fr>, 1997.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along
   with this program; if not, see <http://www.gnu.org/licenses/>.  */

//#if !_LIBC
//# include <config.h>
//#endif

#include <string.h>
#include <ctype.h>

/* states: S_N: normal, S_I: comparing integral part, S_F: comparing
           fractional parts, S_Z: idem but with leading Zeroes only */
#define S_N    0x0
#define S_I    0x4
#define S_F    0x8
#define S_Z    0xC

/* result_type: CMP: return diff; LEN: compare using len_diff/diff */
#define CMP    2
#define LEN    3


/* ISDIGIT differs from isdigit, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char
     or EOF.
   - It's typically faster.
   POSIX says that only '0' through '9' are digits.  Prefer ISDIGIT to
   isdigit unless it's important to use the locale's definition
   of "digit" even when the host does not conform to POSIX.  */
#define ISDIGIT(c) ((unsigned int) (c) - '0' <= 9)

#undef __strverscmp
#undef strverscmp

#ifndef weak_alias
# define __strverscmp strverscmp
#endif

/* Compare S1 and S2 as strings holding indices/version numbers,
   returning less than, equal to or greater than zero if S1 is less than,
   equal to or greater than S2 (for more info, see the texinfo doc).
*/

int
__strverscmp (const char *s1, const char *s2)
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;
  int state;
  int diff;

  /* Symbol(s)    0       [1-9]   others  (padding)
     Transition   (10) 0  (01) d  (00) x  (11) -   */
  static const unsigned int next_state[] =
  {
      /* state    x    d    0    - */
      /* S_N */  S_N, S_I, S_Z, S_N,
      /* S_I */  S_N, S_I, S_I, S_I,
      /* S_F */  S_N, S_F, S_F, S_F,
      /* S_Z */  S_N, S_F, S_Z, S_Z
  };

  static const int result_type[] =
  {
      /* state   x/x  x/d  x/0  x/-  d/x  d/d  d/0  d/-
                 0/x  0/d  0/0  0/-  -/x  -/d  -/0  -/- */

      /* S_N */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_I */  CMP, -1,  -1,  CMP,  1,  LEN, LEN, CMP,
                  1,  LEN, LEN, CMP, CMP, CMP, CMP, CMP,
      /* S_F */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_Z */  CMP,  1,   1,  CMP, -1,  CMP, CMP, CMP,
                 -1,  CMP, CMP, CMP
  };

  if (p1 == p2)
    return 0;

  c1 = *p1++;
  c2 = *p2++;
  /* Hint: '0' is a digit too.  */
  state = S_N | ((c1 == '0') + (ISDIGIT (c1) != 0));

  while ((diff = c1 - c2) == 0 && c1 != '\0')
    {
      state = next_state[state];
      c1 = *p1++;
      c2 = *p2++;
      state |= (c1 == '0') + (ISDIGIT (c1) != 0);
    }

  state = result_type[state << 2 | ((c2 == '0') + (ISDIGIT (c2) != 0))];

  switch (state)
    {
    case CMP:
      return diff;

    case LEN:
      while (ISDIGIT (*p1++))
        if (!ISDIGIT (*p2++))
          return 1;

      return ISDIGIT (*p2) ? -1 : diff;

    default:
      return state;
    }
}
#ifdef weak_alias
weak_alias (__strverscmp, strverscmp)
#endif
/* check-version.h --- Check version string compatibility.
   Copyright (C) 2005, 2009-2013 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson. */

#ifndef CHECK_VERSION_H
# define CHECK_VERSION_H

extern const char *
ntlm_check_version(const char *req_version);

#endif /* CHECK_VERSION_H */
/* byteswap.h - Byte swapping
   Copyright (C) 2005, 2007, 2009-2013 Free Software Foundation, Inc.
   Written by Oskar Liljeblad <oskar@osk.mine.nu>, 2005.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _GL_BYTESWAP_H
#define _GL_BYTESWAP_H

/* Given an unsigned 16-bit argument X, return the value corresponding to
   X with reversed byte order.  */
#define bswap_16(x) ((((x) & 0x00FF) << 8) | \
                     (((x) & 0xFF00) >> 8))

/* Given an unsigned 32-bit argument X, return the value corresponding to
   X with reversed byte order.  */
#define bswap_32(x) ((((x) & 0x000000FF) << 24) | \
                     (((x) & 0x0000FF00) << 8) | \
                     (((x) & 0x00FF0000) >> 8) | \
                     (((x) & 0xFF000000) >> 24))

/* Given an unsigned 64-bit argument X, return the value corresponding to
   X with reversed byte order.  */
#define bswap_64(x) ((((x) & 0x00000000000000FFULL) << 56) | \
                     (((x) & 0x000000000000FF00ULL) << 40) | \
                     (((x) & 0x0000000000FF0000ULL) << 24) | \
                     (((x) & 0x00000000FF000000ULL) << 8) | \
                     (((x) & 0x000000FF00000000ULL) >> 8) | \
                     (((x) & 0x0000FF0000000000ULL) >> 24) | \
                     (((x) & 0x00FF000000000000ULL) >> 40) | \
                     (((x) & 0xFF00000000000000ULL) >> 56))

#endif /* _GL_BYTESWAP_H */
/* des.h --- DES cipher implementation.
 * Copyright (C) 2005, 2007, 2009-2013 Free Software Foundation, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this file; if not, see <http://www.gnu.org/licenses/>.
 *
 */

/* Adapted for gnulib by Simon Josefsson, based on Libgcrypt. */

#ifndef DES_H
# define DES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Encryption/Decryption context of DES
 */
typedef struct
{
  uint32_t encrypt_subkeys[32];
  uint32_t decrypt_subkeys[32];
} gl_des_ctx;

/*
 * Encryption/Decryption context of Triple-DES
 */
typedef struct
{
  uint32_t encrypt_subkeys[96];
  uint32_t decrypt_subkeys[96];
} gl_3des_ctx;

/* Check whether the 8 byte key is weak.  Does not check the parity
 * bits of the key but simple ignore them. */
extern bool
gl_des_is_weak_key (const char * key);

/*
 * DES
 * ---
 */

/* Fill a DES context CTX with subkeys calculated from 64bit KEY.
 * Does not check parity bits, but simply ignore them.  Does not check
 * for weak keys. */
extern void
gl_des_setkey (gl_des_ctx *ctx, const char * key);

/* Fill a DES context CTX with subkeys calculated from 64bit KEY, with
 * weak key checking.  Does not check parity bits, but simply ignore
 * them. */
extern bool
gl_des_makekey (gl_des_ctx *ctx, const char * key, size_t keylen);

/* Electronic Codebook Mode DES encryption/decryption of data
 * according to 'mode'. */
extern void
gl_des_ecb_crypt (gl_des_ctx *ctx, const char * from,  char * to, int mode);

#define gl_des_ecb_encrypt(ctx, from, to)  gl_des_ecb_crypt(ctx, from, to, 0)
#define gl_des_ecb_decrypt(ctx, from, to)  gl_des_ecb_crypt(ctx, from, to, 1)

/* Triple-DES
 * ----------
 */

/* Fill a Triple-DES context CTX with subkeys calculated from two
 * 64bit keys in KEY1 and KEY2.  Does not check the parity bits of the
 * keys, but simply ignore them.  Does not check for weak keys. */
extern void
gl_3des_set2keys (gl_3des_ctx *ctx,
                  const char * key1,
                  const char * key2);

/*
 * Fill a Triple-DES context CTX with subkeys calculated from three
 * 64bit keys in KEY1, KEY2 and KEY3.  Does not check the parity bits
 * of the keys, but simply ignore them.  Does not check for weak
 * keys. */
extern void
gl_3des_set3keys (gl_3des_ctx *ctx,
                  const char * key1,
                  const char * key2,
                  const char * key3);

/* Fill a Triple-DES context CTX with subkeys calculated from three
 * concatenated 64bit keys in KEY, with weak key checking.  Does not
 * check the parity bits of the keys, but simply ignore them. */
extern bool
gl_3des_makekey (gl_3des_ctx *ctx,
                 const char * key,
                 size_t keylen);

/* Electronic Codebook Mode Triple-DES encryption/decryption of data
 * according to 'mode'.  Sometimes this mode is named 'EDE' mode
 * (Encryption-Decryption-Encryption). */
extern void
gl_3des_ecb_crypt (gl_3des_ctx *ctx,
                   const char * from,
                   char * to,
                   int mode);

#define gl_3des_ecb_encrypt(ctx, from, to) gl_3des_ecb_crypt(ctx,from,to,0)
#define gl_3des_ecb_decrypt(ctx, from, to) gl_3des_ecb_crypt(ctx,from,to,1)

#endif /* DES_H */
/* Declarations of functions and data types used for MD4 sum
   library functions.
   Copyright (C) 2000-2001, 2003, 2005, 2008-2013 Free Software Foundation,
   Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef MD4_H
# define MD4_H 1

# include <stdio.h>
# include <stdint.h>

# ifdef __cplusplus
extern "C" {
# endif

# define MD4_DIGEST_SIZE 16

/* Structure to save state of computation between the single steps.  */
struct md4_ctx
{
  uint32_t A;
  uint32_t B;
  uint32_t C;
  uint32_t D;

  uint32_t total[2];
  uint32_t buflen;
  uint32_t buffer[32];
};


/* Initialize structure containing state of computation. */
extern void md4_init_ctx (struct md4_ctx *ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is necessary that LEN is a multiple of 64!!! */
extern void md4_process_block (const void *buffer, size_t len,
                               struct md4_ctx *ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is NOT required that LEN is a multiple of 64.  */
extern void md4_process_bytes (const void *buffer, size_t len,
                               struct md4_ctx *ctx);

/* Process the remaining bytes in the buffer and put result from CTX
   in first 16 bytes following RESBUF.  The result is always in little
   endian byte order, so that a byte-wise output yields to the wanted
   ASCII representation of the message digest.  */
extern void *md4_finish_ctx (struct md4_ctx *ctx, void *resbuf);


/* Put result from CTX in first 16 bytes following RESBUF.  The result is
   always in little endian byte order, so that a byte-wise output yields
   to the wanted ASCII representation of the message digest.  */
extern void *md4_read_ctx (const struct md4_ctx *ctx, void *resbuf);


/* Compute MD4 message digest for bytes read from STREAM.  The
   resulting message digest number will be written into the 16 bytes
   beginning at RESBLOCK.  */
extern int md4_stream (FILE * stream, void *resblock);

/* Compute MD4 message digest for LEN bytes beginning at BUFFER.  The
   result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest.  */
extern void *md4_buffer (const char *buffer, size_t len, void *resblock);

# ifdef __cplusplus
}
# endif

#endif
/* check-version.h --- Check version string compatibility.
   Copyright (C) 1998-2006, 2008-2013 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson.  This interface is influenced by
   gcry_check_version from Werner Koch's Libgcrypt.  Paul Eggert
   suggested the use of strverscmp to simplify implementation. */

//#include <config.h>

#include <stddef.h>
#include <string.h>

/* Get specification. */


/* Version number of package */
#define VERSION "1.4"

/* Check that the version of the library (i.e., the CPP symbol VERSION)
 * is at minimum the requested one in REQ_VERSION (typically found in
 * a header file) and return the version string.  Return NULL if the
 * condition is not satisfied.  If a NULL is passed to this function,
 * no check is done, but the version string is simply returned.
 */
const char *
    ntlm_check_version(const char *req_version)
  {
    if (!req_version || strverscmp(req_version, VERSION) <= 0)
      return VERSION;

    return NULL;
  }
/* des.c --- DES and Triple-DES encryption/decryption Algorithm
 * Copyright (C) 1998-1999, 2001-2007, 2009-2013 Free Software Foundation, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this file; if not, see <http://www.gnu.org/licenses/>.
 *
 */

/* Adapted for gnulib by Simon Josefsson, based on Libgcrypt. */

/*
 * For a description of triple encryption, see:
 *   Bruce Schneier: Applied Cryptography. Second Edition.
 *   John Wiley & Sons, 1996. ISBN 0-471-12845-7. Pages 358 ff.
 * This implementation is according to the definition of DES in FIPS
 * PUB 46-2 from December 1993.
 *
 * Written by Michael Roth <mroth@nessie.de>, September 1998
 */

/*
 *  U S A G E
 * ===========
 *
 * For DES or Triple-DES encryption/decryption you must initialize a proper
 * encryption context with a key.
 *
 * A DES key is 64bit wide but only 56bits of the key are used. The remaining
 * bits are parity bits and they will _not_ checked in this implementation, but
 * simply ignored.
 *
 * For Triple-DES you could use either two 64bit keys or three 64bit keys.
 * The parity bits will _not_ checked, too.
 *
 * After initializing a context with a key you could use this context to
 * encrypt or decrypt data in 64bit blocks in Electronic Codebook Mode.
 *
 * DES Example
 * -----------
 *     unsigned char key[8];
 *     unsigned char plaintext[8];
 *     unsigned char ciphertext[8];
 *     unsigned char recoverd[8];
 *     gl_des_ctx context;
 *
 *     // Fill 'key' and 'plaintext' with some data
 *     ....
 *
 *     // Set up the DES encryption context
 *     gl_des_setkey(&context, key);
 *
 *     // Encrypt the plaintext
 *     des_ecb_encrypt(&context, plaintext, ciphertext);
 *
 *     // To recover the original plaintext from ciphertext use:
 *     des_ecb_decrypt(&context, ciphertext, recoverd);
 *
 *
 * Triple-DES Example
 * ------------------
 *     unsigned char key1[8];
 *     unsigned char key2[8];
 *     unsigned char key3[8];
 *     unsigned char plaintext[8];
 *     unsigned char ciphertext[8];
 *     unsigned char recoverd[8];
 *     gl_3des_ctx context;
 *
 *     // If you would like to use two 64bit keys, fill 'key1' and'key2'
 *     // then setup the encryption context:
 *     gl_3des_set2keys(&context, key1, key2);
 *
 *     // To use three 64bit keys with Triple-DES use:
 *     gl_3des_set3keys(&context, key1, key2, key3);
 *
 *     // Encrypting plaintext with Triple-DES
 *     gl_3des_ecb_encrypt(&context, plaintext, ciphertext);
 *
 *     // Decrypting ciphertext to recover the plaintext with Triple-DES
 *     gl_3des_ecb_decrypt(&context, ciphertext, recoverd);
 */


//#include <config.h>



#include <stdio.h>
#include <string.h>             /* memcpy, memcmp */

/*
 * The s-box values are permuted according to the 'primitive function P'
 * and are rotated one bit to the left.
 */
static const uint32_t sbox1[64] = {
  0x01010400, 0x00000000, 0x00010000, 0x01010404, 0x01010004, 0x00010404,
  0x00000004, 0x00010000, 0x00000400, 0x01010400, 0x01010404, 0x00000400,
  0x01000404, 0x01010004, 0x01000000, 0x00000004, 0x00000404, 0x01000400,
  0x01000400, 0x00010400, 0x00010400, 0x01010000, 0x01010000, 0x01000404,
  0x00010004, 0x01000004, 0x01000004, 0x00010004, 0x00000000, 0x00000404,
  0x00010404, 0x01000000, 0x00010000, 0x01010404, 0x00000004, 0x01010000,
  0x01010400, 0x01000000, 0x01000000, 0x00000400, 0x01010004, 0x00010000,
  0x00010400, 0x01000004, 0x00000400, 0x00000004, 0x01000404, 0x00010404,
  0x01010404, 0x00010004, 0x01010000, 0x01000404, 0x01000004, 0x00000404,
  0x00010404, 0x01010400, 0x00000404, 0x01000400, 0x01000400, 0x00000000,
  0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const uint32_t sbox2[64] = {
  0x80108020, 0x80008000, 0x00008000, 0x00108020, 0x00100000, 0x00000020,
  0x80100020, 0x80008020, 0x80000020, 0x80108020, 0x80108000, 0x80000000,
  0x80008000, 0x00100000, 0x00000020, 0x80100020, 0x00108000, 0x00100020,
  0x80008020, 0x00000000, 0x80000000, 0x00008000, 0x00108020, 0x80100000,
  0x00100020, 0x80000020, 0x00000000, 0x00108000, 0x00008020, 0x80108000,
  0x80100000, 0x00008020, 0x00000000, 0x00108020, 0x80100020, 0x00100000,
  0x80008020, 0x80100000, 0x80108000, 0x00008000, 0x80100000, 0x80008000,
  0x00000020, 0x80108020, 0x00108020, 0x00000020, 0x00008000, 0x80000000,
  0x00008020, 0x80108000, 0x00100000, 0x80000020, 0x00100020, 0x80008020,
  0x80000020, 0x00100020, 0x00108000, 0x00000000, 0x80008000, 0x00008020,
  0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const uint32_t sbox3[64] = {
  0x00000208, 0x08020200, 0x00000000, 0x08020008, 0x08000200, 0x00000000,
  0x00020208, 0x08000200, 0x00020008, 0x08000008, 0x08000008, 0x00020000,
  0x08020208, 0x00020008, 0x08020000, 0x00000208, 0x08000000, 0x00000008,
  0x08020200, 0x00000200, 0x00020200, 0x08020000, 0x08020008, 0x00020208,
  0x08000208, 0x00020200, 0x00020000, 0x08000208, 0x00000008, 0x08020208,
  0x00000200, 0x08000000, 0x08020200, 0x08000000, 0x00020008, 0x00000208,
  0x00020000, 0x08020200, 0x08000200, 0x00000000, 0x00000200, 0x00020008,
  0x08020208, 0x08000200, 0x08000008, 0x00000200, 0x00000000, 0x08020008,
  0x08000208, 0x00020000, 0x08000000, 0x08020208, 0x00000008, 0x00020208,
  0x00020200, 0x08000008, 0x08020000, 0x08000208, 0x00000208, 0x08020000,
  0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const uint32_t sbox4[64] = {
  0x00802001, 0x00002081, 0x00002081, 0x00000080, 0x00802080, 0x00800081,
  0x00800001, 0x00002001, 0x00000000, 0x00802000, 0x00802000, 0x00802081,
  0x00000081, 0x00000000, 0x00800080, 0x00800001, 0x00000001, 0x00002000,
  0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002001, 0x00002080,
  0x00800081, 0x00000001, 0x00002080, 0x00800080, 0x00002000, 0x00802080,
  0x00802081, 0x00000081, 0x00800080, 0x00800001, 0x00802000, 0x00802081,
  0x00000081, 0x00000000, 0x00000000, 0x00802000, 0x00002080, 0x00800080,
  0x00800081, 0x00000001, 0x00802001, 0x00002081, 0x00002081, 0x00000080,
  0x00802081, 0x00000081, 0x00000001, 0x00002000, 0x00800001, 0x00002001,
  0x00802080, 0x00800081, 0x00002001, 0x00002080, 0x00800000, 0x00802001,
  0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const uint32_t sbox5[64] = {
  0x00000100, 0x02080100, 0x02080000, 0x42000100, 0x00080000, 0x00000100,
  0x40000000, 0x02080000, 0x40080100, 0x00080000, 0x02000100, 0x40080100,
  0x42000100, 0x42080000, 0x00080100, 0x40000000, 0x02000000, 0x40080000,
  0x40080000, 0x00000000, 0x40000100, 0x42080100, 0x42080100, 0x02000100,
  0x42080000, 0x40000100, 0x00000000, 0x42000000, 0x02080100, 0x02000000,
  0x42000000, 0x00080100, 0x00080000, 0x42000100, 0x00000100, 0x02000000,
  0x40000000, 0x02080000, 0x42000100, 0x40080100, 0x02000100, 0x40000000,
  0x42080000, 0x02080100, 0x40080100, 0x00000100, 0x02000000, 0x42080000,
  0x42080100, 0x00080100, 0x42000000, 0x42080100, 0x02080000, 0x00000000,
  0x40080000, 0x42000000, 0x00080100, 0x02000100, 0x40000100, 0x00080000,
  0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const uint32_t sbox6[64] = {
  0x20000010, 0x20400000, 0x00004000, 0x20404010, 0x20400000, 0x00000010,
  0x20404010, 0x00400000, 0x20004000, 0x00404010, 0x00400000, 0x20000010,
  0x00400010, 0x20004000, 0x20000000, 0x00004010, 0x00000000, 0x00400010,
  0x20004010, 0x00004000, 0x00404000, 0x20004010, 0x00000010, 0x20400010,
  0x20400010, 0x00000000, 0x00404010, 0x20404000, 0x00004010, 0x00404000,
  0x20404000, 0x20000000, 0x20004000, 0x00000010, 0x20400010, 0x00404000,
  0x20404010, 0x00400000, 0x00004010, 0x20000010, 0x00400000, 0x20004000,
  0x20000000, 0x00004010, 0x20000010, 0x20404010, 0x00404000, 0x20400000,
  0x00404010, 0x20404000, 0x00000000, 0x20400010, 0x00000010, 0x00004000,
  0x20400000, 0x00404010, 0x00004000, 0x00400010, 0x20004010, 0x00000000,
  0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const uint32_t sbox7[64] = {
  0x00200000, 0x04200002, 0x04000802, 0x00000000, 0x00000800, 0x04000802,
  0x00200802, 0x04200800, 0x04200802, 0x00200000, 0x00000000, 0x04000002,
  0x00000002, 0x04000000, 0x04200002, 0x00000802, 0x04000800, 0x00200802,
  0x00200002, 0x04000800, 0x04000002, 0x04200000, 0x04200800, 0x00200002,
  0x04200000, 0x00000800, 0x00000802, 0x04200802, 0x00200800, 0x00000002,
  0x04000000, 0x00200800, 0x04000000, 0x00200800, 0x00200000, 0x04000802,
  0x04000802, 0x04200002, 0x04200002, 0x00000002, 0x00200002, 0x04000000,
  0x04000800, 0x00200000, 0x04200800, 0x00000802, 0x00200802, 0x04200800,
  0x00000802, 0x04000002, 0x04200802, 0x04200000, 0x00200800, 0x00000000,
  0x00000002, 0x04200802, 0x00000000, 0x00200802, 0x04200000, 0x00000800,
  0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const uint32_t sbox8[64] = {
  0x10001040, 0x00001000, 0x00040000, 0x10041040, 0x10000000, 0x10001040,
  0x00000040, 0x10000000, 0x00040040, 0x10040000, 0x10041040, 0x00041000,
  0x10041000, 0x00041040, 0x00001000, 0x00000040, 0x10040000, 0x10000040,
  0x10001000, 0x00001040, 0x00041000, 0x00040040, 0x10040040, 0x10041000,
  0x00001040, 0x00000000, 0x00000000, 0x10040040, 0x10000040, 0x10001000,
  0x00041040, 0x00040000, 0x00041040, 0x00040000, 0x10041000, 0x00001000,
  0x00000040, 0x10040040, 0x00001000, 0x00041040, 0x10001000, 0x00000040,
  0x10000040, 0x10040000, 0x10040040, 0x10000000, 0x00040000, 0x10001040,
  0x00000000, 0x10041040, 0x00040040, 0x10000040, 0x10040000, 0x10001000,
  0x10001040, 0x00000000, 0x10041040, 0x00041000, 0x00041000, 0x00001040,
  0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * These two tables are part of the 'permuted choice 1' function.
 * In this implementation several speed improvements are done.
 */
static const uint32_t leftkey_swap[16] = {
  0x00000000, 0x00000001, 0x00000100, 0x00000101,
  0x00010000, 0x00010001, 0x00010100, 0x00010101,
  0x01000000, 0x01000001, 0x01000100, 0x01000101,
  0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const uint32_t rightkey_swap[16] = {
  0x00000000, 0x01000000, 0x00010000, 0x01010000,
  0x00000100, 0x01000100, 0x00010100, 0x01010100,
  0x00000001, 0x01000001, 0x00010001, 0x01010001,
  0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Numbers of left shifts per round for encryption subkeys.  To
 * calculate the decryption subkeys we just reverse the ordering of
 * the calculated encryption subkeys, so there is no need for a
 * decryption rotate tab.
 */
static const unsigned char encrypt_rotate_tab[16] = {
  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/*
 * Table with weak DES keys sorted in ascending order.  In DES there
 * are 64 known keys which are weak. They are weak because they
 * produce only one, two or four different subkeys in the subkey
 * scheduling process.  The keys in this table have all their parity
 * bits cleared.
 */
static const unsigned char weak_keys[64][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},     /*w */
  {0x00, 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e},
  {0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0},
  {0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe},
  {0x00, 0x1e, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x0e},     /*sw */
  {0x00, 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e, 0x00},
  {0x00, 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0, 0xfe},
  {0x00, 0x1e, 0xfe, 0xe0, 0x00, 0x0e, 0xfe, 0xf0},
  {0x00, 0xe0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xf0},     /*sw */
  {0x00, 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e, 0xfe},
  {0x00, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0, 0x00},
  {0x00, 0xe0, 0xfe, 0x1e, 0x00, 0xf0, 0xfe, 0x0e},
  {0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe},     /*sw */
  {0x00, 0xfe, 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0},
  {0x00, 0xfe, 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e},
  {0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00},
  {0x1e, 0x00, 0x00, 0x1e, 0x0e, 0x00, 0x00, 0x0e},
  {0x1e, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x0e, 0x00},     /*sw */
  {0x1e, 0x00, 0xe0, 0xfe, 0x0e, 0x00, 0xf0, 0xfe},
  {0x1e, 0x00, 0xfe, 0xe0, 0x0e, 0x00, 0xfe, 0xf0},
  {0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x00},
  {0x1e, 0x1e, 0x1e, 0x1e, 0x0e, 0x0e, 0x0e, 0x0e},     /*w */
  {0x1e, 0x1e, 0xe0, 0xe0, 0x0e, 0x0e, 0xf0, 0xf0},
  {0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e, 0xfe, 0xfe},
  {0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0, 0x00, 0xfe},
  {0x1e, 0xe0, 0x1e, 0xe0, 0x0e, 0xf0, 0x0e, 0xf0},     /*sw */
  {0x1e, 0xe0, 0xe0, 0x1e, 0x0e, 0xf0, 0xf0, 0x0e},
  {0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0, 0xfe, 0x00},
  {0x1e, 0xfe, 0x00, 0xe0, 0x0e, 0xfe, 0x00, 0xf0},
  {0x1e, 0xfe, 0x1e, 0xfe, 0x0e, 0xfe, 0x0e, 0xfe},     /*sw */
  {0x1e, 0xfe, 0xe0, 0x00, 0x0e, 0xfe, 0xf0, 0x00},
  {0x1e, 0xfe, 0xfe, 0x1e, 0x0e, 0xfe, 0xfe, 0x0e},
  {0xe0, 0x00, 0x00, 0xe0, 0xf0, 0x00, 0x00, 0xf0},
  {0xe0, 0x00, 0x1e, 0xfe, 0xf0, 0x00, 0x0e, 0xfe},
  {0xe0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xf0, 0x00},     /*sw */
  {0xe0, 0x00, 0xfe, 0x1e, 0xf0, 0x00, 0xfe, 0x0e},
  {0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e, 0x00, 0xfe},
  {0xe0, 0x1e, 0x1e, 0xe0, 0xf0, 0x0e, 0x0e, 0xf0},
  {0xe0, 0x1e, 0xe0, 0x1e, 0xf0, 0x0e, 0xf0, 0x0e},     /*sw */
  {0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e, 0xfe, 0x00},
  {0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00},
  {0xe0, 0xe0, 0x1e, 0x1e, 0xf0, 0xf0, 0x0e, 0x0e},
  {0xe0, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0},     /*w */
  {0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0, 0xfe, 0xfe},
  {0xe0, 0xfe, 0x00, 0x1e, 0xf0, 0xfe, 0x00, 0x0e},
  {0xe0, 0xfe, 0x1e, 0x00, 0xf0, 0xfe, 0x0e, 0x00},
  {0xe0, 0xfe, 0xe0, 0xfe, 0xf0, 0xfe, 0xf0, 0xfe},     /*sw */
  {0xe0, 0xfe, 0xfe, 0xe0, 0xf0, 0xfe, 0xfe, 0xf0},
  {0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe},
  {0xfe, 0x00, 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0},
  {0xfe, 0x00, 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e},
  {0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00},     /*sw */
  {0xfe, 0x1e, 0x00, 0xe0, 0xfe, 0x0e, 0x00, 0xf0},
  {0xfe, 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e, 0xfe},
  {0xfe, 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0, 0x00},
  {0xfe, 0x1e, 0xfe, 0x1e, 0xfe, 0x0e, 0xfe, 0x0e},     /*sw */
  {0xfe, 0xe0, 0x00, 0x1e, 0xfe, 0xf0, 0x00, 0x0e},
  {0xfe, 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e, 0x00},
  {0xfe, 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0, 0xfe},
  {0xfe, 0xe0, 0xfe, 0xe0, 0xfe, 0xf0, 0xfe, 0xf0},     /*sw */
  {0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00},
  {0xfe, 0xfe, 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e},
  {0xfe, 0xfe, 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0},
  {0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe}      /*w */
};

bool
gl_des_is_weak_key (const char * key)
{
  char work[8];
  int i, left, right, middle, cmp_result;

  /* clear parity bits */
  for (i = 0; i < 8; ++i)
    work[i] = ((unsigned char)key[i]) & 0xfe;

  /* binary search in the weak key table */
  left = 0;
  right = 63;
  while (left <= right)
    {
      middle = (left + right) / 2;

      if (!(cmp_result = memcmp (work, weak_keys[middle], 8)))
        return -1;

      if (cmp_result > 0)
        left = middle + 1;
      else
        right = middle - 1;
    }

  return 0;
}

/*
 * Macro to swap bits across two words.
 */
#define DO_PERMUTATION(a, temp, b, offset, mask)        \
    temp = ((a>>offset) ^ b) & mask;                    \
    b ^= temp;                                          \
    a ^= temp<<offset;


/*
 * This performs the 'initial permutation' of the data to be encrypted
 * or decrypted. Additionally the resulting two words are rotated one bit
 * to the left.
 */
#define INITIAL_PERMUTATION(left, temp, right)          \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)    \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)   \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333)    \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)    \
    right =  (right << 1) | (right >> 31);              \
    temp  =  (left ^ right) & 0xaaaaaaaa;               \
    right ^= temp;                                      \
    left  ^= temp;                                      \
    left  =  (left << 1) | (left >> 31);

/*
 * The 'inverse initial permutation'.
 */
#define FINAL_PERMUTATION(left, temp, right)            \
    left  =  (left << 31) | (left >> 1);                \
    temp  =  (left ^ right) & 0xaaaaaaaa;               \
    left  ^= temp;                                      \
    right ^= temp;                                      \
    right  =  (right << 31) | (right >> 1);             \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)    \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333)    \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)   \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)


/*
 * A full DES round including 'expansion function', 'sbox substitution'
 * and 'primitive function P' but without swapping the left and right word.
 * Please note: The data in 'from' and 'to' is already rotated one bit to
 * the left, done in the initial permutation.
 */
#define DES_ROUND(from, to, work, subkey)               \
    work = from ^ *subkey++;                            \
    to ^= sbox8[  work      & 0x3f ];                   \
    to ^= sbox6[ (work>>8)  & 0x3f ];                   \
    to ^= sbox4[ (work>>16) & 0x3f ];                   \
    to ^= sbox2[ (work>>24) & 0x3f ];                   \
    work = ((from << 28) | (from >> 4)) ^ *subkey++;    \
    to ^= sbox7[  work      & 0x3f ];                   \
    to ^= sbox5[ (work>>8)  & 0x3f ];                   \
    to ^= sbox3[ (work>>16) & 0x3f ];                   \
    to ^= sbox1[ (work>>24) & 0x3f ];

/*
 * Macros to convert 8 bytes from/to 32bit words.
 */
#define READ_64BIT_DATA(data, left, right)                                 \
    left  = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];  \
    right = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

#define WRITE_64BIT_DATA(data, left, right)                                \
    data[0] = (left >> 24) &0xff; data[1] = (left >> 16) &0xff;    \
    data[2] = (left >> 8) &0xff; data[3] = left &0xff;                     \
    data[4] = (right >> 24) &0xff; data[5] = (right >> 16) &0xff;          \
    data[6] = (right >> 8) &0xff; data[7] = right &0xff;

/*
 * des_key_schedule():    Calculate 16 subkeys pairs (even/odd) for
 *                        16 encryption rounds.
 *                        To calculate subkeys for decryption the caller
 *                        have to reorder the generated subkeys.
 *
 *    rawkey:       8 Bytes of key data
 *    subkey:       Array of at least 32 uint32_ts. Will be filled
 *                  with calculated subkeys.
 *
 */
static void
des_key_schedule (const char * _rawkey, uint32_t * subkey)
{
  const unsigned char *rawkey = (const unsigned char *) _rawkey;
  uint32_t left, right, work;
  int round;

  READ_64BIT_DATA (rawkey, left, right)
    DO_PERMUTATION (right, work, left, 4, 0x0f0f0f0f)
    DO_PERMUTATION (right, work, left, 0, 0x10101010)
    left = ((leftkey_swap[(left >> 0) & 0xf] << 3)
            | (leftkey_swap[(left >> 8) & 0xf] << 2)
            | (leftkey_swap[(left >> 16) & 0xf] << 1)
            | (leftkey_swap[(left >> 24) & 0xf])
            | (leftkey_swap[(left >> 5) & 0xf] << 7)
            | (leftkey_swap[(left >> 13) & 0xf] << 6)
            | (leftkey_swap[(left >> 21) & 0xf] << 5)
            | (leftkey_swap[(left >> 29) & 0xf] << 4));

  left &= 0x0fffffff;

  right = ((rightkey_swap[(right >> 1) & 0xf] << 3)
           | (rightkey_swap[(right >> 9) & 0xf] << 2)
           | (rightkey_swap[(right >> 17) & 0xf] << 1)
           | (rightkey_swap[(right >> 25) & 0xf])
           | (rightkey_swap[(right >> 4) & 0xf] << 7)
           | (rightkey_swap[(right >> 12) & 0xf] << 6)
           | (rightkey_swap[(right >> 20) & 0xf] << 5)
           | (rightkey_swap[(right >> 28) & 0xf] << 4));

  right &= 0x0fffffff;

  for (round = 0; round < 16; ++round)
    {
      left = ((left << encrypt_rotate_tab[round])
              | (left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
      right = ((right << encrypt_rotate_tab[round])
               | (right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

      *subkey++ = (((left << 4) & 0x24000000)
                   | ((left << 28) & 0x10000000)
                   | ((left << 14) & 0x08000000)
                   | ((left << 18) & 0x02080000)
                   | ((left << 6) & 0x01000000)
                   | ((left << 9) & 0x00200000)
                   | ((left >> 1) & 0x00100000)
                   | ((left << 10) & 0x00040000)
                   | ((left << 2) & 0x00020000)
                   | ((left >> 10) & 0x00010000)
                   | ((right >> 13) & 0x00002000)
                   | ((right >> 4) & 0x00001000)
                   | ((right << 6) & 0x00000800)
                   | ((right >> 1) & 0x00000400)
                   | ((right >> 14) & 0x00000200)
                   | (right & 0x00000100)
                   | ((right >> 5) & 0x00000020)
                   | ((right >> 10) & 0x00000010)
                   | ((right >> 3) & 0x00000008)
                   | ((right >> 18) & 0x00000004)
                   | ((right >> 26) & 0x00000002)
                   | ((right >> 24) & 0x00000001));

      *subkey++ = (((left << 15) & 0x20000000)
                   | ((left << 17) & 0x10000000)
                   | ((left << 10) & 0x08000000)
                   | ((left << 22) & 0x04000000)
                   | ((left >> 2) & 0x02000000)
                   | ((left << 1) & 0x01000000)
                   | ((left << 16) & 0x00200000)
                   | ((left << 11) & 0x00100000)
                   | ((left << 3) & 0x00080000)
                   | ((left >> 6) & 0x00040000)
                   | ((left << 15) & 0x00020000)
                   | ((left >> 4) & 0x00010000)
                   | ((right >> 2) & 0x00002000)
                   | ((right << 8) & 0x00001000)
                   | ((right >> 14) & 0x00000808)
                   | ((right >> 9) & 0x00000400)
                   | ((right) & 0x00000200)
                   | ((right << 7) & 0x00000100)
                   | ((right >> 7) & 0x00000020)
                   | ((right >> 3) & 0x00000011)
                   | ((right << 2) & 0x00000004)
                   | ((right >> 21) & 0x00000002));
    }
}

void
gl_des_setkey (gl_des_ctx *ctx, const char * key)
{
  int i;

  des_key_schedule (key, ctx->encrypt_subkeys);

  for (i = 0; i < 32; i += 2)
    {
      ctx->decrypt_subkeys[i] = ctx->encrypt_subkeys[30 - i];
      ctx->decrypt_subkeys[i + 1] = ctx->encrypt_subkeys[31 - i];
    }
}

bool
gl_des_makekey (gl_des_ctx *ctx, const char * key, size_t keylen)
{
  if (keylen != 8)
    return false;

  gl_des_setkey (ctx, key);

  return !gl_des_is_weak_key (key);
}

void
gl_des_ecb_crypt (gl_des_ctx *ctx, const char * _from, char * _to, int mode)
{
  const unsigned char *from = (const unsigned char *) _from;
  unsigned char *to = (unsigned char *) _to;
  uint32_t left, right, work;
  uint32_t *keys;

  keys = mode ? ctx->decrypt_subkeys : ctx->encrypt_subkeys;

  READ_64BIT_DATA (from, left, right)
    INITIAL_PERMUTATION (left, work, right)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    FINAL_PERMUTATION (right, work, left)
    WRITE_64BIT_DATA (to, right, left)
}

void
gl_3des_set2keys (gl_3des_ctx *ctx, const char * key1, const char * key2)
{
  int i;

  des_key_schedule (key1, ctx->encrypt_subkeys);
  des_key_schedule (key2, &(ctx->decrypt_subkeys[32]));

  for (i = 0; i < 32; i += 2)
    {
      ctx->decrypt_subkeys[i] = ctx->encrypt_subkeys[30 - i];
      ctx->decrypt_subkeys[i + 1] = ctx->encrypt_subkeys[31 - i];

      ctx->encrypt_subkeys[i + 32] = ctx->decrypt_subkeys[62 - i];
      ctx->encrypt_subkeys[i + 33] = ctx->decrypt_subkeys[63 - i];

      ctx->encrypt_subkeys[i + 64] = ctx->encrypt_subkeys[i];
      ctx->encrypt_subkeys[i + 65] = ctx->encrypt_subkeys[i + 1];

      ctx->decrypt_subkeys[i + 64] = ctx->decrypt_subkeys[i];
      ctx->decrypt_subkeys[i + 65] = ctx->decrypt_subkeys[i + 1];
    }
}

void
gl_3des_set3keys (gl_3des_ctx *ctx, const char * key1,
                    const char * key2, const char * key3)
{
  int i;

  des_key_schedule (key1, ctx->encrypt_subkeys);
  des_key_schedule (key2, &(ctx->decrypt_subkeys[32]));
  des_key_schedule (key3, &(ctx->encrypt_subkeys[64]));

  for (i = 0; i < 32; i += 2)
    {
      ctx->decrypt_subkeys[i] = ctx->encrypt_subkeys[94 - i];
      ctx->decrypt_subkeys[i + 1] = ctx->encrypt_subkeys[95 - i];

      ctx->encrypt_subkeys[i + 32] = ctx->decrypt_subkeys[62 - i];
      ctx->encrypt_subkeys[i + 33] = ctx->decrypt_subkeys[63 - i];

      ctx->decrypt_subkeys[i + 64] = ctx->encrypt_subkeys[30 - i];
      ctx->decrypt_subkeys[i + 65] = ctx->encrypt_subkeys[31 - i];
    }
}

void
gl_3des_ecb_crypt (gl_3des_ctx *ctx,
                   const char * _from,
                   char * _to, int mode)
{
  const unsigned char *from = (const unsigned char *) _from;
  unsigned char *to = (unsigned char *) _to;
  uint32_t left, right, work;
  uint32_t *keys;

  keys = mode ? ctx->decrypt_subkeys : ctx->encrypt_subkeys;

  READ_64BIT_DATA (from, left, right)
    INITIAL_PERMUTATION (left, work, right)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    FINAL_PERMUTATION (right, work, left)
    WRITE_64BIT_DATA (to, right, left)
}

bool
gl_3des_makekey (gl_3des_ctx *ctx, const char * key, size_t keylen)
{
  if (keylen != 24)
    return false;

  gl_3des_set3keys (ctx, key, key + 8, key + 16);

  return !(gl_des_is_weak_key (key)
           || gl_des_is_weak_key (key + 8)
           || gl_des_is_weak_key (key + 16));
}
/* Functions to compute MD4 message digest of files or memory blocks.
   according to the definition of MD4 in RFC 1320 from April 1992.
   Copyright (C) 1995-1997, 1999-2003, 2005-2006, 2008-2013 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* Adapted by Simon Josefsson from gnulib md5.? and Libgcrypt
   cipher/md4.c . */

//#include <config.h>



//#include <stdalign.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if USE_UNLOCKED_IO

#endif

#ifdef WORDS_BIGENDIAN
# define SWAP(n)                                                        \
  (((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#else
# define SWAP(n) (n)
#endif

#define BLOCKSIZE 32768
#if BLOCKSIZE % 64 != 0
# error "invalid BLOCKSIZE"
#endif

/* This array contains the bytes used to pad the buffer to the next
   64-byte boundary.  (RFC 1320, 3.1: Step 1)  */
static const unsigned char fillbuf[64] = { 0x80, 0 /* , 0, 0, ...  */  };


/* Initialize structure containing state of computation.
   (RFC 1320, 3.3: Step 3)  */
void
md4_init_ctx (struct md4_ctx *ctx)
{
  ctx->A = 0x67452301;
  ctx->B = 0xefcdab89;
  ctx->C = 0x98badcfe;
  ctx->D = 0x10325476;

  ctx->total[0] = ctx->total[1] = 0;
  ctx->buflen = 0;
}

/* Copy the 4 byte value from v into the memory location pointed to by *cp,
   If your architecture allows unaligned access this is equivalent to
   * (uint32_t *) cp = v  */
static void
set_uint32 (char *cp, uint32_t v)
{
  memcpy (cp, &v, sizeof v);
}

/* Put result from CTX in first 16 bytes following RESBUF.  The result
   must be in little endian byte order.  */
void *
md4_read_ctx (const struct md4_ctx *ctx, void *resbuf)
{
  char *r = (char*)resbuf;
  set_uint32 (r + 0 * sizeof ctx->A, SWAP (ctx->A));
  set_uint32 (r + 1 * sizeof ctx->B, SWAP (ctx->B));
  set_uint32 (r + 2 * sizeof ctx->C, SWAP (ctx->C));
  set_uint32 (r + 3 * sizeof ctx->D, SWAP (ctx->D));

  return resbuf;
}

/* Process the remaining bytes in the internal buffer and the usual
   prolog according to the standard and write the result to RESBUF.  */
void *
md4_finish_ctx (struct md4_ctx *ctx, void *resbuf)
{
  /* Take yet unprocessed bytes into account.  */
  uint32_t bytes = ctx->buflen;
  size_t pad;

  /* Now count remaining bytes.  */
  ctx->total[0] += bytes;
  if (ctx->total[0] < bytes)
    ++ctx->total[1];

  pad = bytes >= 56 ? 64 + 56 - bytes : 56 - bytes;
  memcpy (&((char*)ctx->buffer)[bytes], fillbuf, pad);

  /* Put the 64-bit file length in *bits* at the end of the buffer.  */
  ctx->buffer[(bytes + pad) / 4] = SWAP (ctx->total[0] << 3);
  ctx->buffer[(bytes + pad) / 4 + 1] = SWAP ((ctx->total[1] << 3) |
                                             (ctx->total[0] >> 29));

  /* Process last bytes.  */
  md4_process_block (ctx->buffer, bytes + pad + 8, ctx);

  return md4_read_ctx (ctx, resbuf);
}

/* Compute MD4 message digest for bytes read from STREAM.  The
   resulting message digest number will be written into the 16 bytes
   beginning at RESBLOCK.  */
int
md4_stream (FILE * stream, void *resblock)
{
  struct md4_ctx ctx;
  size_t sum;

  char *buffer = (char*)malloc (BLOCKSIZE + 72);
  if (!buffer)
    return 1;

  /* Initialize the computation context.  */
  md4_init_ctx (&ctx);

  /* Iterate over full file contents.  */
  while (1)
    {
      /* We read the file in blocks of BLOCKSIZE bytes.  One call of the
         computation function processes the whole buffer so that with the
         next round of the loop another block can be read.  */
      size_t n;
      sum = 0;

      /* Read block.  Take care for partial reads.  */
      while (1)
        {
          n = fread (buffer + sum, 1, BLOCKSIZE - sum, stream);

          sum += n;

          if (sum == BLOCKSIZE)
            break;

          if (n == 0)
            {
              /* Check for the error flag IFF N == 0, so that we don't
                 exit the loop after a partial read due to e.g., EAGAIN
                 or EWOULDBLOCK.  */
              if (ferror (stream))
                {
                  free (buffer);
                  return 1;
                }
              goto process_partial_block;
            }

          /* We've read at least one byte, so ignore errors.  But always
             check for EOF, since feof may be true even though N > 0.
             Otherwise, we could end up calling fread after EOF.  */
          if (feof (stream))
            goto process_partial_block;
        }

      /* Process buffer with BLOCKSIZE bytes.  Note that
         BLOCKSIZE % 64 == 0
       */
      md4_process_block (buffer, BLOCKSIZE, &ctx);
    }

process_partial_block:;

  /* Process any remaining bytes.  */
  if (sum > 0)
    md4_process_bytes (buffer, sum, &ctx);

  /* Construct result in desired memory.  */
  md4_finish_ctx (&ctx, resblock);
  free (buffer);
  return 0;
}

/* Compute MD4 message digest for LEN bytes beginning at BUFFER.  The
   result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest.  */
void *
md4_buffer (const char *buffer, size_t len, void *resblock)
{
  struct md4_ctx ctx;

  /* Initialize the computation context.  */
  md4_init_ctx (&ctx);

  /* Process whole buffer but last len % 64 bytes.  */
  md4_process_bytes (buffer, len, &ctx);

  /* Put result in desired memory area.  */
  return md4_finish_ctx (&ctx, resblock);
}

void
md4_process_bytes (const void *buffer, size_t len, struct md4_ctx *ctx)
{
  /* When we already have some bits in our internal buffer concatenate
     both inputs first.  */
  if (ctx->buflen != 0)
    {
      size_t left_over = ctx->buflen;
      size_t add = 128 - left_over > len ? len : 128 - left_over;

      memcpy (&((char*)ctx->buffer)[left_over], buffer, add);
      ctx->buflen += add;

      if (ctx->buflen > 64)
        {
          md4_process_block (ctx->buffer, ctx->buflen & ~63, ctx);

          ctx->buflen &= 63;
          /* The regions in the following copy operation cannot overlap.  */
          memcpy (ctx->buffer, &((char*)ctx->buffer)[(left_over + add) & ~63],
                  ctx->buflen);
        }

      buffer = (const char *) buffer + add;
      len -= add;
    }

  /* Process available complete blocks.  */
  if (len >= 64)
    {
#if !_STRING_ARCH_unaligned
# define alignof __alignof
# define UNALIGNED_P(p) ((uintptr_t) (p) % alignof (uint32_t) != 0)
      if (UNALIGNED_P (buffer))
        while (len > 64)
          {
            md4_process_block (memcpy (ctx->buffer, buffer, 64), 64, ctx);
            buffer = (const char *) buffer + 64;
            len -= 64;
          }
      else
#endif
        {
          md4_process_block (buffer, len & ~63, ctx);
          buffer = (const char *) buffer + (len & ~63);
          len &= 63;
        }
    }

  /* Move remaining bytes in internal buffer.  */
  if (len > 0)
    {
      size_t left_over = ctx->buflen;

      memcpy (&((char*)ctx->buffer)[left_over], buffer, len);
      left_over += len;
      if (left_over >= 64)
        {
          md4_process_block (ctx->buffer, 64, ctx);
          left_over -= 64;
          memcpy (ctx->buffer, &ctx->buffer[16], left_over);
        }
      ctx->buflen = left_over;
    }
}

/* --- Code below is the primary difference between md5.c and md4.c --- */

/* MD4 round constants */
#define K1 0x5a827999
#define K2 0x6ed9eba1

/* Round functions.  */
#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define rol(x, n) (((x) << (n)) | ((uint32_t) (x) >> (32 - (n))))
#define R1(a,b,c,d,k,s) a=rol(a+F(b,c,d)+x[k],s);
#define R2(a,b,c,d,k,s) a=rol(a+G(b,c,d)+x[k]+K1,s);
#define R3(a,b,c,d,k,s) a=rol(a+H(b,c,d)+x[k]+K2,s);

/* Process LEN bytes of BUFFER, accumulating context into CTX.
   It is assumed that LEN % 64 == 0.  */

void
md4_process_block (const void *buffer, size_t len, struct md4_ctx *ctx)
{
  const uint32_t *words = (uint32_t *)buffer;
  size_t nwords = len / sizeof (uint32_t);
  const uint32_t *endp = words + nwords;
  uint32_t x[16];
  uint32_t A = ctx->A;
  uint32_t B = ctx->B;
  uint32_t C = ctx->C;
  uint32_t D = ctx->D;
  uint32_t lolen = len;

  /* First increment the byte count.  RFC 1320 specifies the possible
     length of the file up to 2^64 bits.  Here we only compute the
     number of bytes.  Do a double word increment.  */
  ctx->total[0] += lolen;
  ctx->total[1] += (len >> 31 >> 1) + (ctx->total[0] < lolen);

  /* Process all bytes in the buffer with 64 bytes in each round of
     the loop.  */
  while (words < endp)
    {
      int t;
      for (t = 0; t < 16; t++)
        {
          x[t] = SWAP (*words);
          words++;
        }

      /* Round 1.  */
      R1 (A, B, C, D, 0, 3);
      R1 (D, A, B, C, 1, 7);
      R1 (C, D, A, B, 2, 11);
      R1 (B, C, D, A, 3, 19);
      R1 (A, B, C, D, 4, 3);
      R1 (D, A, B, C, 5, 7);
      R1 (C, D, A, B, 6, 11);
      R1 (B, C, D, A, 7, 19);
      R1 (A, B, C, D, 8, 3);
      R1 (D, A, B, C, 9, 7);
      R1 (C, D, A, B, 10, 11);
      R1 (B, C, D, A, 11, 19);
      R1 (A, B, C, D, 12, 3);
      R1 (D, A, B, C, 13, 7);
      R1 (C, D, A, B, 14, 11);
      R1 (B, C, D, A, 15, 19);

      /* Round 2.  */
      R2 (A, B, C, D, 0, 3);
      R2 (D, A, B, C, 4, 5);
      R2 (C, D, A, B, 8, 9);
      R2 (B, C, D, A, 12, 13);
      R2 (A, B, C, D, 1, 3);
      R2 (D, A, B, C, 5, 5);
      R2 (C, D, A, B, 9, 9);
      R2 (B, C, D, A, 13, 13);
      R2 (A, B, C, D, 2, 3);
      R2 (D, A, B, C, 6, 5);
      R2 (C, D, A, B, 10, 9);
      R2 (B, C, D, A, 14, 13);
      R2 (A, B, C, D, 3, 3);
      R2 (D, A, B, C, 7, 5);
      R2 (C, D, A, B, 11, 9);
      R2 (B, C, D, A, 15, 13);

      /* Round 3.  */
      R3 (A, B, C, D, 0, 3);
      R3 (D, A, B, C, 8, 9);
      R3 (C, D, A, B, 4, 11);
      R3 (B, C, D, A, 12, 15);
      R3 (A, B, C, D, 2, 3);
      R3 (D, A, B, C, 10, 9);
      R3 (C, D, A, B, 6, 11);
      R3 (B, C, D, A, 14, 15);
      R3 (A, B, C, D, 1, 3);
      R3 (D, A, B, C, 9, 9);
      R3 (C, D, A, B, 5, 11);
      R3 (B, C, D, A, 13, 15);
      R3 (A, B, C, D, 3, 3);
      R3 (D, A, B, C, 11, 9);
      R3 (C, D, A, B, 7, 11);
      R3 (B, C, D, A, 15, 15);

      A = ctx->A += A;
      B = ctx->B += B;
      C = ctx->C += C;
      D = ctx->D += D;
    }
}
/* ntlm.h --- Header file for libntlm.                                -*- c -*-
 *
 * This file is part of libntlm.
 *
 * Libntlm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libntlm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libntlm; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#ifndef NTLM_H
# define NTLM_H

# ifdef __cplusplus
extern "C"
{
# endif

  /* Get FILE. */
#include <stdio.h>

  typedef unsigned short uint16;
  typedef unsigned int uint32;
  typedef unsigned char uint8;

#define NTLM_VERSION "1.4"

/*
 * These structures are byte-order dependant, and should not
 * be manipulated except by the use of the routines provided
 */

  typedef struct
  {
    uint16 len;
    uint16 maxlen;
    uint32 offset;
  } tSmbStrHeader;

  typedef struct
  {
    char ident[8];
    uint32 msgType;
    uint32 flags;
    tSmbStrHeader user;
    tSmbStrHeader domain;
    uint8 buffer[1024];
    uint32 bufIndex;
  } tSmbNtlmAuthRequest;

  typedef struct
  {
    char ident[8];
    uint32 msgType;
    tSmbStrHeader uDomain;
    uint32 flags;
    uint8 challengeData[8];
    uint8 reserved[8];
    tSmbStrHeader emptyString;
    uint8 buffer[1024];
    uint32 bufIndex;
  } tSmbNtlmAuthChallenge;


  typedef struct
  {
    char ident[8];
    uint32 msgType;
    tSmbStrHeader lmResponse;
    tSmbStrHeader ntResponse;
    tSmbStrHeader uDomain;
    tSmbStrHeader uUser;
    tSmbStrHeader uWks;
    tSmbStrHeader sessionKey;
    uint32 flags;
    uint8 buffer[1024];
    uint32 bufIndex;
  } tSmbNtlmAuthResponse;

/* public: */

#define SmbLength(ptr) (((ptr)->buffer - (uint8*)(ptr)) + (ptr)->bufIndex)

  extern void
    dumpSmbNtlmAuthRequest (FILE * fp, tSmbNtlmAuthRequest * request);
  extern void
    dumpSmbNtlmAuthChallenge (FILE * fp, tSmbNtlmAuthChallenge * challenge);
  extern void
    dumpSmbNtlmAuthResponse (FILE * fp, tSmbNtlmAuthResponse * response);

  extern void
    buildSmbNtlmAuthRequest (tSmbNtlmAuthRequest * request,
			     const char *user, const char *domain);

  /* Same as buildSmbNtlmAuthRequest, but won't treat @ in USER as a
     DOMAIN. */
  extern void
    buildSmbNtlmAuthRequest_noatsplit (tSmbNtlmAuthRequest * request,
				       const char *user, const char *domain);

  extern void
    buildSmbNtlmAuthResponse (tSmbNtlmAuthChallenge * challenge,
			      tSmbNtlmAuthResponse * response,
			      const char *user, const char *password);

  /* Same as buildSmbNtlmAuthResponse, but won't treat @ in USER as a
     REALM. */
  extern void
    buildSmbNtlmAuthResponse_noatsplit (tSmbNtlmAuthChallenge * challenge,
					tSmbNtlmAuthResponse * response,
					const char *user,
					const char *password);

  /* smbencrypt.c */
  extern void
  ntlm_smb_encrypt (const char *passwd,
		    const uint8 * challenge,
		    uint8 * answer);
  extern void
  ntlm_smb_nt_encrypt (const char *passwd,
		       const uint8 * challenge,
		       uint8 * answer);

//  extern const char *ntlm_check_version (const char *req_version);

# ifdef __cplusplus
}
# endif

#endif				/* NTLM_H */
/*
 * Copyright (C) 2005, 2006, 2007, 2008 Simon Josefsson
 * Copyright (C) 1998-1999  Brian Bruns
 * Copyright (C) 2004 Frediano Ziglio
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

//#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <unistd.h>
//#include <byteswap.h>






/* C89 compliant way to cast 'char' to 'unsigned char'. */
static __inline unsigned char
to_uchar (char ch)
{
  return ch;
}

/*
 * The following code is based on some psuedo-C code from ronald@innovation.ch
 */

static void ntlm_encrypt_answer (char *hash,
				 const char *challenge, char *answer);
static void ntlm_convert_key (char *key_56, gl_des_ctx * ks);

void
ntlm_smb_encrypt (const char *passwd, const uint8 * challenge, uint8 * answer)
{
#define MAX_PW_SZ 14
  int len;
  int i;
  static const char magic[8] =
    { 0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 };
  gl_des_ctx ks;
  char hash[24];
  char passwd_up[MAX_PW_SZ];

  /* convert password to upper and pad to 14 chars */
  memset (passwd_up, 0, MAX_PW_SZ);
  len = strlen (passwd);
  if (len > MAX_PW_SZ)
    len = MAX_PW_SZ;
  for (i = 0; i < len; i++)
    passwd_up[i] = toupper (passwd[i]);

  /* hash the first 7 characters */
  ntlm_convert_key (passwd_up, &ks);
  gl_des_ecb_encrypt (&ks, magic, hash + 0);

  /* hash the second 7 characters */
  ntlm_convert_key (passwd_up + 7, &ks);
  gl_des_ecb_encrypt (&ks, magic, hash + 8);

  memset (hash + 16, 0, 5);

  ntlm_encrypt_answer (hash, (char*)challenge, (char*)answer);

  /* with security is best be pedantic */
  memset (&ks, 0, sizeof (ks));
  memset (hash, 0, sizeof (hash));
  memset (passwd_up, 0, sizeof (passwd_up));
}

void
ntlm_smb_nt_encrypt (const char *passwd,
		     const uint8 * challenge,
		     uint8 * answer)
{
  size_t len, i;
  unsigned char hash[24];
  unsigned char nt_pw[256];

  /* NT resp */
  len = strlen (passwd);
  if (len > 128)
    len = 128;
  for (i = 0; i < len; ++i)
    {
      nt_pw[2 * i] = passwd[i];
      nt_pw[2 * i + 1] = 0;
    }

  md4_buffer ((char*)nt_pw, len * 2, hash);

  memset (hash + 16, 0, 5);
  ntlm_encrypt_answer ((char*)hash, (char*)challenge, (char*)answer);

  /* with security is best be pedantic */
  memset (hash, 0, sizeof (hash));
  memset (nt_pw, 0, sizeof (nt_pw));
}

/*
 * takes a 21 byte array and treats it as 3 56-bit DES keys. The
 * 8 byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
static void
ntlm_encrypt_answer (char *hash, const char *challenge, char *answer)
{
  gl_des_ctx ks;

  ntlm_convert_key (hash, &ks);
  gl_des_ecb_encrypt (&ks, challenge, answer);

  ntlm_convert_key (&hash[7], &ks);
  gl_des_ecb_encrypt (&ks, challenge, &answer[8]);

  ntlm_convert_key (&hash[14], &ks);
  gl_des_ecb_encrypt (&ks, challenge, &answer[16]);

  memset (&ks, 0, sizeof (ks));
}

/*
 * turns a 56 bit key into the 64 bit, and sets the key schedule ks.
 */
static void
ntlm_convert_key (char *key_56, gl_des_ctx * ks)
{
  char key[8];

  key[0] = to_uchar (key_56[0]);
  key[1] = ((to_uchar (key_56[0]) << 7) & 0xFF) | (to_uchar (key_56[1]) >> 1);
  key[2] = ((to_uchar (key_56[1]) << 6) & 0xFF) | (to_uchar (key_56[2]) >> 2);
  key[3] = ((to_uchar (key_56[2]) << 5) & 0xFF) | (to_uchar (key_56[3]) >> 3);
  key[4] = ((to_uchar (key_56[3]) << 4) & 0xFF) | (to_uchar (key_56[4]) >> 4);
  key[5] = ((to_uchar (key_56[4]) << 3) & 0xFF) | (to_uchar (key_56[5]) >> 5);
  key[6] = ((to_uchar (key_56[5]) << 2) & 0xFF) | (to_uchar (key_56[6]) >> 6);
  key[7] = (to_uchar (key_56[6]) << 1) & 0xFF;

  gl_des_setkey (ks, key);

  memset (&key, 0, sizeof (key));
}


/** \@} */
/* smbutil.c --- Main library functions.
 * Copyright (C) 2002, 2004, 2005, 2006, 2008 Simon Josefsson
 * Copyright (C) 1999-2001 Grant Edwards
 * Copyright (C) 2004 Frediano Ziglio
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

//#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <unistd.h>




#ifdef NTLM_UNIQUE_MODULE



#else


#endif

///* Define to the full name and version of this package. */
//#define PACKAGE_STRING "libntlm 1.4"
//
//char versionString[] = PACKAGE_STRING;

/* Utility routines that handle NTLM auth structures. */

/*
 * Must be multiple of two
 * We use a statis buffer of 1024 bytes for message
 * At maximun we but 48 bytes (ntlm responses) and 3 unicode strings so
 * NTLM_BUFSIZE * 3 + 48 <= 1024
 */
#define NTLM_BUFSIZE 320

/*
 * all bytes in our structures are aligned so just swap bytes so 
 * we have just to swap order 
 */
#ifdef WORDS_BIGENDIAN
# define UI16LE(n) bswap_16(n)
# define UI32LE(n) bswap_32(n)
#else
# define UI16LE(n) (n)
# define UI32LE(n) (n)
#endif

/* I am not crazy about these macros -- they seem to have gotten
 * a bit complex.  A new scheme for handling string/buffer fields
 * in the structures probably needs to be designed
 */
#define AddBytes(ptr, header, buf, count) \
{ \
  ptr->header.len = ptr->header.maxlen = UI16LE(count); \
  ptr->header.offset = UI32LE((ptr->buffer - ((uint8*)ptr)) + ptr->bufIndex); \
  memcpy(ptr->buffer+ptr->bufIndex, buf, count); \
  ptr->bufIndex += count; \
}

#define AddString(ptr, header, string) \
{ \
const char *p = (string); \
size_t len = p ? strlen(p) : 0; \
AddBytes(ptr, header, p, len); \
}

#define AddUnicodeStringLen(ptr, header, string, len) \
{ \
unsigned char buf[NTLM_BUFSIZE]; \
unsigned char *b = strToUnicode(string, len, buf); \
AddBytes(ptr, header, b, len*2); \
}

#define AddUnicodeString(ptr, header, string) \
{ \
size_t len = strlen(string); \
AddUnicodeStringLen(ptr, header, string, len); \
}

#define GetUnicodeString(structPtr, header, output) \
getUnicodeString(UI32LE(structPtr->header.offset), UI16LE(structPtr->header.len), ((char*)structPtr), (structPtr->buffer - (uint8*) structPtr), sizeof(structPtr->buffer), output)
#define GetStrinG(structPtr, header, output) \
getString(UI32LE(structPtr->header.offset), UI16LE(structPtr->header.len), ((char*)structPtr), (structPtr->buffer - (uint8*) structPtr), sizeof(structPtr->buffer), output)
#define DumpBuffer(fp, structPtr, header) \
dumpBuffer(fp, UI32LE(structPtr->header.offset), UI16LE(structPtr->header.len), ((char*)structPtr), (structPtr->buffer - (uint8*) structPtr), sizeof(structPtr->buffer))


static void
dumpRaw (FILE * fp, const unsigned char *buf, size_t len)
{
  size_t i;

  for (i = 0; i < len; ++i)
    fprintf (fp, "%02x ", buf[i]);

  fprintf (fp, "\n");
}

static __inline void
dumpBuffer (FILE * fp, uint32 offset, uint32 len, char *structPtr,
	    size_t buf_start, size_t buf_len)
{
  /* prevent buffer reading overflow */
  if (offset < buf_start || offset > buf_len + buf_start
      || offset + len > buf_len + buf_start)
    len = 0;
  dumpRaw (fp, (const unsigned char*)structPtr + offset, len);
}

static char *
unicodeToString (const char *p, size_t len, char *buf)
{
  size_t i;

  if (len >= NTLM_BUFSIZE)
    len = NTLM_BUFSIZE - 1;

  for (i = 0; i < len; ++i)
    {
      buf[i] = *p & 0x7f;
      p += 2;
    }

  buf[i] = '\0';
  return buf;
}

static __inline char *
getUnicodeString (uint32 offset, uint32 len, char *structPtr,
		  size_t buf_start, size_t buf_len, char *output)
{
  /* prevent buffer reading overflow */
  if (offset < buf_start || offset > buf_len + buf_start
      || offset + len > buf_len + buf_start)
    len = 0;
  return unicodeToString (structPtr + offset, len / 2, output);
}

static unsigned char *
strToUnicode (const char *p, size_t l, unsigned char *buf)
{
  int i = 0;

  if (l > (NTLM_BUFSIZE / 2))
    l = (NTLM_BUFSIZE / 2);

  while (l--)
    {
      buf[i++] = *p++;
      buf[i++] = 0;
    }

  return buf;
}

static char *
toString (const char *p, size_t len, char *buf)
{
  if (len >= NTLM_BUFSIZE)
    len = NTLM_BUFSIZE - 1;

  memcpy (buf, p, len);
  buf[len] = 0;
  return buf;
}

static __inline char *
getString (uint32 offset, uint32 len, char *structPtr, size_t buf_start,
	   size_t buf_len, char *output)
{
  /* prevent buffer reading overflow */
  if (offset < buf_start || offset > buf_len + buf_start
      || offset + len > buf_len + buf_start)
    len = 0;
  return toString (structPtr + offset, len, output);
}

void
dumpSmbNtlmAuthRequest (FILE * fp, tSmbNtlmAuthRequest * request)
{
  char buf1[NTLM_BUFSIZE], buf2[NTLM_BUFSIZE];
  fprintf (fp, "NTLM Request:\n"
	   "      Ident = %.8s\n"
	   "      mType = %d\n"
	   "      Flags = %08x\n"
	   "       User = %s\n"
	   "     Domain = %s\n",
	   request->ident,
	   UI32LE (request->msgType),
	   UI32LE (request->flags),
	   GetStrinG (request, user, buf1),
	   GetStrinG (request, domain, buf2));
}

void
dumpSmbNtlmAuthChallenge (FILE * fp, tSmbNtlmAuthChallenge * challenge)
{
  unsigned char buf[NTLM_BUFSIZE];
  fprintf (fp, "NTLM Challenge:\n"
	   "      Ident = %.8s\n"
	   "      mType = %d\n"
	   "     Domain = %s\n"
	   "      Flags = %08x\n"
	   "  Challenge = ",
	   challenge->ident,
	   UI32LE (challenge->msgType),
	   GetUnicodeString (challenge, uDomain, (char*)buf),
	   UI32LE (challenge->flags));
  dumpRaw (fp, challenge->challengeData, 8);
}

void
dumpSmbNtlmAuthResponse (FILE * fp, tSmbNtlmAuthResponse * response)
{
  unsigned char buf1[NTLM_BUFSIZE], buf2[NTLM_BUFSIZE], buf3[NTLM_BUFSIZE];
  fprintf (fp, "NTLM Response:\n"
	   "      Ident = %.8s\n"
	   "      mType = %d\n"
	   "     LmResp = ", response->ident, UI32LE (response->msgType));
  DumpBuffer (fp, response, lmResponse);
  fprintf (fp, "     NTResp = ");
  DumpBuffer (fp, response, ntResponse);
  fprintf (fp, "     Domain = %s\n"
	   "       User = %s\n"
	   "        Wks = %s\n"
	   "       sKey = ",
	   GetUnicodeString (response, uDomain, (char*)buf1),
	   GetUnicodeString (response, uUser, (char*)buf2),
	   GetUnicodeString (response, uWks, (char*)buf3));
  DumpBuffer (fp, response, sessionKey);
  fprintf (fp, "      Flags = %08x\n", UI32LE (response->flags));
}

static void
buildSmbNtlmAuthRequest_userlen (tSmbNtlmAuthRequest * request,
				 const char *user,
				 size_t user_len, const char *domain)
{
  request->bufIndex = 0;
  memcpy (request->ident, "NTLMSSP\0\0\0", 8);
  request->msgType = UI32LE (1);
  request->flags = UI32LE (0x0000b207);	/* have to figure out what these mean */
  /* FIXME this should be workstation, not username */
  AddBytes (request, user, user, user_len);
  AddString (request, domain, domain);
}

void
buildSmbNtlmAuthRequest (tSmbNtlmAuthRequest * request,
			 const char *user, const char *domain)
{
  const char *p = strchr (user, '@');
  size_t user_len = strlen (user);

  if (p)
    {
      if (!domain)
	domain = p + 1;
      user_len = p - user;
    }

  buildSmbNtlmAuthRequest_userlen (request, user, user_len, domain);
}

void
buildSmbNtlmAuthRequest_noatsplit (tSmbNtlmAuthRequest * request,
				   const char *user, const char *domain)
{
  buildSmbNtlmAuthRequest_userlen (request, user, strlen (user), domain);
}


static void
buildSmbNtlmAuthResponse_userlen (tSmbNtlmAuthChallenge * challenge,
				  tSmbNtlmAuthResponse * response,
				  const char *user, size_t user_len,
				  const char *domain, const char *password)
{
  uint8 lmRespData[24];
  uint8 ntRespData[24];

  ntlm_smb_encrypt (password, challenge->challengeData, lmRespData);
  ntlm_smb_nt_encrypt (password, challenge->challengeData, ntRespData);

  response->bufIndex = 0;
  memcpy (response->ident, "NTLMSSP\0\0\0", 8);
  response->msgType = UI32LE (3);

  AddUnicodeString (response, uDomain, domain);
  AddUnicodeStringLen (response, uUser, user, user_len);
  /* TODO just a dummy value for workstation */
  AddUnicodeStringLen (response, uWks, user, user_len);
  AddBytes (response, lmResponse, lmRespData, 24);
  AddBytes (response, ntResponse, ntRespData, 24);
  AddString (response, sessionKey, NULL);

  response->flags = challenge->flags;
}

void
buildSmbNtlmAuthResponse (tSmbNtlmAuthChallenge * challenge,
			  tSmbNtlmAuthResponse * response,
			  const char *user, const char *password)
{
  const char *p = strchr (user, '@');
  size_t user_len = strlen (user);
  unsigned char buf[NTLM_BUFSIZE];
  const char *domain = GetUnicodeString (challenge, uDomain, (char*)buf);

  if (p)
    {
      domain = p + 1;
      user_len = p - user;
    }

  buildSmbNtlmAuthResponse_userlen (challenge, response,
				    user, user_len, domain, password);
}

void
buildSmbNtlmAuthResponse_noatsplit (tSmbNtlmAuthChallenge * challenge,
				    tSmbNtlmAuthResponse * response,
				    const char *user, const char *password)
{
  unsigned char buf[NTLM_BUFSIZE];
  const char *domain = GetUnicodeString (challenge, uDomain, (char*)buf);

  buildSmbNtlmAuthResponse_userlen (challenge, response,
				    user, strlen (user), domain, password);
}
