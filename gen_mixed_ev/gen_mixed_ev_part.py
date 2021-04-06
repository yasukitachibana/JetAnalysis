import random
import os
import numpy as np
import jetscape_files as jf
import time

def GenerateMixedEvent(id, nev, input, ecm):

  
  output_file = 'test.txt'
  if os.path.exists(output_file):
    os.remove(output_file)
  
  ptHatBins = jf.PtHatBins(ecm)
  sigmas, sigma_errs = LoadSigmaFiles(input,  ptHatBins)
  #print(sigmas)
  sigmas = Normalization(sigmas)
  sigmas = Cumulation(sigmas)
  #print(sigmas)
  
  for iev in range(nev):
    print( iev, '/', nev)
    index = DecidePtHatBin(sigmas, ptHatBins)
    pthatbin = ptHatBins[index]
    print( ' --Bin:', pthatbin[0], '-', pthatbin[1] , 'GeV')
    input_file = os.path.join( input, jf.HadronFileName(pthatbin[0],pthatbin[1]) )
    DecideEvent( input_file, output_file )

def Normalization(sigmas):
  return sigmas/sigmas.sum()

def Cumulation(sigmas):
  cumus = np.zeros(len(sigmas))
  for i in range(len(sigmas)):
    cumus[i] = sigmas[:i+1].sum()
  return cumus

def LoadSigmaFiles(input, ptHatBins):

  sigmas = np.array([])
  sigma_errs = np.array([])
  

  for pthatbin in ptHatBins:
    sigma_file = os.path.join( input, jf.SigmaFileName(pthatbin[0],pthatbin[1]) )
    if os.path.exists(sigma_file):
      sigma_data = np.loadtxt(sigma_file)
      sigmas = np.append(sigmas,sigma_data[0])
      sigma_errs = np.append(sigma_errs,sigma_data[1])
    else:
      sigmas = np.append(sigmas,0.0)
      sigma_errs = np.append(sigma_errs,0.0)
    
  return sigmas, sigma_errs


def DecidePtHatBin( sigmas, ptHatBins ):
  rand = random.random()
  diff = sigmas - rand
  i = np.where(diff > 0, diff, np.inf).argmin()
  return i


def DecideEvent( input_file, output_file ):


  f_event_set = open(input_file, 'r')
  event_set = f_event_set.read()
  f_event_set.close()
  event_set = event_set.split('#')

  n_ev = len(event_set) - 1
  i_ev = random.randint(1,n_ev)
  print( ' --Event# ', i_ev, '/', n_ev)

  f_mixed_event = open(output_file,'a')
  f_mixed_event.write('#'+event_set[i_ev])
  f_mixed_event.close

def main():
  
  start = time.time()
  
  import argparse
  parser = argparse.ArgumentParser()
    
  parser.add_argument("--id", type=int, default=-1)
  parser.add_argument("--nev", type=int, default=5)
  parser.add_argument('--input', type=str, default='../test_data')
  parser.add_argument('--ecm', type=int, default=5020)
  
  args = parser.parse_args()
  
  GenerateMixedEvent(args.id, args.nev, args.input, args.ecm)
  
  end = time.time()
  
  elapsed_time = (end - start)/3600
  print(elapsed_time, 'hours')


#    parser.add_argument("--time", type=str, default="720:00:00")
#    parser.add_argument("--mem", type=str, default="24G")
#
#    parser.add_argument("--e", type=str, default="/dev/null")
#    parser.add_argument("--o", type=str, default="/dev/null")
#
#    parser.add_argument("--root", type=str, default="ON")
#    parser.add_argument("--d", type=str, default="../build")
#
#    parser.add_argument("--n", type=str, default="jet_analysis")
#
#    parser.add_argument("--c", type=str, default="./Main")
#
#    parser.add_argument("--xml", type=str, default="")
#    parser.add_argument("--input", type=str, default="")
#    parser.add_argument("--output", type=str, default="")
#
#    args = parser.parse_args()
#
#    if args.p == "ex":
#          args.p = "express -p eamxp"
#
#    if args.xml == "" or args.input == "" or args.output == "":
#        print ('please specify xml, input directory, and out put directory')
#        print ('python gen_slurm_sub.py --xml [XMLFILENAME] --input [INPUTDIRECTORY] --output [OUTPUTDIRECTORY]')
#        exit()
#
#
#    if  args.xml == 'atlas_5tev_ffz' or args.xml == '../config/atlas_5tev_ffz' :
#        for i in range(0,3):
#            args.xml = atlas_5tev_ffz(i)
#            generate(args)
#    elif args.xml == 'atlas_5tev_ffpt' or args.xml == '../config/atlas_5tev_ffpt' :
#        for i in range(0,3):
#            args.xml = atlas_5tev_ffpt(i)
#            generate(args)
#    else:
#        generate(args)
#



if __name__ == '__main__':
  main()
