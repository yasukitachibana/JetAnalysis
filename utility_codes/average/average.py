import os
import glob
import argparse
import numpy as np




def GetDataAvr(filenames, comments, diff):
  if len(filenames) == 0:
    print('Error: Taking average of 0-data table. Exit')
    exit()
  elif len(filenames) == 1:
    return GetData1(filenames[0], comments, diff)
  elif len(filenames) == 2:
    return GetData2(filenames[0],filenames[1], comments, diff)    
  else:
    print("under construction")
    return 0


####################################################################################    
# Data from Files
def GetData1(filename, comments, diff = 1):
  return np.loadtxt(filename, comments=comments)

def GetData2(filename1,filename2, comments, diff = 1):
  if diff == 1:
    if filename1 == filename2:
      print('Error: Taking average of the same data table. Exit')
      exit()
  data1 = GetData1(filename1, comments, diff)
  data2 = GetData1(filename2, comments, diff)


  if data1.ndim == 2:
    data = data1
    y1 = data1[:,3]
    err1 = data1[:,4]
    y2 = data2[:,3]
    err2 = data2[:,4]

    y, err = SumError(0.5*y1, 0.5*err1, 0.5*y2, 0.5*err2)
    data[:,3] = y
    data[:,4] = err
  elif data1.ndim == 1:
    data = data1
    y1 = data1[3]
    err1 = data1[4]
    y2 = data2[3]
    err2 = data2[4]

    y, err = SumError(0.5*y1, 0.5*err1, 0.5*y2, 0.5*err2)
    data[3] = y
    data[4] = err

  return data

####################################################################################
# Errors
def RatioError(y1, err1, y2, err2):
    return y1/y2, np.sqrt( (err1/y2)*(err1/y2) + (y1*err2/y2/y2)*(y1*err2/y2/y2) )

def ProdError(y1, err1, y2, err2):
    return y1*y2, np.sqrt( y2*y2*err1*err1 + y1*y1*err2*err2 )

def SumError(y1, err1, y2, err2, pm=1.):
    return y1 + pm*y2, np.sqrt( err1*err1 + err2*err2 )

def main():
  
  parser = argparse.ArgumentParser()

  parser.add_argument(
  "-i", 
  "--inputDirectory",
  type=str, 
  metavar="inputDirectory",
  default="Files",
  help="inputDirectory")
  
  parser.add_argument(
  "-od", 
  "--outputDirectory",
  type=str, 
  metavar="outputDirectory",
  default="Outputs",
  help="outputDirectory")

  parser.add_argument(
  "-c", 
  "--comments",
  type=str, 
  metavar="comments",
  default='#',
  help="comments")

  parser.add_argument(
  "-diff", 
  "--diff",
  type=int, 
  metavar="diff",
  default=1,
  help="diff")  

  parser.add_argument(
  "-head", 
  "--header",
  type=str, 
  metavar="header",
  default="# x_center x_low x_high value error n_jet n_event",
  help="header")  


  # Parse the arguments
  args = parser.parse_args()  

  input_directory_name = args.inputDirectory
  output_directory_name = args.outputDirectory
  
  if not os.path.exists(output_directory_name):
    print(f"Creating directory: {output_directory_name}")
    os.makedirs(output_directory_name)
  # output_filename = os.path.join(output_directory_name, args.outputFilename)


  input_subdirectory_list = glob.glob(os.path.join(input_directory_name, '*/'))
  print("Input subdirectories:" , input_subdirectory_list)

  input_filename_list = glob.glob(os.path.join(input_subdirectory_list[0], '*.txt'))
  input_filename_list = [os.path.basename(f) for f in input_filename_list]


  for input_filename in input_filename_list:

    filenames = [os.path.join(subd, input_filename) for subd in input_subdirectory_list]
    
    print('----------------------------------------------')
    print('Taking average of the files below\n - ', end='')
    print(*filenames, sep='\n - ')
    print('---------')

    output_data = GetDataAvr(filenames, args.comments, args.diff)

    # print(output_data)
    output_filename = os.path.join(output_directory_name,input_filename)
    print('Writing to file: \n   ', output_filename)

    np.savetxt(output_filename, output_data, header=args.header)

  # input_filename_list = glob.glob(os.path.join(input_directory_name, '*.txt'))
  # print("Input files:" , input_filename_list)

  # output_data = GetDataAvr(input_filename_list, args.comments, args.diff)
  
  # if not os.path.exists(output_directory_name):
  #   print(f"Creating directory: {output_directory_name}")
  #   os.makedirs(output_directory_name)
  
  # print(f"Writing to file: {output_filename}")

  

# ---------------------------------------------------------------
if __name__ == "__main__":
  print("\n-- Start average.py --")
  #parser = argparse.ArgumentParser()
  main()
  print("#####################")  
  print("-- End average.py --\n")