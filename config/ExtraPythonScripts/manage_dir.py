import os
import glob
import shutil

def Mkdirs(path):
    if not os.path.isdir(path):
        print('creating "'+path+'" directory')
        os.makedirs(path)

def IsEmpty(path):
    files = os.listdir(path)
    files = [f for f in files if not f.startswith(".")]
    if not files:
        return True
    else:
        return False

def DeleteEmptyFiles(dir_path):
    filename = os.path.join(dir_path,'*')
    file_list = glob.glob(filename)
    for file in file_list:
        if os.stat(file).st_size==0:#the file is empty
            print( file, 'is empty')
            os.remove(file)
            print('*Delete' , file)