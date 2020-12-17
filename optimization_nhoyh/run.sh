#! /bin/bash

for i in $(seq 0 30 60)
do
    for j in $(seq 0 1 5)
    do    
        python scipy_optimizer.py --method=CG --data=${i}    
        python scipy_optimizer.py --method=BFGS --data=${i}
    done
done

