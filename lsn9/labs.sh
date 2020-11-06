#!/bin/bash



for ((var=1; var<=20; var++))
  do
    echo m = $var
    time ./a.out 100000 $var 0
  done

