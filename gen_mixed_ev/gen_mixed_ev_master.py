import os
import numpy as np
import time
import datetime
import slurm_sub as ss


#############################################################################################################
def JobSubmission(nev, input, nodes, ecm, tag, que, combine_only):

  cwd = os.getcwd()
  
  if not combine_only:
  
    n_per_nodes, n_last = divmod(nev, nodes-1)
    node_full_ev = nodes-1
    n_per_node_list = n_per_nodes*np.ones(nodes-1)
  
    if not n_last == 0:
      n_per_nodes, n_last = divmod(nev, nodes-2)
      node_full_ev = nodes-2
      n_per_node_list = n_per_nodes*np.ones(nodes-1)
      n_per_node_list[-1] = n_last
      print(n_last)
    

    for index in range(nodes-1):
      cmd = 'python gen_mixed_ev_part.py --id {} --nev {} --input {} --ecm {}'
      cmd = cmd.format( str(index), str(int(n_per_node_list[index])), input, str(ecm) )
      slurm_cmd = ss.GetSbatchCmd(cmd, cwd, que, tag+str(index), './log' ,'16G')
      print(slurm_cmd,'\n---')
      os.system(slurm_cmd)
    
    
  cmd = 'python combine_mixed_ev.py --id_start {} --id_end {} --input {} --tag {} --wait {}'
  cmd = cmd.format( str(0), str(int(nodes-1)), input, tag, 1 )
  slurm_cmd = ss.GetSbatchCmd(cmd, cwd, que, 'co_'+tag, './log', '16G')
  print(slurm_cmd,'\n---')
  os.system(slurm_cmd)


  
  

#############################################################################################################
## Main
#############################################################################################################
def main():

  import argparse
  parser = argparse.ArgumentParser()
    
  parser.add_argument("--nev", type=int, default=48000)
  parser.add_argument('--input', type=str, default='../test_data')
  parser.add_argument('--nodes', type=int, default=250)
  parser.add_argument('--ecm', type=int, default=5020)
  parser.add_argument('--q', type=str, default='primary')
  parser.add_argument('--tag', type=str, default='j_ana_0')
  parser.add_argument('--combine_only', type=int, default=0)
  
  args = parser.parse_args()
  
  JobSubmission(args.nev, args.input, args.nodes, args.ecm, args.tag, args.q, args.combine_only)

#############################################################################################################
#############################################################################################################
if __name__ == '__main__':
  main()
#############################################################################################################
#############################################################################################################
