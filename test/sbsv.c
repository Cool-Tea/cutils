#include <stdio.h>
#include <strbdr.h>
#include <strview.h>

int main() {
  strbdr_t sb = {0};

  sb_concat(&sb, "Hello string builder!");
  sb_append(&sb, '\n');
  sb_nconcat(&sb, "a string buffer. ^^vv<><>abab", 16);
  sb_append(&sb, '\n');

  sb_insert(&sb, 22, "This is ", 8);

  printf(sb_printfmt, sb_printarg(&sb));

  sb_clear(&sb);
  sb_concatf(&sb, "What is the result of %s?", "1 + 2");
  sb_append(&sb, '\n');
  sb_concatf(&sb, "Obviously, it's %d.", 1 + 2);
  sb_append(&sb, '\n');

  strview_t sv = sv_frombdr(&sb);

  printf(sv_printfmt, sv_printarg(sv));

  sv = sv_subview(sv, 5, 2);
  printf(sv_printfmt "\n", sv_printarg(sv));

  sb_clear(&sb);
  sb_concatf(&sb, "pseudoscience");
  sv = sb_toview(&sb);
  printf("sv_eq(sv, \"pseudoscience\") = %d\n",
         sv_eq(sv, sv_fromstr("pseudoscience")));
  printf("sv_eqstr(sv, \"pseudocode\") = %d\n", sv_eqstr(sv, "pseudocode"));
  printf("sv_startwithstr(sv, \"pseudo\") = %d\n",
         sv_startwithstr(sv, "pseudo"));
  printf("sv_startwith(sv, \"pseudoc\") = %d\n",
         sv_startwith(sv, sv_fromstr("pseudoc")));
  printf("sv_endwithstr(sv, \"ence\") = %d\n", sv_endwithstr(sv, "ence"));
  printf("sv_endwith(sv, \"conscience\") = %d\n",
         sv_endwith(sv, sv_fromstr("conscience")));

  sb_free(&sb);
  return 0;
}