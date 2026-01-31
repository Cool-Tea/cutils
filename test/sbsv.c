#include <stdio.h>
#include <strbdr.h>
#include <strview.h>

void strbdr_test() {
  printf("==== String Builder Test ====\n");

  strbdr_t sb = {0};

  sb_concat(&sb, "Hello string builder!");
  sb_append(&sb, '\n');
  printf(sb_printfmt, sb_printarg(&sb));

  sb_nconcat(&sb, "a string buffer. ^^vv<><>abab", 16);
  sb_append(&sb, '\n');
  sb_insert(&sb, 22, "This is ", 8);
  printf(sb_printfmt, sb_printarg(&sb));

  sb_clear(&sb);

  sb_concatf(&sb, "Formatted number: %d, string: %s\n", 12345, "Hello");
  printf(sb_printfmt, sb_printarg(&sb));

  sb_resize(&sb, 10);
  sb_append(&sb, '\n');
  printf(sb_printfmt, sb_printarg(&sb));

  sb_free(&sb);
}

void strview_test() {
  printf("==== String View Test ====\n");

  strview_t sv = sv_fromstr("Hello, string view!");
  printf(sv_printfmt "\n", sv_printarg(sv));

  sv = sv_subview(sv, 8, 11);
  printf(sv_printfmt "\n", sv_printarg(sv));

  sv = sv_fromstr("pseudoscience");
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

  sv = sv_fromstr("token1,token2;token3 token4");
  printf("Tokenizing string: " sv_printfmt "\n", sv_printarg(sv));
  while (sv.size > 0) {
    strview_t token = sv_tokstr(&sv, ",; ");
    printf("Token: " sv_printfmt "\n", sv_printarg(token));
  }

  sv = sv_fromstr("another|test|string|view");
  printf("Tokenizing string: " sv_printfmt "\n", sv_printarg(sv));
  while (sv.size > 0) {
    strview_t token = sv_tok(&sv, sv_fromstr("|"));
    printf("Token: " sv_printfmt "\n", sv_printarg(token));
  }
}

void strbdr_strview_conversion_test() {
  printf("==== String Builder and String View Conversion Test ====\n");

  strbdr_t sb = {0};
  sb_concatf(&sb, "What is the result of %s?", "1 + 2");
  sb_append(&sb, '\n');
  sb_concatf(&sb, "Obviously, it's %d.", 1 + 2);
  sb_append(&sb, '\n');

  strview_t sv = sv_frombdr(&sb);

  printf(sv_printfmt, sv_printarg(sv));

  sb_free(&sb);
}

int main() {
  strbdr_test();
  strview_test();
  strbdr_strview_conversion_test();
  return 0;
}