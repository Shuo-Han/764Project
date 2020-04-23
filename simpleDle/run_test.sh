#!/bin/bash
# A simple variable example
_alg=0
_numKeys=50
_ratio=5
_numThread=10
_readCount=5
_writeCount=1

a=$_alg
k=$_numKeys
r=$_ratio
n=$_numThread
read=$_readCount
write=$_writeCount

pwd=/home/shuohan/Documents/gitrepo/CS764TopicsInDatabaseManagementSystems/simpleDle

pConfig() {
    printf "Algo $a, numKeys $k, ratio $r, "
    printf "numThread $n, readCount $read, writeCount: $write\n"
}

reset() {
    a=$_alg
    k=$_numKeys
    r=$_ratio
    n=$_numThread
    readt=$_readCount
    write=$_writeCount
}

runMain() {
    $pwd/littleBadger_main $a $k $r $n $read $write 1>> output
}

# printf "Start test:\n" > output
# printf "\nExamine ratio:\n" >> output
for i in 1
do
    a=$i
    for j in {0..10}
    do
        r=$j
        pConfig >> output
        runMain
    done
done


