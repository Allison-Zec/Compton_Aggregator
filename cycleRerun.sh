#!/bin/bash

for i in {101..174}; do
  fname=$COMPMON_SNAILS/snail$i.list
  echo "Current snail: " $i
  while read line; do
    echo "  Current Run: " $line
    root -l -q $COMPMON_LASERCYCLES/laserCycles.C\($line\)
  done < $fname
done
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5491)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5505)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5511)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5519)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5534)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5542)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5550)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5591)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5617)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5660)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5671)'
root -l -q $COMPMON_LASERCYCLES/laserCycles.C'(5681)'
