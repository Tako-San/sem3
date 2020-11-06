#!/bin/bash



for ((var=1; var<=20; var++))
  do
    echo m = $var
    time ./a.out 1000000000 $var 4
  done

