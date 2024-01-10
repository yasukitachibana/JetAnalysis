import os
import sys
import pathlib
import re
import numpy as np
import yaml
import xml.etree.ElementTree as ET
import edit_xml as exml
import gen_slurm_sub as gslurmsub




########################################
# Default Parameters to be set by common.yaml
common_yaml_file = os.path.join('BatchSubmissionConfig','common.yaml')
#default_sigma_files_path = ''
default_batch_xml_par_dir = ''

def SetDefaultPram(default_param_file):
    
    with open(default_param_file, 'r') as ymlf:
        data = yaml.safe_load(ymlf)

        # global default_sigma_files_path 
        # default_sigma_files_path = data['default_sigma_files_path']

        global default_batch_xml_par_dir
        default_batch_xml_par_dir = data['default_batch_xml_par_dir']

def GetArgs():
    import argparse

    parser = argparse.ArgumentParser()
    
    
    parser.add_argument("--p", type=str, default="primary")
    parser.add_argument("--time", type=str, default="720:00:00")
    parser.add_argument("--mem", type=str, default="24G")
    
    parser.add_argument("--root", type=str, default="ON")
    parser.add_argument("--d", type=str, default="../build")

    parser.add_argument("--n", type=str, default="")

    parser.add_argument("--c", type=str, default="./Main")

    parser.add_argument("--xml_dir", type=str, default="")


    parser.add_argument("--xml", type=str, default="")
    parser.add_argument("--input", type=str, default="")
    parser.add_argument("--output", type=str, default="")
    parser.add_argument("--yaml", type=str, default="")
    parser.add_argument("--xml_list", type=str, default="off")
    parser.add_argument("--merge", type=str, default="no")
    
    args = parser.parse_args()

    return args


def GenerateBatchXML(batch_xml_dir,pthat_low,pthat_high):

    # Generate XML file path
    batch_xml_path = GenerateBatchXMLFilename(pthat_low,pthat_high,batch_xml_dir)

    # Set Xml Contents
    ex = exml.EditXml()   
    SetBatchXML(pthat_low,pthat_high)    

    # Printout (Generate) XML
    ex.PrintXml(batch_xml_path)
    return batch_xml_path

def SetBatchXML(pthat_low,pthat_high):
    ex = exml.EditXml()    

    # set mode
    ex.EditParams('./mode',1)
    # set ptHat
    ex.EditParamsArray('./ptHat','Item',[pthat_low,pthat_high])
    

def GenerateBatchXMLFilename(pthat_low,pthat_high,filedir='',head=''):
    name = 'pthat'+str(pthat_low)+'-'+str(pthat_high)
    return GenerateXMLFilename(name,filedir,head)

def GenerateXMLFilename(name,filedir='',head=''):
    if not head == '':
        head = head + '_'
    filename = head+name+'.xml'
    if filedir == '':
        return filename
    else: 
        return os.path.join(filedir,filename)


def SetMergeXML(pthat_bin_edges):
    ex = exml.EditXml()    

    # set mode
    ex.EditParams('./mode',2)

    # set ptHat
    ex.EditParamsArray('./ptHat','Item',pthat_bin_edges)


def SetXMLBase(ascii_one_line,sigma_files_path):
    ex = exml.EditXml()
    if ascii_one_line:
        ex.EditParams('./inputFiles/inputStyle','JetScapeAsciiOneLine')    
    else: 
        ex.EditParams('./inputFiles/inputStyle','JetScapeAscii')    
    # set sigma file path
    ex.EditParams('./inputFiles/sigma/path',sigma_files_path)    



def GenerateMergeXML(batch_xml_dir,pthat_bin_edges,file_generate):
    ex = exml.EditXml()    
    merge_xml_path = GenerateXMLFilename('merge',batch_xml_dir)
    if file_generate == 'yes':
        SetMergeXML(pthat_bin_edges)
        ex.PrintXml(merge_xml_path)
    return merge_xml_path

def GetXmlList(args):
    xml_list = []
    xml_list_file = args.xml #This is Yaml File
    # Open Yaml
    with open(xml_list_file, 'r') as ymlf:
        data = yaml.safe_load(ymlf)

    xml_list = data['xml_list']
    xml_common_path = data['common_path']

    return xml_list, xml_common_path

