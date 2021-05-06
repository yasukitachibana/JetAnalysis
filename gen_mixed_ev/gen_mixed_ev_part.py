import random
import os
import numpy as np
import jetscape_files as jf
import time

def GenerateMixedEvent(id, nev, input, ecm, hadpart):
  
  ptHatBins = jf.PtHatBins(ecm)
  #  sigmas, sigma_errs = LoadSigmaFiles(input,  ptHatBins)
  sigmas = LoadSigmaFiles(input,  ptHatBins)
  #print(sigmas)
  sigmas = Normalization(sigmas)
  sigmas = Cumulation(sigmas)
  
  print(sigmas)
  
  
  output_file = os.path.join(input, jf.MixedEventFileName(hadpart, id) )
  f_mixed_event = open(output_file,'w')
  
  for iev in range(nev):
    print( iev, '/', nev)
    index = DecidePtHatBin(sigmas, ptHatBins)
    pthatbin = ptHatBins[index]
    print( ' --Bin:', pthatbin[0], '-', pthatbin[1] , 'GeV')
    input_file = os.path.join( input, jf.HadronFileName(pthatbin[0],pthatbin[1]) )
    f_mixed_event.write( '#' + str(iev) + DecideEvent( input_file ) )
    
  f_mixed_event.close()


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
    sigma_file = os.path.join( jf.PpDir(), jf.SigmaFileName( pthatbin[0],pthatbin[1]) )
    #sigma_file = os.path.join( input, jf.SigmaFileName(pthatbin[0],pthatbin[1]) )
    if os.path.exists(sigma_file):
      sigma_data = np.loadtxt(sigma_file)
      sigmas = np.append(sigmas,sigma_data[0])
      sigma_errs = np.append(sigma_errs,sigma_data[1])
    else:
      sigmas = np.append(sigmas,0.0)
      sigma_errs = np.append(sigma_errs,0.0)
    
  return sigmas#, sigma_errs


def DecidePtHatBin( sigmas, ptHatBins ):
  rand = random.random()
  print(rand)
  diff = sigmas - rand
  i = np.where(diff > 0, diff, np.inf).argmin()
  return int(i)


def DecideEvent( input_file ):


  f_event_set = open(input_file, 'r')
  event_set = f_event_set.read()
  f_event_set.close()
  event_set = event_set.split('#')

  n_ev = len(event_set) - 1
  i_ev = random.randint(1,n_ev)
  print( ' --Event# ', i_ev, '/', n_ev)

  return event_set[i_ev]

def main():
  
  start = time.time()
  
  import argparse
  parser = argparse.ArgumentParser()
    
  parser.add_argument("--id", type=int, default=-1)
  parser.add_argument("--nev", type=int, default=5)
  parser.add_argument('--input', type=str, default='../test_data')
  parser.add_argument('--ecm', type=int, default=5020)
  parser.add_argument('--p', type=str, default='Hadron')
  
  args = parser.parse_args()
  
  GenerateMixedEvent(args.id, args.nev, args.input, args.ecm, args.p)
  
  end = time.time()
  
  elapsed_time = (end - start)/3600
  print(elapsed_time, 'hours')


if __name__ == '__main__':
  main()
