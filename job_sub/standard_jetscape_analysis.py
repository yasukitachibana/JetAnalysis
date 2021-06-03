import argparse
import os
import sys
import pathlib
import re
import numpy as np
import yaml

def main():

    command_format = 'python gen_slurm_sub.py --xml {} --input {} --output {}'

    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("--xml", type=str, default="")
    parser.add_argument("--output", type=str, default="./TestOutPut")
    parser.add_argument("--hp", type=str, default="Hadron")
    parser.add_argument('-y', '--yamlFile', type=str, default='job_info.yaml')
    args = parser.parse_args()

    yaml_file = args.yamlFile
    hp = args.hp
    xml = args.xml
    output_dir = args.output

    with open(yaml_file, 'r') as ymlf:
        yaml_data_list = yaml.safe_load(ymlf)
  
    for key in yaml_data_list.keys():
        print('Get Data for ', key)
        data =  yaml_data_list[key]
        input_dir_base = data['DirNameBase']
        input_dir_format = data['DirName']
        hadron_parton = data['HadronParton']
        centrarities = data['Centrality']

        for cent in centrarities:
            if (not hadron_parton == hp) and (not hadron_parton == 'Both'):
                continue
            input_dir = input_dir_format
            if not cent == 'NA':
                input_dir = input_dir.format(cent)
            
            full_input = os.path.join(input_dir_base,input_dir)
            full_output = os.path.join(output_dir,hp,input_dir)
            
            command = command_format.format(xml,full_input,full_output)
            print(command)
            os.system(command)

if __name__ == '__main__':
    main()
