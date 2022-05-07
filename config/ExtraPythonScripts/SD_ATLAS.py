import os
import sys
import yaml
import numpy as np
import manage_dir as mdir
import manage_data as mdata

###########################################
main_results_dirname = 'MainResults'
yaml_filename = 'SD_ATLAS.yaml'
###########################################


def GetDirectory(argc, argvs):
  print('# your input: ', argvs)
  #print(argc)       
  ##################################
  if argc < 2:
    print('# Please Input Options')
    print('# \t$python SD_ATLAS.py')
    print('# Exit')    
    print('#########################################\n')            
    exit()
  else: 
    return argvs[-1]
  ##################################

def MoveCodeDir():
  ################################## 
  # Move to the directory of this code
  path_of_this_code = os.path.abspath(__file__)
  dir_of_this_code = os.path.dirname(path_of_this_code)
  print('#----------------------------------------') 
  print("# You are now ruuning") 
  print("#\t", path_of_this_code)
  os.chdir(dir_of_this_code)
  print("# Move to ") 
  print("#\t", os.getcwd())
  return True
  ##################################  


def MakeOutDir(filedir):
  ################################## 
  ## Make directory for output
  print('#----------------------------------------') 
  print("# Read files in")
  print("#\t", filedir)
  if not os.path.isdir(filedir):
    print('# No such directory: ')
    print("#\t", filedir)    
    return False
  main_results_dir = os.path.join(filedir,main_results_dirname)    
  mdir.Mkdirs(main_results_dir)  
  print("# Main result files are going to be stored in")
  print("#\t", main_results_dir)  
  return main_results_dir
  ##################################  


def Make2DTable(main_results_dir, target_file_name, rg_bin_finest, pt_bin_finest):

  raw_number = (len(rg_bin_finest)-1)*(len(pt_bin_finest)-1)
  data = np.zeros((raw_number,10))

  for i in range(len(pt_bin_finest)-1):
    ###############################################
    pt = 0.5*(pt_bin_finest[i] + pt_bin_finest[i+1])
    delta_pt = pt_bin_finest[i+1] - pt_bin_finest[i]
    ###############################################    
    filename = target_file_name.format(str(pt_bin_finest[i]), str(pt_bin_finest[i+1]))
    x, xl, xh, y, yerr = mdata.GetData(filename)
    delta_rg = xh - xl
    print(delta_rg)


  return True



def ReadYamlFile():
  ################################## 
  ## Set yaml File in the directory
  yamlFile = yaml_filename
  print('#----------------------------------------')
  print("# Read yaml file:")
  print("#\t", yamlFile)
  with open(yamlFile, 'r') as ymlf:
    yaml_data_list = yaml.safe_load(ymlf)

  target_file_name = yaml_data_list['target_file_name']
  rg_bin_finest = yaml_data_list['rg_bin_finest']
  rg_bin_combine = yaml_data_list['rg_bin_combine']
  pt_bin_finest = yaml_data_list['pt_bin_finest']
  pt_bin_combine = yaml_data_list['pt_bin_combine']
  return target_file_name, rg_bin_finest, rg_bin_combine, pt_bin_finest, pt_bin_combine
  ##################################  


def Main(filedir):

  MoveCodeDir()
  ###########################  
  main_results_dir = MakeOutDir(filedir)  
  if not main_results_dir: 
    return False
  ###########################    
  target_file_name, rg_bin_finest, rg_bin_combine, pt_bin_finest, pt_bin_combine = ReadYamlFile()
  ###########################
  if not Make2DTable(main_results_dir, os.path.join(filedir, target_file_name), rg_bin_finest, pt_bin_finest):
    return False

  return True

if __name__ == '__main__':

  initial_path = os.getcwd()
  print('\n#########################################')    
  print('# Python code: SD_ATLAS.py') 
  print('# Your current directory:', initial_path)      
  print('#----------------------------------------')    
  argvs = sys.argv
  argc = len(argvs)
  filedir = GetDirectory(argc, argvs, )
  main_job = Main(filedir)
  if main_job:
    print('# Job Success')
  else:
    print('# Job Failure')
  print('#----------------------------------------')      
  os.chdir(initial_path)
  print('# You came back to')
  print('#\t', initial_path)
  print('#########################################\n')      