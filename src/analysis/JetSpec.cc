#include "JetSpec.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetSpec> JetSpec::reg("JetSpec");



JetSpec::JetSpec(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetSpec::~JetSpec(){
  std::cout << "-$-Deleting " << name << std::endl;
}
