/* Useful if you wish to make target-specific gcc changes. */
#undef TARGET_OST
#define TARGET_OST 1

/* Default arguments you want when running your
   i686-ost-gcc/x86_64-ost-gcc toolchain */
#define LIB_SPEC "-lc -lg -lm" /* link against C standard libraries */
                               /* modify this based on your needs */

/* Don't automatically add extern "C" { } around header files. */
#undef  NO_IMPLICIT_EXTERN_C
#define NO_IMPLICIT_EXTERN_C 1

/* Additional predefined macros. */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
  do {                                \
    builtin_define ("__ost__");      \
    builtin_define ("__unix__");      \
    builtin_assert ("system=ost");   \
    builtin_assert ("system=unix");   \
    builtin_assert ("system=posix");   \
  } while(0);

