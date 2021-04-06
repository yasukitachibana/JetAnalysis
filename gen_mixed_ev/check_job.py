import os
import subprocess
import time

def GheckJob(id, tag):

  n_run = NumberOfRUnningJobs(id,tag)
  while n_run > 0:
    #time.sleep(120)
    time.sleep(10)
    n_run = NumberOfRunningJobs(id,tag)
    print( n_ev, '\b-jobs are running now.')

  return 1
  
 
 
def NumberOfRunningJobs(id,tag):
  cmd = 'squeue -u {} | grep {} | wc'.format(id,tag)
  #cmd = 'ls | wc'
  out = subprocess.Popen(
      cmd, stdout=subprocess.PIPE,
      shell=True).communicate()[0]
  return int(str(out).split()[1])

def main():

  
  import argparse
  parser = argparse.ArgumentParser()
    
  parser.add_argument("--u", type=str, default='go5410')
  parser.add_argument("--tag", type=str, default='jet_analysis')
  
  args = parser.parse_args()
  
  n_run = NumberOfRunningJobs(args.u, args.tag)
  GheckJob(id, tag)
  print(n_run)


if __name__ == '__main__':
  main()
