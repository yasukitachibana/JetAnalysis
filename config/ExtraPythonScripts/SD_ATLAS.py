import os
import sys
import manage_dir as mdir

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
  path_of_this_code = __file__
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

def Main(filedir):

  MoveCodeDir()
  main_results_dir = MakeOutDir(filedir)
  if not main_results_dir: 
    return False

  ################################## 
  ## Set yaml File in the directory
  yamlFile = yaml_filename
  print('#----------------------------------------') 
  # main_sub.Init(params)
  # base = Base('check and run')
  # function = base.GetFunction()
  # n_run_total = main_sub.Sequence(params, function)
  # print( 'Submission Ends.')
  # print( 'Total: ', str(n_run_total)+'-jobs were submitted.')  
  # print( '##################')

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