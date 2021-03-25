#include "SetXML.h"
//#include "JetScapeLogger.h"
//#include <stdlib.h>


SetXML* SetXML::m_pInstance = NULL;

SetXML* SetXML::Instance() {
  if (!m_pInstance)
  {
    std::cout<<"[SetXML] Created SetXML Instance"<<std::endl;
    m_pInstance = new SetXML();
  }
  return m_pInstance;
}

void SetXML::OpenXMLFile() {
  
  if (!xml_file_open) {
    xml_doc.LoadFile((char *)GetXMLFileName().c_str());
    std::cout << "[SetXML] Trying XML file : " << GetXMLFileName()<<std::endl;
    if (xml_doc.ErrorID() < 1) {
      std::cout << "[SetXML] Open XML file : " << GetXMLFileName()<<std::endl;
      xml_root = (tinyxml2::XMLElement *)xml_doc.FirstChildElement("jetAnalysis");
      if (!xml_root) {
        std::cout << std::endl;
        std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
        std::cout << "[SetXML] Not a valid XML file!" << std::endl;
        std::cout << "[SetXML] Exit."<< std::endl;
        std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
        std::cout << std::endl;
        exit(-1);
      }
    } else {
      std::cout << std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << "[SetXML] XML file not found/not properly opened! Error code : " << xml_doc.ErrorID()<< std::endl;
      std::cout << "[SetXML] Exit."<< std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << std::endl;
      exit(-1);
    }
    xml_file_open = true;
  }
  
}

void SetXML::OpenXMLFile(std::string m_name) {
  SetXMLFileName(m_name);
  OpenXMLFile();
}

tinyxml2::XMLElement *
SetXML::GetXMLElement(std::initializer_list<const char *> &path) {
  
  //std::cout << "[SetXML] Looking for element: " << path << std::endl;
  OpenXMLFile();
  
  tinyxml2::XMLElement *currentElement = nullptr;
  for (auto &elementName : path) {
    if (!currentElement) {
      currentElement = xml_root->FirstChildElement(elementName);
      if (currentElement) {
        //std::cout << "[SetXML] Loaded " << elementName << " from xml_root" << std::endl;
      } else {
        std::cout << "[SetXML]" << elementName << " not found in xml_root" << std::endl;
      }
    } else {
      currentElement = currentElement->FirstChildElement(elementName);
      if (currentElement) {
        //std::cout << "[SetXML] Loaded " << elementName << " from " << currentElement->Name() << std::endl;
      } else {
        std::cout << "[SetXML] " << elementName << " not found in " << currentElement->Name() << std::endl;
      }
    }
  }
  
  if (currentElement) {
    //std::cout << "[SetXML] Found element." << std::endl;
  } else {
    std::cout << "[SetXML] Did not find element." << std::endl;
  }
  
  return currentElement;
}



tinyxml2::XMLElement *
SetXML::GetElement(std::initializer_list<const char *> path,
                   bool isRequired /* = true */) {
  
  //Try to get value from XML file
  tinyxml2::XMLElement *element = GetXMLElement(path);
  if (element) {
    return element;
  }else {
    if (isRequired) {
      std::cout << std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << "[SetXML] XML element " << path << " not found, but is required." << std::endl;
      std::cout << "[SetXML] Exit."<< std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << std::endl;
      exit(-1);
    }
    return nullptr;
  }
  
}
//________________________________________________________________
std::string SetXML::GetElementText(std::initializer_list<const char *> path,
                                   bool isRequired /* = true */) {
  
  tinyxml2::XMLElement *element = GetElement(path, isRequired);
  if (element) {
    return element->GetText();
  } else {
    return "";
  }
}
//________________________________________________________________
int SetXML::GetElementInt(std::initializer_list<const char *> path,
                          bool isRequired /* = true */) {
  
  tinyxml2::XMLElement *element = GetElement(path, isRequired);
  
  if (element) {
    int value = 0;
    element->QueryIntText(&value);
    return value;
  } else {
    return 0;
  }
}
//________________________________________________________________
double SetXML::GetElementDouble(std::initializer_list<const char *> path,
                                bool isRequired /* = true */) {
  
  tinyxml2::XMLElement *element = GetElement(path, isRequired);
  
  if (element) {
    double value = 0;
    element->QueryDoubleText(&value);
    return value;
  } else {
    return 0.;
  }
}
//________________________________________________________________
std::vector<int> SetXML::GetElementVectorInt(std::initializer_list<const char *> path,
                                             bool isRequired /* = true */) {
  
  tinyxml2::XMLElement *element = GetElement(path, isRequired);
  auto pathEnd = path.end();
  --pathEnd;
  
  if (element) {
    std::vector<int> vecList;
    while (element != nullptr)
    {
      int iOutListValue;
      element->QueryIntText(&iOutListValue);
      vecList.push_back(iOutListValue);
      element = element->NextSiblingElement(*pathEnd);
    }
    return vecList;
  } else {
    return {0};
  }
}
//________________________________________________________________
std::vector<double> SetXML::GetElementVectorDouble(std::initializer_list<const char *> path,
                                                   bool isRequired /* = true */) {
  
  tinyxml2::XMLElement *element = GetElement(path, isRequired);
  auto pathEnd = path.end();
  --pathEnd;
  
  if (element) {
    std::vector<double> vecList;
    while (element != nullptr)
    {
      double iOutListValue;
      element->QueryDoubleText(&iOutListValue);
      vecList.push_back(iOutListValue);
      element = element->NextSiblingElement(*pathEnd);
    }
    return vecList;
  } else {
    return {0.};
  }
}
//________________________________________________________________
std::vector<std::string> SetXML::GetElementNameVector(std::initializer_list<const char *> path,
                                                      bool isRequired /* = true */) {
  
  tinyxml2::XMLElement *element = GetElement(path, isRequired)->FirstChildElement();
  if (element) {
    std::vector<std::string> vecList;
    while (element != nullptr)
    {
      std::string elementName = element->Name();
      vecList.push_back(elementName);
      element = element->NextSiblingElement();
    }
    return vecList;
  } else {
    return {""};
  }
  
}
//________________________________________________________________
std::ostream &operator<<(std::ostream &os,
                         std::initializer_list<const char *> path) {
  
  int i = 0;
  int size = path.size();
  
  os << "\"";
  for (auto name : path) {
    os << name;
    if (i < size - 1) {
      os << ":";
    }
    i++;
  }
  os << "\"";
  
  return os;
}
