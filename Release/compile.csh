#!/bin/csh -f

# See README.txt for license.

# Simple script to illustrate recommended way of compiling C Ken
# applications, which is somewhat unconventional.  By compiling
# all source files together we enable the compiler to perform
# more thorough checks.  Such checks could in principle be
# performed by a lint-like tool (e.g., HP's Code Advisor, PC-Lint/
# FlexeLint, or the Coverity static checker).  Because such tools
# aren't used in practice as often as they should be, we encourage
# the strongest possible compiler checks as a matter of routine.
# Some versions of gcc require the "-combine" flag to treat all
# given .c files as a single translation unit.
#
# See "man gcc" and other relevant documentation for the runtime
# checks enabled by FORTIFY_SOURCE, -ftrapv, -fstack-protector, and
# -fmudflap; decide if they're right for your needs.  Note that some
# versions of gcc don't implement -ftrapv very well, so it would be
# unwise to rely heavily upon this feature without thoroughly testing
# it on your gcc.
#
# To compile a C Ken application, invoke this script, listing all
# of an application's .c files on the command line, like this:
#
#    % ./compile.csh app01.c
#
# If your app uses the interface in kenvar.h you'd need to specify
# kenvar.c on the compile line also. This script yields a clean
# compile with gcc 4.6.1 on an Ubuntu Linux laptop in August 2012.
# If you see many warning messages, you might be working with a
# different version.


# run this occasionally and ponder output:
#    -Wconversion -Wpadded
# this one warns about mudflap implementation:
#    -Wnested-externs
set fw = '-Wall -W -Wextra -Wundef -Wshadow -Wunreachable-code -Wredundant-decls -Wunused-macros -Wcast-qual -Wcast-align -Wwrite-strings -Wmissing-field-initializers -Wendif-labels -Winit-self -Wlogical-op -Wmissing-declarations -Wpacked -Wstack-protector -Wformat=2 -Wswitch-default -Wswitch-enum -Wunused -Wstrict-overflow=5 -Wpointer-arith -Wnormalized=nfc -Wlong-long'
set fwC = '-Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wbad-function-cast -Wc++-compat -Wjump-misses-init -Wnested-externs'
set ff = '-D_FORTIFY_SOURCE=2'
set fp = '-ftrapv -fstack-protector-all -fstrict-overflow'
set fg = -g3
set rest = "-o kenapp $argv[*] kenext.c ken.c kencom.c kenpat.c kencrc.c"
# XSI conformance & C89 undefine several functions that we use, and
# also constants like MAP_ANONYMOUS; consider how to achieve more
# specific standards conformance (?).  For XSI superset of POSIX, see
# Stevens & Rago APUE2; for conformance, see 2008 POSIX 2.2.4, p. 30.
# set fs = '-D_XOPEN_SOURCE=700 -ansi -pedantic -std=c89'
set fs = '-D_XOPEN_SOURCE_EXTENDED'


echo ''
echo 'compile with g++ (for compatibility, find need for casts)'
echo '  note that this is NOT correct way to compile for C++!'
echo ''
rm -f kenapp
# the "PRI" defines below might need to be adjusted on different
# systems if the compiler complains that it's wrong, but we're
# discarding the resulting executable in any case
g++ '-DPRId64="ld"' '-DPRId32="d"' '-DPRIu64="lu"' \
    -DUINT16_MAX=65535 -DUINT32_MAX=4294967296 \
    -DINT64_MAX=9223372036854775807 \
    $fw $ff $fp $fs $fg $rest
rm -f kenapp


echo ''
echo 'compile with optimization (to detect uninitialized vars)'
echo ''
# sometimes the specific checks that are performed depend on the
# optimization level, so let's try them all
foreach opt ( O0 O1 O2 O3 Os Ofast )
    echo '   ---- level' $opt
    # for some older versions of gcc we would use "-combine" here
    gcc $fw $ff $fp $fs $fwC -$opt -flto -fwhole-program $rest
    rm -f kenapp
    gcc $fw $ff $fp $fs $fwC -$opt                       $rest
    rm -f kenapp
end


echo ''
echo 'no optimization, for production'
echo ''
gcc $fw $ff $fp $fs $fwC $fg $rest
echo ''


# If for whatever reason the above doesn't work, then we'll try one
# last time to compile with no warnings.
if (! -e kenapp) then
    echo ''
    echo 'COMPILING WITHOUT WARNINGS!'
    echo ''
    echo '(if you get to here, you might be using an older'
    echo ' compiler version; if so, consider upgrading)'
    echo ''
    gcc $fg $rest
    echo ''
endif


echo ''
if (! -e kenapp) then
    echo 'compilation failed'
else
    echo 'compilation succeeded'
endif
echo ''

