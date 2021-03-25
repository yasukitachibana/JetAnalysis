
#ifndef SETXML_H
#define SETXML_H

#include<string>
#include<iostream>
#include <vector>
//#include<stdexcept>

#include "tinyxml2.h"

class SetXML{
public:
  static SetXML* Instance();
  
  void SetXMLFileName(std::string m_name) {
    xml_file_name = m_name;
  }
  
  std::string GetXMLFileName() {
    return xml_file_name;
  }
  
  tinyxml2::XMLElement *GetXMLRoot() {
    return xml_root;
  }
  
  tinyxml2::XMLDocument &GetXMLDocument() {
    return xml_doc;
  }
  
  void OpenXMLFile();
  void OpenXMLFile(std::string m_name);
  
  tinyxml2::XMLElement *GetXMLElement(std::initializer_list<const char *> &path);
  tinyxml2::XMLElement *GetElement(std::initializer_list<const char *> path, bool isRequired = true);
  std::string GetElementText(std::initializer_list<const char *> path, bool isRequired = true);
  int GetElementInt(std::initializer_list<const char *> path, bool isRequired = true);
  double GetElementDouble(std::initializer_list<const char *> path, bool isRequired = true);
  std::vector<int> GetElementVectorInt(std::initializer_list<const char *> path, bool isRequired = true);
  std::vector<double> GetElementVectorDouble(std::initializer_list<const char *> path, bool isRequired = true);
  std::vector<std::string> GetElementNameVector(std::initializer_list<const char *> path, bool isRequired = true);
  
private:
  SetXML(){
    std::cout << "-@-Creating SetXML"<<std::endl;
    xml_file_name = "";
    xml_file_open = false;
  }
  
  SetXML(SetXML const&){}
  
  static SetXML* m_pInstance;
  
  tinyxml2::XMLElement *xml_root;
  tinyxml2::XMLDocument xml_doc;
  
  std::string xml_file_name;
  bool xml_file_open;
  
};
// Print the XML element path name
std::ostream &operator<<(std::ostream &os,
                         std::initializer_list<const char *> path);
#endif

