#!/bin/bash

usage() { echo "Usage: $0 [-n <snail number>] [-h, print help info]" 1>&2; exit 1; }

while getopts ":h:n:" opt; do
  case ${opt} in
    h )
      usage
      ;;
    n )
      snail_num=$OPTARG
      ;;
  esac
done

if [ -z "${snail_num}" ]; then
  usage
fi

if [ ! -d $COMPMON_WEB/snails/snail$snail_num ]; then
  mkdir $COMPMON_WEB/snails/snail$snail_num
fi

run_code=2
if [ "$snail_num" -lt 100 ]; then
  run_code=1
fi

#root -l -b -q miniruns.C\(\"snail${snail_num}\"\)
#root -l -b -q laserCycles.C\(${snail_num}\)
#root -l -b -q aggregate.C\(\"snail${snail_num}\"\)
#root -l -b -q aggregate.C\(\"snail${snail_num}\",4\)
#root -l -b -q aggregate.C\(\"snail${snail_num}\",0,1\)
#root -l -b -q aggregate.C\(\"snail${snail_num}\",4,1\)

root -l -b -q $COMPMON_GRAND/buildGrandRootfile.C\($run_code\)
root -l -b -q aggregateGrand.C\(${snail_num}\)
root -l -b -q $COMPMON_GRAND/grandMacros/snailwisePlots.C\($run_code\)
root -l -b -q $COMPMON_GRAND/grandMacros/runwisePlots.C\($run_code\)

date=`date +"%Y-%m-%d"`
time=`date +"%T"`
python $COMPMON_DIR/write_html.py $date $time index.html
