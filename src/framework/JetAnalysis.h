#ifndef JETANALYSIS_H
#define JETANALYSIS_H

#include "AnalysisModuleBase.h"
#include "SubtractionModuleBase.h"
#include "ReconstructionModuleBase.h"
#include "LoadFileModuleBase.h"
#include "SmearingModuleBase.h"

#include <iostream>
#include <vector>


class JetAnalysis{

public:
  JetAnalysis();
  JetAnalysis(std::string xml_filename, std::string input_dirname, std::string output_dirname);
  ~JetAnalysis();
  void Init();
  void Exec();

private:
  const std::string xml_filename;
  const std::string input_dirname;
  const std::string output_dirname;
  
  int running_mode;
  std::vector< std::string > observables;  
  std::string reconstruction_method;
  std::string subtraction_method;
  std::string input_file_style;
  std::string extra_command;
  int extra_command_arg;    


  void ShowSettings();

};

#endif
