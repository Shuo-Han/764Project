#!/bin/bash
# A simple variable example
_alg=0
_numKeys=50
_ratio=5
_numThread=10
_readCount=20
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
    read=$_readCount
    write=$_writeCount
}

runMain() {
    $pwd/littleBadger_main $a $k $r $n $read $write >> output
}

# printf "Start test:\n" > output
# printf "\n-------------Examine ratio---------------:\n" >> output
# for i in 0 1
# do
#     a=$i
#     for j in {1..9}
#     do
#         r=$j
#         pConfig >> output
#         runMain
#     done
# done

reset
printf "\n-------------Examine readCount---------------:\n" >> output
for i in 0 1
do
    a=$i
    for j in 10 20 30 40 50 100
    do
        read=$j
        pConfig >> output
        runMain
    done
done

reset
printf "\n-------------Examine writeCount---------------:\n" >> output
for i in 0 1
do
    a=$i
    for j in 1 5 15 20 25
    do
        write=$j
        pConfig >> output
        runMain
    done
done

reset
printf "\n-------------Examine numThread---------------:\n" >> output
for i in 0 1
do
    a=$i
    for j in 5 10 20 50
    do
        n=$j
        pConfig >> output
        runMain
    done
done

