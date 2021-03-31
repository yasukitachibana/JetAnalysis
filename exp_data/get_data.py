import argparse
import urllib.request
import load_hepdata_yaml
import yaml
import os


def main():

  parser = argparse.ArgumentParser(description='test')
  parser.add_argument('-y', '--yamlFile', type=str, default='hepdata_list.yaml')
  args = parser.parse_args()

  yamlFile = args.yamlFile

  
  with open(yamlFile, 'r') as ymlf:
    yaml_data_list = yaml.safe_load(ymlf)
  
  for key in yaml_data_list.keys():
    
    print('Get Data for ', key)
    
    data =  yaml_data_list[key]
    dir = data['data_save_dir']
    url_temp = data['data_url']
    n_first = data['data_table_first']
    n_last = data['data_table_last']
    
    Mkdirs(dir)
    
    for i in range(n_first,n_last+1):
      url = url_temp.format(str(i))
      print('downloading \n>>', url)
      
      hepdata_filename = os.path.join(dir,'hepdata{}.yaml'.format(str(i)))
      urllib.request.urlretrieve(url,hepdata_filename)
      load_hepdata_yaml.convert_yaml(hepdata_filename, dir)
      os.remove(hepdata_filename)

def Mkdirs(path):
    if not os.path.isdir(path):
        print('creating "'+path+'" directory')
        os.makedirs(path)
    
#----------------------------------------------------------------------------------------------
if __name__ == '__main__':
  main()
