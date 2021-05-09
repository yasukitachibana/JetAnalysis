import os
import numpy as np
import time
import datetime
import random
import slurm_sub as ss


#############################################################################################################
def JobSubmission(i_set, nev, input, nodes, ecm, tag, hadpart, que, combine_only):

  cwd = os.getcwd()
  
  if not combine_only:
  
    n_per_nodes, n_last = divmod(nev, nodes-1)
    #node_full_ev = nodes-1
    n_per_node_list = n_per_nodes*np.ones(nodes-1)
  
    if not n_last == 0:
      n_per_nodes, n_last = divmod(nev, nodes-2)
      #node_full_ev = nodes-2
      n_per_node_list = n_per_nodes*np.ones(nodes-1)
      n_per_node_list[-1] = n_last
      print(n_last)
    

    for index in range(nodes-1):

      cmd = 'python gen_mixed_ev_part.py --id {} --nev {} --input {} --ecm {} --p {}'
      cmd = cmd.format( str(index), str(int(n_per_node_list[index])), input, str(ecm), hadpart)

      if que == 'test':
        print(cmd,'\n---')                
        os.system(cmd)
      else:
        slurm_cmd = ss.GetSbatchCmd(cmd, cwd, que, tag+'r'+str(index), './log' ,'16G')        
        print(slurm_cmd,'\n---')        
        os.system(slurm_cmd)
    
    
  cmd = 'python combine_mixed_ev.py --id_start {} --id_end {} --input {} --tag {}  --p {} --wait {}'
  cmd = cmd.format( str(0), str(int(nodes-1)), input, tag, hadpart, 1 )

  if que == 'test':
    print(cmd,'\n---')                    
    os.system(cmd)
  else:
    slurm_cmd = ss.GetSbatchCmd(cmd, cwd, que, 'co_'+tag, './log', '16G')    
    print(slurm_cmd,'\n---')
    os.system(slurm_cmd)


  
#############################################################################################################
## Generate Serial Number
#############################################################################################################
def GetSerialNumber():
  
  sn = datetime.datetime.now()
  sn = str(sn)[9:22].replace(' ','').replace('-','').replace(':','').replace('.','')
  sn = sn+str(random.random())[3:5]

  return sn

#############################################################################################################
## Main
#############################################################################################################
def main():

  #========================================
  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument("--set", type=int, default=1)
  parser.add_argument("--nev", type=int, default=48000)
  parser.add_argument('--input', type=str, default='../test_data')
  parser.add_argument('--nodes', type=int, default=250)
  parser.add_argument('--ecm', type=int, default=5020)
  parser.add_argument('--q', type=str, default='primary')
  parser.add_argument('--tag', type=str, default='SN')
  parser.add_argument('--combine_only', type=int, default=0)
  parser.add_argument('--p', type=str, default='Hadron')
  args = parser.parse_args()

  #========================================
  if not args.p == 'Hadron' and not args.p == 'Parton':
    print('Bad option for p. Please put Hadron or Parton')
    exit()
  #========================================
  if args.tag == 'SN':
    args.tag = GetSerialNumber()
  #========================================
  nodes_per_set, rem = divmod(args.nodes,args.set)
  nodes_per_set_list = nodes_per_set*np.ones(args.set)
  nodes_per_set_list[:rem] =   nodes_per_set_list[:rem] + 1

  for i in range(args.set):
    print('####################################')
    tag = args.tag + 's' + str(i)
    print('job id: ', tag)
    nodes = int(nodes_per_set_list[i])
    print('nodes: ', nodes)
    print('====================================')
    
    JobSubmission( i, args.nev, args.input, nodes, args.ecm, tag, args.p, args.q, args.combine_only)

#############################################################################################################
#############################################################################################################
if __name__ == '__main__':
  main()
#############################################################################################################
#############################################################################################################
