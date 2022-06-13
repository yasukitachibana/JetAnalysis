import random
import os
import numpy as np
import jetscape_files as jf
import check_job as cj
import time


#############################################################################################################
def CombineMixedEvent(id_start, id_end, input, hadpart):

  output_file = os.path.join(input, jf.MixedEventFileName(hadpart) )
  print('Mixed Event File:', output_file)
  f_mixed_event = open(output_file,'w')

  for index in range(id_start, id_end):
    input_file = os.path.join(input, jf.MixedEventFileName(hadpart, index) )
    f_event_set = open(input_file, 'r')
    event_set = f_event_set.read()
    f_event_set.close()
    f_mixed_event.write(event_set)
    os.remove(input_file)

  f_mixed_event.close()
#############################################################################################################
## Main
#############################################################################################################
def main():
  
  start = time.time()
  
  import argparse
  parser = argparse.ArgumentParser()
    
  parser.add_argument("--id_start", type=int, default=0)
  parser.add_argument("--id_end", type=int, default=2)
  parser.add_argument('--input', type=str, default='../test_data')
  parser.add_argument('--tag', type=str, default='j_ana_0')
  parser.add_argument('--p', type=str, default='Hadron')  
  parser.add_argument('--wait', type=int, default=1)
  
  args = parser.parse_args()
  
  
  if args.wait and cj.CheckJob('go5410', args.tag, 1):
    CombineMixedEvent(args.id_start, args.id_end, args.input, args.p)
  
  end = time.time()
  
  elapsed_time = (end - start)/3600
  print(elapsed_time, 'hours')


#############################################################################################################
#############################################################################################################
if __name__ == '__main__':
  main()
#############################################################################################################
#############################################################################################################
