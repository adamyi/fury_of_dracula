#!/bin/bash
for i in `seq 1 100`;
do
    echo $i
    2521 dryrun assign2hunt Makefile *.c *.h | tee $i | grep disqua
done
