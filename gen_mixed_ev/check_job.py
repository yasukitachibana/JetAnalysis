import os
import subprocess
import time

def CheckJob(id, tag, n_trigg):

  n_run = NumberOfRunningJobs(id,tag)
  print( n_run, '\b-jobs are running now.')
  while n_run > n_trigg:
    #time.sleep(120)
    time.sleep(10)
    n_run_now = NumberOfRunningJobs(id,tag)
    if n_run_now != n_run:
      n_run = n_run_now
      print( n_run, '\b-jobs are running now.')

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
  parser.add_argument("--trigg", type=int, default=1)  
  
  args = parser.parse_args()
  
#  n_run = NumberOfRunningJobs(args.u, args.tag)
#    print(n_run)
  CheckJob(args.u, args.tag, args.trigg)



if __name__ == '__main__':
  main()
