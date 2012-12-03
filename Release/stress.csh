#!/bin/csh -f

# See README.txt for license.

# Run a Ken application, crashing periodically

# the "usleep" utility is not available everywhere and "sleep"
# is inconsistent across different Unix variants
if ( ! -X usleep ) then
    echo ''
    echo 'please edit this script to make it work with'
    echo 'whatever variant of "sleep" is available on'
    echo 'your system; the script was written on Red Hat'
    echo 'systems that have a "usleep" utility, but your'
    echo 'system does not have it'
    echo ''
    exit
endif

if ( ('' == $1) || ('' == $2) ) then
    echo "usage: $0 KenApp KenID"
    exit
endif

if ( ! -e $1 ) then
    echo no KenApp $1 exiting
    exit
endif

set port = `echo $2 | gawk -F: '{print $2}'`

if ( ('' == $port) || (0 >= $port) || (65535 < $port)) then
    echo bad port $port
    exit
endif

set dir = /tmp/ken_${port}_`date +%s`
mkdir $dir
cp $1 $dir
cd $dir

echo ''
echo run KenApp $1 KenID $2 on `hostname` in dir `pwd` at `date`
echo ''
killall $1
echo ''

set n = 1
while (1)
    set sleep = `date +%N | gawk '{print int($1/1000)}'`
    @ sleep += (1 + ($n % 9)) * 1000 * 1000
    # to aid debugging, we keep list of files before & after every
    # odd & even iteration
    @ oddeven = ($n % 2)
    set nz = `printf "%05d" $n`

    echo '' ; echo START $nz `date` `date +%s.%N` sleep $sleep
    echo '' ; ls -al >&! files_before_$oddeven
    echo ''

    # Pipe through cat to prevent bad interleaving of stderr
    # diagnostics from the three Unix processes that constitute
    # a Ken process.
    ./$1 $2 |& cat >& $1.out.$nz &

    echo '' ; ps -Fu `whoami`
    echo '' ; usleep $sleep
    echo '' ; kill %1
              sleep 1
    echo '' ; killall $1
              sleep 1
    echo '' ; ps -Fu `whoami`
    echo '' ; ls -al >&! files_after_$oddeven
    echo '' ; echo END $nz `date` `date +%s.%N`
    echo ''
    @ n ++
end

