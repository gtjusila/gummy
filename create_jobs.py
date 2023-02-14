import json
import os

if not os.path.exists('jobs'):
   os.makedirs('jobs')

if not os.path.exists('result/answer'):
   os.makedirs('result/answer')

iteration = 100
list_instances = ['3506', '3565', '3642', '3650', '3693', '3705', '3706', '3738', '3745', '3822', '3832',
                '3838', '3850', '3852', '3877', '5721', '5725', '5755', '5875', '5881', '5882', '5909', '5922']
list_solver = ['DTSS', 'MSTS', 'SA']


for idx, i in enumerate(list_solver):
    for j in range(len(list_instances)):
        d = f"""#!/bin/bash --login
#$ -cwd
#$ -N test_{i}_{list_instances[j]}
#$ -j y
#$ -o ./result/answer/test_{i}_{list_instances[j]}.out
#$ -m e
#$ -l h_rt=185000
#$ -l cputype=xeon2630v4
#$ -l mem_free=16G
#$ -M michael.adipoetra@campus.tu-berlin.de
cd result
mkdir -p log_{i}
cd log_{i}
mkdir -p QPLIB_{list_instances[j]}
cd ..
"""
        d_2 =  f"""../build/gummy ../problem_instances/qplib/instances/QPLIB_{list_instances[j]}.qs ../problem_instances/qplib/solution/QPLIB_{list_instances[j]}_answer.qs {i} | tee --append ../result/log_{i}/QPLIB_{list_instances[j]}/output.file
"""

        with open(f'./jobs/job_{i}_{list_instances[j]}.job',"w") as f:
            f.write(d)
            for k in range(iteration):
                f.write(d_2)
            f.close()

        if idx == 0 and j == 0:
            file = open('run_job.sh', 'w')
            to_write = f"""bash compile.sh \n"""
            file.write(to_write)
            to_write = f"""qsub ./jobs/job_{i}_{list_instances[j]}.job"""
            file.write(to_write)
            file.close()

        else:
            file = open('run_job.sh', 'a')
            file.write("\n")
            to_write = f"""qsub ./jobs/job_{i}_{list_instances[j]}.job"""
            file.write(to_write)
            file.close()