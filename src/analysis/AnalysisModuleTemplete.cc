#include "AnalysisModuleTemplete.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<AnalysisModuleTemplete> AnalysisModuleTemplete::reg("AnalysisModuleTemplete");

AnalysisModuleTemplete::AnalysisModuleTemplete(){
  std::cout << "-@-Creating AnalysisModule Templete" << std::endl;
}

AnalysisModuleTemplete::~AnalysisModuleTemplete(){
  std::cout << "-$-Deleting AnalysisModule Templete"<<std::endl;
}
