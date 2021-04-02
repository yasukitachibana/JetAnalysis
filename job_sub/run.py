import os
import sys
import shutil
import numpy as np
import subprocess

def Main(argc,argvs):
    cwd = os.getcwd()
    #---------------------------------------

    exe_path = argvs[1]

    
    #print('[run.py] code path: ', code_path, ' build_name: ', build_name)

    job_command = ''
    for seq in argvs[2:]:
        job_command += seq+' '
    
    #print('[run.py] command: ', job_command)
    os.chdir(exe_path)
    RunJob(job_command)
    #---------------------------------------
    os.chdir(cwd)



def RunJob(job_command):
    print('[run.py] ', job_command)
    os.system(job_command)


if __name__ == '__main__':
    argvs = sys.argv
    argc = len(argvs)
    Main(argc,argvs)

