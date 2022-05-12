import numpy as np


def GetData(filename):
    data = np.loadtxt(filename, comments='#')
    return GetVals(data)


def GetVals(data):
    x = data[:, 0]
    xl = data[:, 1]
    xh = data[:, 2]
    y = data[:, 3]
    yerr = data[:, 4]
    return x, xl, xh, y, yerr

def SetVals(x, xl, xh, y, yerr, extra_val1 = 0, extra_val2 = 0 ):
    data_1d = np.zeros( (len(x),7) )
    data_1d[:,0] = x
    data_1d[:,1] = xl
    data_1d[:,2] = xh
    data_1d[:,3] = y
    data_1d[:,4] = yerr
    data_1d[:,5] = extra_val1
    data_1d[:,6] = extra_val2
    return data_1d


def Combine(data_list, bin_list=[], bin_width=True, bin_divide=True):

    if bin_list==[] or bin_width==False:
      print('# Combine data list with uniform binning')             
      bin_list = np.ones(len(data_list))

    # Set First File
    full_bin_width = bin_list[0]
    combined_data = data_list[0]
    combined_data[:,3] *= bin_list[0]
    combined_data[:,4] *= bin_list[0]    
    combined_data[:,5] *= bin_list[0]        
    combined_data[:,6] *= bin_list[0]            
    combined_data[:,4] *= combined_data[:,4]# error: sum of squares

    # Set Second and Later Files
    for i_data_m_1, data in enumerate(data_list[1:]):
      i_data = i_data_m_1+1
      #bin width
      this_bin = bin_list[i_data]
      #print(this_bin)
      full_bin_width += this_bin
      combined_data[:,3] += data[:,3]*this_bin
      combined_data[:,4] += data[:,4]*data[:,4]*this_bin*this_bin # error: sum of squares
      combined_data[:,5] += data[:,5]*this_bin      
      combined_data[:,6] += data[:,6]*this_bin            
    
    combined_data[:,4] = np.sqrt(combined_data[:,4]) # error: square root of square sum
    #

    if bin_divide:
        combined_data[:,3] /= full_bin_width
        combined_data[:,4] /= full_bin_width
        combined_data[:,5] /= full_bin_width
        combined_data[:,6] /= full_bin_width
    else:
        print('no bin divide')
    return combined_data

def Reduc2to1D(data,ix):

    if (not ix ==0 ) and (not ix == 1):
        print( 'Bad second argument in Reduc2to1D in manage_data.py' )
        exit()

    x = data[:,0+3*ix]
    xl = data[:,1+3*ix]
    xh = data[:,2+3*ix]
    y = data[:,6]
    yerr = data[:,7]
    extra_val1 = data[:,8]
    extra_val2 = data[:,9]
    return SetVals(x, xl, xh, y, yerr, extra_val1, extra_val2 )