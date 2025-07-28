import os
import threading
#import xml.etree.ElementTree as ET
from lxml import etree

class EditXml:
  _instance = None
  _lock = threading.Lock()
  __xml = None

  def __init__(self):
    pass

  def __new__(cls):
    with cls._lock:
      if cls._instance is None:
        cls._instance = super().__new__(cls)
    return cls._instance

  def ReadXml(self, xml_filename):
    self.__xml = etree.parse(xml_filename)

  def Xml(self):
    return self.__xml


  def EditParamsArray(self,xpath,tag,val_list):

    parent = self.__xml.find(xpath)    
    if parent == None:
      self.AddElement(xpath)

    item_path = xpath+'/'+tag
    self.DeleteAllElements(item_path)

    for val in val_list:
      element = etree.SubElement(parent, tag)
      element.text = str(val)

  def EditParams(self,xpath,val):
    element = self.__xml.find(xpath)    
    if element == None:
      self.AddElement(xpath)
      element = self.__xml.find(xpath)
      if element == None:
        print('ERROR in XML')
        exit()
    element.text = str(val)

  def AddElement(self,xpath):
    path_list = xpath.split('/')
    parent = self.__xml.find('.')
    element = parent
    for i in range(1,len(path_list)):
      path = os.path.join(*path_list[:i+1])
      parent = element
      element = self.__xml.find(path)
      if element == None:
        element = etree.SubElement(parent, path_list[i])

  def DeleteElement(self,xpath):
    parent = self.__xml.find(os.path.dirname(xpath))
    element = self.__xml.find(xpath)
    if not element == None:
      parent.remove(element)

  def DeleteAllElements(self,xpath):
    parent = self.__xml.find(os.path.dirname(xpath))
    element = self.__xml.find(xpath)

    while not element == None:
      parent.remove(element)
      element = self.__xml.find(xpath)



  def PrintXml(self,filename):
    self.__xml.write(filename, pretty_print=True)