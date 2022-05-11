import os
import sys
import yaml
import numpy as np
import manage_dir as mdir
import manage_data as mdata

###########################################
main_results_tail = 'MainResults'
yaml_filename = 'SD_ATLAS.yaml'
pt_rg_2d_filename = 'atlas_pt_rg.txt'
rg_1d_filename = 'atlas_rg_pt{}-{}.txt'
pt_1d_filename = 'atlas_pt_rg0p{}-0p{}.txt'
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
  main_results_dir = filedir+main_results_tail
  mdir.Mkdirs(main_results_dir)  
  print("# Main result files are going to be stored in")
  print("#\t", main_results_dir)  
  return main_results_dir
  ##################################  


def Make2DTable(main_results_dir, target_file_name, rg_bin_finest, pt_bin_finest):
  print('#----------------------------------------')
  print('# Create 2D data (function of rg and pt)')
  n_rg_bin = len(rg_bin_finest)-1
  n_pt_bin = len(pt_bin_finest)-1
  raw_number = (n_rg_bin)*(n_pt_bin)
  data = np.zeros((raw_number,10))

  for i in range(n_pt_bin):
    ###############################################
    # pt info
    ptl = pt_bin_finest[i]
    pth = pt_bin_finest[i+1]    
    pt = 0.5*(ptl + pth)
    delta_pt = pth - ptl
    ###############################################
    # rg info, spectra info
    filename = target_file_name.format(str(int(ptl)), str(int(pth)))
    #filename = '/Users/yasukitachibana/GoogleDrive/Downloads/PP_TEST/count_hist_total_SoftDropGroom_rG_jetr0.2_ptj60-80_rapj0.0-0.5_pt0.1-100.0_rap0.0-0.9_beta0.00_zCut0.20.txt'
    rg, rgl, rgh, val, err = mdata.GetData(filename)
    delta_rg = rgh - rgl
    # milibarn(-3) to nanobarn(-9)
    val = val*1000000
    err = err*1000000    
    # spectra info
    val = val/delta_pt/delta_rg
    err = err/delta_pt/delta_rg
    ###############################################   
    # set 2d array
    r_slice = slice(i*n_rg_bin,(i+1)*n_rg_bin)
    data[r_slice,0]=pt
    data[r_slice,1]=ptl
    data[r_slice,2]=pth
    data[r_slice,3]=rg
    data[r_slice,4]=rgl
    data[r_slice,5]=rgh
    data[r_slice,6]=val
    data[r_slice,7]=err
    ###############################################   
  output_filename = os.path.join(main_results_dir,pt_rg_2d_filename)
  np.savetxt(output_filename,data)
  print('# 2D data (function of rg and pt) is saved in')  
  print('#\t', output_filename)    
  return data

def  Make1DTableRg(main_results_dir, pt_rg_2d_data, rg_bin_finest, pt_bin_finest, pt_bin_combine):
  print('#----------------------------------------')
  print('# Create 1D data (function of rg)')
  n_rg_bin = len(rg_bin_finest)-1

  for k in pt_bin_combine:
    ptl=pt_bin_finest[k[0]]
    pth=pt_bin_finest[k[1]]
    data_list = []
    bin_list = []
    print('# Combine pt bins', end='')    
    for l in range(k[0],k[1]):
      pt_slice = slice(l*n_rg_bin,(l+1)*n_rg_bin)
      data_2D = pt_rg_2d_data[pt_slice,:]
      print(' [{}-{}] '.format(int(data_2D[0,1]),int(data_2D[0,2])), end='')            
      data_1D = mdata.Reduc2to1D(data_2D,1)
      ptbin = data_2D[:,2] - data_2D[:,1]
      #print(ptbin)
      data_list.append(data_1D)
      bin_list.append(ptbin)

    print('')
    data = mdata.Combine(data_list,bin_list,True)
    output_filename = rg_1d_filename.format(str(int(ptl)),str(int(pth)))
    output_filename = os.path.join(main_results_dir,output_filename)  
    np.savetxt(output_filename,data)
    print('# 1D data (function of rg) is saved in')  
    print('#\t', output_filename)    

      



def  Make1DTablePt(main_results_dir, pt_rg_2d_data, rg_bin_finest, pt_bin_finest, r_bin_combine):
  print('#----------------------------------------')
  print('# Create 1D data (function of rg)')

  n_rg_bin = len(rg_bin_finest)-1
  n_pt_bin = len(pt_bin_finest)-1
  raw_number = (n_rg_bin)*(n_pt_bin)

  for k in r_bin_combine:
    rgl=rg_bin_finest[k[0]]
    rgh=rg_bin_finest[k[1]]
    data_list = []
    bin_list = []
    print('# Combine r bins', end='')       
    for l in range(k[0],k[1]):
      print(l)
      exit()
      #r_slice = slice( l, raw_number, n_pt_bin )
      # data_2D = pt_rg_2d_data[r_slice,:]
      # print(' [{:.2f}-{:.2f}] '.format(data_2D[0,4],data_2D[0,5]), end='')      
      # data_1D = mdata.Reduc2to1D(data_2D,0)
      # rbin = data_2D[:,5] - data_2D[:,4]
      # print(data_2D,len(rbin))
      # #print(ptbin)
      # data_list.append(data_1D)
      # bin_list.append(rbin)      


    print('')
    data = mdata.Combine(data_list,bin_list,True)
    output_filename = pt_1d_filename.format(str(int(10000*rgl)).zfill(4),str(int(10000*rgh)).zfill(4))
    
    print(output_filename)
    output_filename = os.path.join(main_results_dir,output_filename)  
    np.savetxt(output_filename,data)
    print('# 1D data (function of pt) is saved in')  
    print('#\t', output_filename)    

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
  pt_rg_2d_data = Make2DTable(main_results_dir, os.path.join(filedir, target_file_name), rg_bin_finest, pt_bin_finest)
  ###########################
  Make1DTableRg(main_results_dir, pt_rg_2d_data, rg_bin_finest, pt_bin_finest, pt_bin_combine)
  Make1DTablePt(main_results_dir, pt_rg_2d_data, rg_bin_finest, pt_bin_finest, rg_bin_combine)  
  ###########################  



  return True

if __name__ == '__main__':

  initial_path = os.getcwd()
  print('\n#########################################')    
  print('# Python code: SD_ATLAS.py') 
  print('# Your current directory:', initial_path)      
  print('#----------------------------------------')    
  argvs = sys.argv
  argc = len(argvs)
  filedir = GetDirectory(argc, argvs )
  main_job = Main(filedir)
  if main_job:
    print('#----------------------------------------\n#')    
    print('# 👍 Job Success 👍\n#')
  else:
    print('#----------------------------------------\n#')    
    print('# ⚠️  Job Failure ⚠️\n#')
  print('#----------------------------------------\n#')      
  os.chdir(initial_path)
  print('# You came back to')
  print('#\t', initial_path)
  print('#########################################\n')      