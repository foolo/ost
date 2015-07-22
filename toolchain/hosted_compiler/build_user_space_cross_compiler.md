# Prerequisites

## Install autoconf 2.64

	sudo apt-get install m4
	wget http://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.xz

Extract, configure, make, make install

## Install automake 1.11.1

	wget http://ftp.gnu.org/gnu/automake/automake-1.11.1.tar.bz2

Extract, configure, make, make install

## Install libtool
sudo apt-get install libtool-bin



# Building

## Download and extract sources

./download_sources.sh
mkdir -p build
for f in download/*.tar*; do tar xf $f --directory=build; done
cd build

git init
git add *
git commit -m initial

## Apply patch

	git apply ../modify-gcc-and-binutils-32-64.patch

Now check the local modifications that the patch created
Optional, commit changes to keep track of later modifications

git add *
git commit -m modify-gcc-and-binutils

## Run automake and autoconf

	pushd binutils-2.24/ld
	automake
	popd

	pushd gcc-4.9.2/libstdc++-v3/
	autoconf
	popd


# Alternative to the patch

## Edit binutils-2.24/config.sub

	# First accept the basic system types.
	...
	      | -aos* | -aros* | -ost* \

## Edit binutils-2.24/bfd/config.bfd

	  i[3-7]86-*-ost*)
	    targ_defvec=bfd_elf32_i386_vec
	    targ_selvecs=
	    targ64_selvecs=bfd_elf64_x86_64_vec
	    ;;
#ifdef BFD64
  x86_64-*-ost*)
    targ_defvec=bfd_elf64_x86_64_vec
    targ_selvecs=bfd_elf32_i386_vec
    want64=true
    ;;
#endif

## Edit binutils-2.24/gas/configure.tgt

	case ${generic_target} in
	  ...
	  i386-*-ost*)    fmt=elf ;;
	  ...

## Edit ld/configure.tgt

	...
	i[3-7]86-*-ost*)
				targ_emul=elf_i386_ost
				targ_extra_emuls=elf_i386
				targ64_extra_emuls="elf_x86_64_ost elf_x86_64"
				;;
	x86_64-*-ost*)
				targ_emul=elf_x86_64_ost
				targ_extra_emuls=elf_i386_ost elf_x86_64 elf_i386
				;;
	...


## Create binutils-2.24/ld/emulparams/elf_i386_ost.sh

	. ${srcdir}/emulparams/elf_i386.sh
	GENERATE_SHLIB_SCRIPT=yes
	GENERATE_PIE_SCRIPT=yes


## Create binutils-2.24/ld/emulparams/elf_x86_64_ost.sh

	. ${srcdir}/emulparams/elf_i386.sh


## Edit binutils-2.24/ld/Makefile.am
After rule for eelf_i386.c, add

	eelf_i386_ost.c: $(srcdir)/emulparams/elf_i386_ost.sh \
	  $(ELF_DEPS) $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
		${GENSCRIPTS} elf_i386_ost "$(tdir_elf_i386_ost)"

After rule eelf_x86_64.c, add

	eelf_x86_64_ost.c: $(srcdir)/emulparams/elf_x86_64_ost.sh \
	  $(ELF_DEPS) $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
		${GENSCRIPTS} elf_x86_64_ost "$(tdir_elf_x86_64_ost)"

And add eelf_i386_ost.c to ALL_EMULATION_SOURCES

	...
	eelf_i386.c \
	eelf_i386_ost.c \
	eelf_i386_be.c \
	...


## TODO run automake


## Edit gcc-4.9.2/config.sub

	# First accept the basic system types.
	...
	      | -aos* | -aros* | -ost* \


## Edit gcc-4.9.2/gcc/config.gcc

	# Common parts for widely ported systems.
	case ${target} in
	*-*-ost*)
	  gas=yes
	  gnu_ld=yes
	  default_use_cxa_atexit=yes
	  ;;
	*-*-darwin*)

Edit

Line around 888

	case ${target} in
	...
	i[34567]86-*-ost*)
	    tm_file="${tm_file} i386/unix.h i386/att.h dbxelf.h elfos.h i386/i386elf.h ost.h"
	    ;;
	...
	aarch64*-*-elf)
		tm_file="${tm_file} dbxelf.h elfos.h newlib-stdint.h"


## Create gcc-4.9.2/gcc/config/ost.h

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

## Edit gcc-4.9.2/libstdc++-v3/crossconfig.m4

	  *-ost*)
	    GLIBCXX_CHECK_COMPILER_FEATURES
	    GLIBCXX_CHECK_LINKER_FEATURES
	    GLIBCXX_CHECK_MATH_SUPPORT
	    GLIBCXX_CHECK_STDLIB_SUPPORT
	    ;;

## Edit gcc-4.9.2/libgcc/config.host

	...
	case ${host} in
	i[34567]86-*-ost*)
		extra_parts="$extra_parts crtbegin.o crtend.o"
		tmake_file="$tmake_file i386/t-crtstuff t-crtstuff-pic t-libgcc-pic"
	aarch64*-*-elf)
		extra_parts="$extra_parts crtbegin.o crtend.o crti.o crtn.o"
	...

## Edit gcc-4.9.2/fixincludes/mkfixinc.sh

	...
	case $machine in
	    *-ost* | \
	...
