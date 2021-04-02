import os
import sys
import pathlib
import re
import numpy as np



def submission(args):

    print("\n---")
    cwd = os.getcwd()
    
    head = 'sbatch -q {} --time {} --mem {} --job-name {} -o {} -e {}'
    head = head.format(args.p, args.time, args.mem, args.n, args.o, args.e)
    print( "#slurm sub: ", head)

    master_command = os.path.join(cwd,'JobMaster') + ' "' + cwd + '" "python run.py {}'
    master_command = master_command.format( os.path.join(cwd, args.d) )
    print( "-\n#master command: ", master_command)
    
    root = 'module load root/6.14.06; .  /wsu/el7/gnu7/root/6.14.06/bin/thisroot.sh'
    command = '{} {}'.format(args.c, args.args)
    
    if args.root == "ON":
        command = root + '; ' + command
    print( "-\n#command: ", command)

    main_command = master_command + ' ' + command + '"'
    command = head + ' ' + main_command

    print("-\n#Submission: ",command)
    os.system(command)
    #os.system(main_command)
    print("---\n")
    
def main():

    import argparse

    parser = argparse.ArgumentParser()
    
    parser.add_argument("--p", type=str, default="primary")
    
    parser.add_argument("--time", type=str, default="720:00:00")
    parser.add_argument("--mem", type=str, default="24G")
    
    parser.add_argument("--e", type=str, default="/dev/null")
    parser.add_argument("--o", type=str, default="/dev/null")

    parser.add_argument("--root", type=str, default="OFF")
    parser.add_argument("--d", type=str, default="../build")

    parser.add_argument("--n", type=str, default="jet_analysis")
    parser.add_argument("--c", type=str, default="")
    parser.add_argument("--args", type=str, default="")
    
    args = parser.parse_args()
    if args.c == "":
        print('Please pass command' )
        exit()


    submission(args)




if __name__ == '__main__':
    main()
