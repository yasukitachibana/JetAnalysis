import numpy as np

def GetData(filename):

    data = np.loadtxt(filename, comments='#')
    x = data[:,0]
    xl = data[:,1]
    xh = data[:,2]    
    y = data[:,3]
    yerr = data[:,4]

    return x, xl, xh, y, yerr