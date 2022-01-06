#%matplotlib inline
import matplotlib.pyplot as plt
import numpy as np
import os

plt.rc('text', usetex=True)
plt.rc('font', family='serif')
plt.rcParams['xtick.direction'] = 'in'
plt.rcParams['ytick.direction'] = 'in'
plt.rcParams["xtick.minor.visible"] = True 
plt.rcParams["ytick.minor.visible"] = True 

############################################
#Colors, Lines and Markers
colors = ['#1f77b4',
 '#ff7f0e',
 '#2ca02c',
 '#d62728',
 '#9467bd',
 '#8c564b',
 '#e377c2',
 '#7f7f7f',
 '#bcbd22',
 '#17becf']
curves = ['solid', 'dashed', 'dotted', 'dashdot','solid', 'dashed', 'dotted', 'dashdot']
markers = ['s', 'o', '^', '*','p','d']
############################################
def SetFiles(): #Edit Below!

  files = [
           '../output_examples/pp_parton_full/jetshape_hist_total_JetShape_r_jetr0.4_ptj20-300_rapj0.0-2.8_pt0.0-10000.0_rap0.0-5.0_pt.txt',
           '../output_examples/pp_hadron_full/jetshape_hist_total_JetShape_r_jetr0.4_ptj20-300_rapj0.0-2.8_pt0.0-10000.0_rap0.0-5.0_pt.txt'           
          ] # List of Input Files
  labels = [
            'Parton Jet [pp pthat:130-140 GeV]',
            'Hadron Jet [pp pthat:130-140 GeV]' 
            ] # List of Legent Labels 
  text = 'This is Test' # Text
  plot_file = 'test.pdf'

  return files, labels, text, plot_file
############################################  

############################################
# Making Plot
def JetPtR(files, labels, text, plot_name):
  fig, ax = plt.subplots(1, 1, figsize=(6, 5))

  ax.set_xlim(0.0,1)
  ax.set_ylim(0,160)


  for i in range(len(files)):
    x, y, err, xstep, ystep = GetData1Integrate(files[i],bin=1)
    ax.step(xstep, ystep, where='post', color=colors[i],linestyle=curves[i], linewidth = 3)
    ax.errorbar(x, y, err, marker="", linestyle="none", color=colors[i])
    ax.plot([],[], label= labels[i]+' '+'{:.2f}'.format(y[7])+' GeV', linestyle=curves[i], color = colors[i], linewidth = 3)


  ax.set_xlabel(r'$R$', fontsize=18, labelpad=6)
  ax.set_ylabel(r'$p^{\mathrm{jet}}_{\mathrm{T}} \quad (\mathrm{GeV})$', fontsize=18, labelpad=2)


  ax.legend(bbox_to_anchor=(1, -0.05), borderaxespad=1,handlelength=1.6, handletextpad=0.5,loc='lower right', fontsize=14,edgecolor='none', frameon=True, facecolor='none')
  ax.text(0.05, 125, text, fontsize = 15, ha = 'left')

  ax.axvline(x=0.4,c='grey',ls=':')

  plt.tight_layout()
  plt.savefig(plot_name)  


############################################
# Subroutines 

def GetData1(filename):
  data = np.loadtxt(filename, comments='#')
  x = data[:,0]
  y = data[:,3]
  err = data[:,4]
  xstep = np.append(data[:,1],data[-1,2])
  ystep = np.append(data[:,3],data[-1,3])

  return x, y, err, xstep, ystep

def GetData2(filename1,filename2):
  x1, y1, err1, xstep1, ystep1 = GetData1(filename1)
  x2, y2, err2, xstep2, ystep2 = GetData1(filename2)
  x = x1
  y = 0.5*(y1+y2)
  err = 0.5*np.sqrt(err1*err1+err2*err2)
  xstep = xstep1
  ystep = 0.5*(ystep1+ystep2)
  return x, y, err, xstep, ystep

def GetExpData(filename):
  data = np.loadtxt(filename, comments='#')
  x = data[:,0]
  y = data[:,3]
  err = data[:,4]
  xstep = np.append(data[:,1],data[-1,2])
  ystep = np.append(data[:,3],data[-1,3])
  sysp = np.append(data[:,6],data[-1,6])
  sysm = -sysp
  #print(len(data[0,:]))
  if len(data[0,:]) > 7:
    sysm = np.append(data[:,7],data[-1,7])
  return x, y, err, xstep, ystep, sysp, sysm

def GetDataRatio(x1, y1, err1, xstep1, ystep1, x2, y2, err2, xstep2, ystep2):
  x = x1
  y = y1/y2
  err = np.sqrt( (err1/y2)*(err1/y2) + (y1*err2/y2/y2)*(y1*err2/y2/y2) )
  xstep = xstep1
  ystep = ystep1/ystep2
  return x, y, err, xstep, ystep



def GetData1Integrate(filename,bin=0):
  data = np.loadtxt(filename, comments='#')
  x = data[:,0]

  dx = 1.0
  if bin == 1:
    dx = data[:,2] - data[:,1]
    
  y = data[:,3]*dx
  err = data[:,4]*dx
  err2 = err*err

  for i in range(1,len(y)):
    y[i]=y[i-1]+y[i]
    err2[i]=err2[i-1]+err2[i]

  xstep = np.append(data[:,1],data[-1,2])
  ystep = np.append(y[:],y[-1])
  err = np.sqrt(err2)

  return x, y, err, xstep, ystep

############################################
# Main Functions
def Plot():
  files, labels, text, plot_file = SetFiles()
  JetPtR(files,labels,text,plot_file)
  os.system('open '+plot_file) #Open the Plot File

def Main():
  print( '##################')
  print( 'Job Starts.')    
  Plot()
  print( 'Job Ends.')
  print( '##################')


if __name__ == '__main__':
  Main()
