# Poly1305

## Input
- 16-byte secret key $r$
- $L$-byte message $m$

## Output
- 16-byte hash 

## Definition

1. Interprets $r$ as 16-byte integer.
2. Creates 16-byte chunks: $m=(m[0],m[1],m[2],...,m[L-1])$.
3. Interprets the 16-byte chunks as 17-byte little-endian integers by appending 1 byte to each chunk. These will be used as coefficients of a polynomial.
4. Evaluates the polynomial at the point $r\mod{(2^{130}-5)}$.
5. Reduces the polynomial result $\mod{2^{128}}$ and encodes it in little-endian to return a 16-byte hash.

### Coefficients
- The coefficients $c_i$ of the polynomial $c_1r^q + c_2r^{q-1} + \cdots + c_qr$ where $q = \lceil L/16 \rceil$, are defined as:
  $ 
  c_i = m[16i - 16] + 2^8m[16i - 15] + 2^{16}m[16i - 14] + \cdots + 2^{120}m[16i - 1] + 2^{128} 
  $
- For the last chunk, if $L \not\equiv 0 \pmod{16}$, then:
  $
  c_q = m[16q - 16] + 2^8m[16q - 15] + \cdots + 2^{8(L \bmod 16) - 8}m[L - 1] + 2^{8(L \bmod 16)}
  $

### Key Restrictions
- The secret key $r$ must adhere to specific restrictions to ensure security:
  - Bytes $r[3], r[7], r[11], r[15]$ are limited to values $0, 1, 2, \ldots, 15$ (top four bits are clear).
  - Bytes $r[4], r[8], r[12]$ are limited to values $0, 4, 8, \ldots, 252$ (bottom two bits are clear).
- There are $2^{106}$ distinct possible values for $r$.