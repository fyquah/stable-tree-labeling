#!/bin/bash

set -euo pipefail

BINARY=$1

for i in 1 10 100 1000 10000 50000; do
  echo -n ">>>>> ";
  echo $i;
  echo -n "  "
  valgrind $BINARY $i <100_3.txt 2>&1  | grep 'bytes allocated';
  echo -n "  "
  (time $BINARY 2>/dev/null $i <100_3.txt) 2>&1 | grep 'real'
done