def GetOutputPathList(args,xml_list):
    output_path_base = args.output
    return [output_path_base.format(xml_name) for xml_name in xml_list]


def MainListSubmission(args):
    
    if args.xml_list == 'off':
        xml_list = [args.xml]
        output_path_list = [args.output]
    elif args.xml_list == 'on':
        xml_name_list, xml_common_path = GetXmlList(args)
        output_path_list = GetOutputPathList(args,xml_name_list)
        xml_list = [os.path.join(xml_common_path,xml_name+'.xml') for xml_name in xml_list]
    else:
        print('ERROR: xml_list must be on or off')
        exit()

    for i_xml, xml_file in enumerate(xml_list):
        output_path = output_path_list[i_xml]
        print(xml_file)
        print(output_path)        
        MainSubmission(args,xml_file,output_path)

def MainSubmission(args,xml_file,output_path):
    # print('Yes!')
    # exit()

    command_format = 'python gen_slurm_sub.py --xml {} --input {} --output {} --p "{}" --time {} --mem {} --n {} --o {} --e {} --root {} '
    ##########################################################################
    #'../config/PromptPhoton/CMS/prompt_photon_cent0_10_60_10000.xml'
    input_path = args.input
    #output_path = args.output
        
    yaml_file = args.yaml
    #os.path.join('BatchSubmissionConfig','jetscape_photon_high_stat.yaml')
    ##########################################################################    
    
    # Create Folder for XMLS
    batch_xml_par_dir = default_batch_xml_par_dir
    if not args.xml_dir == '':
        batch_xml_par_dir = args.xml_dir
    xml_basename = os.path.basename(xml_file)
    batch_xml_dau_dir = os.path.splitext(xml_basename)[0]+'_'+os.path.basename(input_path)
    batch_xml_dir = os.path.join(batch_xml_par_dir,batch_xml_dau_dir)
    gslurmsub.Mkdirs(batch_xml_dir)

    # Set Job Name
    name_head = batch_xml_dau_dir
    if not args.n == '':
        name_head = args.n


    # Read XML
    ex = exml.EditXml()
    ex.ReadXml(xml_file)


    # Open Yaml
    with open(yaml_file, 'r') as ymlf:
        data = yaml.safe_load(ymlf)
  
    
    pthat_bin_edges = data['pthat_bin_edges']
    sigma_files_path = data['sigma_files_path']
    ascii_one_line = data['ascii_one_line']

    SetXMLBase(ascii_one_line,sigma_files_path)


    if args.merge == 'no':
        for i_pthat in range(len(pthat_bin_edges)-1):
            pthat_low = pthat_bin_edges[i_pthat]
            pthat_high = pthat_bin_edges[i_pthat+1]

            # Generate XML for this bin
            batch_xml_path = GenerateBatchXML(batch_xml_dir,pthat_low,pthat_high)
            out = os.path.splitext(batch_xml_path)[0]+'_out.txt'
            error = os.path.splitext(batch_xml_path)[0]+'_error.txt'
            name = name_head + '_' + os.path.splitext(os.path.basename(batch_xml_path))[0]
            command = command_format.format(batch_xml_path, input_path, output_path, args.p, args.time, args.mem, name, out, error, args.root)
            print('NAME: ',name)  
            print('COMMAND: ',command)    
            os.system(command)

        merge_xml_path = GenerateMergeXML(batch_xml_dir,pthat_bin_edges,args.merge)
    elif args.merge == 'yes':
        merge_xml_path = GenerateMergeXML(batch_xml_dir,pthat_bin_edges,args.merge)
        out = os.path.splitext(merge_xml_path)[0]+'_out.txt'
        error = os.path.splitext(merge_xml_path)[0]+'_error.txt'
        name = name_head + '_' + os.path.splitext(os.path.basename(merge_xml_path))[0]
        command = command_format.format(merge_xml_path, input_path, output_path, args.p, args.time, args.mem, name, out, error, args.root)
        print('NAME: ',name)  
        print('COMMAND: ',command)    
        os.system(command) 

    else:
        print('ERROR: merge must be yes or no')
        exit()




def main():
    
    SetDefaultPram(common_yaml_file)
    args = GetArgs()    
    MainListSubmission(args)



if __name__ == '__main__':
    main()
