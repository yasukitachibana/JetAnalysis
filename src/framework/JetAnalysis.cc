#include "JetAnalysis.h"
#include "EventAnalysis.h"
#include "CombineEvents.h"
#include "SetXML.h"
#include "SetFile.h"

#include <iostream>

JetAnalysis::JetAnalysis
(std::string xml_filename_in,
 std::string input_dirname_in,
 std::string output_dirname_in):
xml_filename(xml_filename_in),
input_dirname(input_dirname_in),
output_dirname(output_dirname_in)
{
  std::cout << "-@-Creating JetAnalysis"<<std::endl;
  std::cout <<  "=========================================================" << std::endl;
  SetXML::Instance()->OpenXMLFile(xml_filename);
  std::cout <<  "=========================================================" << std::endl;
  SetFile::Instance()->Init(input_dirname, output_dirname);
  std::cout <<  "=========================================================" << std::endl;
}


JetAnalysis::JetAnalysis(){
  std::cout << "-@-Creating JetAnalysis"<<std::endl;
}

JetAnalysis::~JetAnalysis() {
  std::cout << "-$-Deleting JetAnalysis"<<std::endl;
}

void JetAnalysis::Init() {
  running_mode = SetXML::Instance()->GetElementInt({"mode"});
  observables = SetXML::Instance()->GetElementNameVector({"observable"});
  reconstruction_method = SetXML::Instance()->GetElementText({"jetReco","reconstructionMethod"});
  subtraction_method = SetXML::Instance()->GetElementText({"subtractionMethod"});
  input_file_style = SetXML::Instance()->GetElementText({"inputFiles","inputStyle"});
  ShowSettings();
}

void JetAnalysis::ShowSettings(){
  
  std::cout << "[JetAnalysis] **********************************************"<< std::endl;
  std::cout << "[JetAnalysis] *** Registered Modules"<< std::endl;
  std::cout << "[JetAnalysis] ***-------------------------------------------"<< std::endl;
  ShowRegisteredAnalysisModule();
  std::cout << "[JetAnalysis] ***-------------------------------------------"<< std::endl;
  ShowRegisteredReconstructionModule();
  std::cout << "[JetAnalysis] ***-------------------------------------------"<< std::endl;
  ShowRegisteredSubtractionModule();
  std::cout << "[JetAnalysis] ***-------------------------------------------"<< std::endl;
  ShowRegisteredLoadFileModule();
  std::cout << "[JetAnalysis] **********************************************"<< std::endl<< std::endl;
  
  std::cout << "[JetAnalysis] **********************************************"<< std::endl;
  std::cout << "[JetAnalysis] *** Settings"<< std::endl;
  std::cout << "[JetAnalysis] ***-------------------------------------------"<< std::endl;
  std::cout << "[JetAnalysis] *** Reconstruction Method: " << reconstruction_method << std::endl;
  std::cout << "[JetAnalysis] *** Subtraction Method: " << subtraction_method << std::endl;
  std::cout << "[JetAnalysis] *** Input File Style: " << input_file_style << std::endl;
  std::cout << "[JetAnalysis] **********************************************"<< std::endl;
  
}

void JetAnalysis::Exec() {
  

  std::cout << "[JetAnalysis] Start JetAnalysis" << std::endl;
  std::cout <<  "=========================================================" << std::endl;

  auto reco_ptr = ReconstructionModuleFactory::createInstance(reconstruction_method);
  auto sub_ptr = SubtractionModuleFactory::createInstance(subtraction_method);
  auto load_ptr = LoadFileModuleFactory::createInstance(input_file_style);

  sub_ptr->Init();
  load_ptr->Init();
  reco_ptr->Init(sub_ptr);
  
  for(auto &obs: observables){
    
    std::string obs_name = SetXML::Instance()->GetElementText({"observable",obs.c_str(),"name"});
    std::cout << "[JetAnalysis] Observable: " << obs_name << " [" << obs << "]" << std::endl;
    std::cout <<  "---------------------------------------------------------" << std::endl;

    auto obs_ptr = AnalysisModuleFactory::createInstance(obs);
    
    obs_ptr->Init(reco_ptr, sub_ptr, load_ptr);
    
    if( running_mode == 0 || running_mode == 1 ){

      std::cout << "[JetAnalysis] -Event Analysis-" << std::endl;

      EventAnalysis ea(obs_ptr);
      ea.Init();
      ea.Analyze();
      ea.Clear();
    }else{
      std::cout << "[JetAnalysis] Skip Event Analysis " << std::endl;
    }
    
    std::cout <<  "---------------------------------------------------------" << std::endl;
    if( running_mode == 0 || running_mode == 2 ){
      std::cout << "[JetAnalysis] -Combine-" << std::endl;
      
      CombineEvents ce(obs_ptr);
      ce.Init();
      ce.Combine();
      ce.Clear();
    }else{
      std::cout << "[JetAnalysis] Skip Combine " << std::endl;
    }
    std::cout <<  "=========================================================" << std::endl;
    
  }
  

}
