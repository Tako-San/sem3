#!/bin/bash


# shellcheck disable=SC1073
for ((i=0; i < 20; i++))
  do
    echo m = $i
    ./mandel $i
  done