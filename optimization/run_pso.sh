#! /bin/bash
for i in $(seq 0 15 210)
do
    for j in $(seq 1 1 5)
    do   
         python3 bdh_optimizer_PPO.py --data=${i}
    done
done


for i in $(seq 240 15 270)
do
    for j in $(seq 1 1 5)
    do 
         python3 bdh_optimizer_PPO.py --data=${i}
    done
done
