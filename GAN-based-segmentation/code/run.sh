#! /bin/bash

#mkdir test/sim_w=1_300e
#mv checkpoint/tool_256/sim_w=1/checkpoint checkpoint/tool_256
#mv checkpoint/tool_256/sim_w=1/cyclegan* checkpoint/tool_256
#CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --epoch=100 --save_freq=40
#CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --phase=test --which_direction=AtoB
#CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --phase=test --which_direction=BtoA
#mv test/AtoB* test/sim_w=1_300e
#mv test/BtoA* test/sim_w=1_300e
#mv checkpoint/tool_256/checkpoint checkpoint/tool_256/sim_w=1
#mv checkpoint/tool_256/cyclegan* checkpoint/tool_256/sim_w=1
#mv logs/events* sim_w=1

#mkdir test/sim_w=5_300e
#mv checkpoint/tool_256/sim_w=5/checkpoint checkpoint/tool_256
#mv checkpoint/tool_256/sim_w=5/cyclegan* checkpoint/tool_256
#CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --epoch=300 --save_freq=40
#CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --phase=test --which_direction=AtoB
#CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --phase=test --which_direction=BtoA
#mv test/AtoB* test/sim_w=5_300e
#mv test/BtoA* test/sim_w=5_300e
#mv checkpoint/tool_256/checkpoint checkpoint/tool_256/sim_w=5
#mv checkpoint/tool_256/cyclegan* checkpoint/tool_256/sim_w=5
#mv logs/events* sim_w=5
for L1 in $(seq 20 10 50)
do
	for j in $(seq 1 1 30)
	do
	    mkdir test/sim_w=${L1}_${j}e
	    mkdir checkpoint/tool_256/sim_w=${L1}_${j}e
	    mkdir logs/sim_w=${L1}_${j}e
	    CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --epoch=1 --save_freq=20
	    CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --phase=test --which_direction=AtoB
	    CUDA_VISIBLE_DEVICES=0 python3 main.py --dataset_dir=tool --continue_train=1 --phase=test --which_direction=BtoA
	    mv test/AtoB* test/sim_w=${L1}_${j}e
	    mv test/BtoA* test/sim_w=${L1}_${j}e
	    cp checkpoint/tool_256/cyclegan* checkpoint/tool_256/sim_w=${L1}_${j}e
	    cp checkpoint/tool_256/checkpoint checkpoint/tool_256/sim_w=${L1}_${j}e
	    mv logs/events* logs/sim_w=${L1}_${j}e
	    sleep 1
	done
done


#		mkdir layer$i/fold$j
#		python main.py ../../2.tfrecord_construction_nfold/db5/fold${j}_train.tfrecords 1000 29 $i
#		mv lgu* ./layer$i/fold$j
#		mv log.txt ./layer$i/fold$j
#		mv checkpoint ./layer$i/fold$j




