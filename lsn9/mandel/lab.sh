#!/bin/bash


# shellcheck disable=SC1073
for ((i = 1; i < $1 + 1; i++))
  do
    echo m = $i
    ./mndl.out $i 0
  done