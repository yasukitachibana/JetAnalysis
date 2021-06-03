import os
import yaml
import numpy as np
import argparse

def name_format(s):
  k = ''
  u = '_'
  return s.replace(' ',k).replace('(',k).replace(')',k).replace('%',k).replace('/','_over_')#.replace('.',u)

def generate_output_filename(outputDir, name):
  return os.path.join(outputDir, name+'.txt')

def generate_name(yaml_y_data):

  i = 0
  name = yaml_y_data['header']['name']
  for  yaml_qual in yaml_y_data['qualifiers']:
    for key in yaml_qual.keys():
      if i==0:
        i=1
      else:
        name += '_'
      name += str(yaml_qual[key])
  return name_format(name)

def data_output(output_filename,data):
  print('Output File -> ', output_filename)
  np.savetxt(output_filename,data)

def convert_yaml(yamlFile, outputDir):

  with open(yamlFile, 'r') as yml:
    yaml_data = yaml.safe_load(yml)

  xce, xlo, xhi = get_x_values(yaml_data['independent_variables'][0])

  for yaml_y_data in yaml_data['dependent_variables']:
    name = generate_name(yaml_y_data)
    output_filename = generate_output_filename(outputDir, name)
    yval, ystatp, ystatm, ysysp, ysysm = get_y_values(yaml_y_data)
    
    data = np.zeros((len(xce),8))
    data[:,0]=xce
    data[:,1]=xlo
    data[:,2]=xhi
    data[:,3]=yval
    data[:,4]=ystatp
    data[:,5]=ystatm
    data[:,6]=ysysp
    data[:,7]=ysysm
    data_output(output_filename,data)


def get_x_values(yaml_x_data):
  xlo  = np.array([])
  xhi  = np.array([])
  for x in yaml_x_data['values']:
    xlo = np.append(xlo,x['low'])
    xhi = np.append(xhi,x['high'])
  xce = 0.5*(xlo+xhi)
  return xce, xlo, xhi

def get_y_values(yaml_y_data):

  yval  = np.array([])
  ystatp = np.array([])
  ystatm = np.array([])
  ysysp = np.array([])
  ysysm = np.array([])

  for y in yaml_y_data['values']:

    if y['value'] == '':
      yval = np.append(yval,0.0)
      ysysp = np.append(ysysp, 0.0)
      ysysm = np.append(ysysm, 0.0)
      ystatp = np.append(ystatp, 0.0)
      ystatm = np.append(ystatm, 0.0)
      continue

    yval = np.append(yval,y['value'])
    for err in (y['errors']):
      try:
        e = str(err['symerror'])
        #Convert if percentage
        if (e[-1]=='%'):
          e = str(float(e[:-1])*y['value']*0.01)
        e = float(e)

        if err['label'] == 'sys':
          ysysp = np.append(ysysp, e)
          ysysm = np.append(ysysm, -e)
        else:
          ystatp = np.append(ystatp, e)
          ystatm = np.append(ystatm, -e)

      except KeyError:
        e = err['asymerror']

        if err['label'] == 'sys':
          ysysp = np.append(ysysp, e['plus'])
          ysysm = np.append(ysysm, e['minus'])
        else:
          ystatp = np.append(ystatp, e['plus'])
          ystatm = np.append(ystatm, e['minus'])

  return yval, ystatp, ystatm, ysysp, ysysm

def main():
  
  parser = argparse.ArgumentParser(description='test')
    
    
  parser.add_argument('-y', '--yamlFile', type=str, default='HEPData-ins1673177-v2-Table_20.yaml')
  yamlFile = parser.parse_args().yamlFile
  parser.add_argument('-o', '--outputDir', type=str, default='./')
    
  args = parser.parse_args()
    
  print('input:', args.yamlFile)
  print('output_directory:', args.outputDir)
    
  convert_yaml(args.yamlFile, args.outputDir)
    
#----------------------------------------------------------------------------------------------
if __name__ == '__main__':
  main()
