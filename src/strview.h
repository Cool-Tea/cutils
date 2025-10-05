#ifndef STRVIEW_H
#define STRVIEW_H

#include <stdint.h>
#include <string.h>

#define sv_min(a, b) ((b) > (a) ? (a) : (b))
#define sv_max(a, b) ((b) > (a) ? (b) : (a))

typedef struct string_view {
  uint32_t size;
  const char* data;
} strview_t;

#define sv_fromstr(str) ((strview_t){.size = strlen(str), .data = (str)})

#define sv_fromnstr(str, len) ((strview_t){.size = (len), .data = (str)})

#define sv_eq(lsv, rsv)         \
  ((lsv).size != (rsv).size ? 0 \
                            : memcmp((lsv).data, (rsv).data, (lsv).size) == 0)

#define sv_eqstr(sv, str) sv_eq((sv), sv_fromstr(str))

#define sv_startwith(sv, pfsv) \
  ((sv).size < (pfsv).size ? 0 \
                           : memcmp((sv).data, (pfsv).data, (pfsv).size) == 0)

#define sv_startwithstr(sv, prefix) sv_startwith((sv), sv_fromstr(prefix))

#define sv_endwith(sv, sfsv)                                             \
  ((sv).size < (sfsv).size ? 0                                           \
                           : memcmp((sv).data + (sv).size - (sfsv).size, \
                                    (sfsv).data, (sfsv).size) == 0)

#define sv_endwithstr(sv, suffix) sv_endwith((sv), sv_fromstr(suffix))

#define sv_subview(sv, i, len)                             \
  ((strview_t){.size = sv_min((len), (sv).size - (i) - 1), \
               .data = (sv).data + (i)})

#define sv_printfmt "%.*s"

#define sv_printarg(sv) (sv).size, (sv).data

#ifdef STRBDR_H
#define sb_toview(sb) ((strview_t){.size = (sb)->size, .data = (sb)->data})
#define sv_frombdr(sb) ((strview_t){.size = (sb)->size, .data = (sb)->data})
#endif

#endif  // STRVIEW_H