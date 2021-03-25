#include "ReconstructionModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// ReconstructionModuleFactory, RegisterReconstructionModule
//###############################################################################################################
// Create an instance of the static map to register modules
ReconstructionModuleFactory::map_type *ReconstructionModuleFactory::moduleMap =
new ReconstructionModuleFactory::map_type;
//###############################################################################################################


void ReconstructionModuleBase::Init(std::shared_ptr<SubtractionModuleBase> sub_ptr_in)
{
  
  std::cout
  << "[ReconstructionModuleBase] Intialize " << Name()
  << " with " << sub_ptr_in->Name()
  << std::endl;
  
  sub_ptr = nullptr;
  sub_ptr=sub_ptr_in;
  
  //  ReadParametersFromXML();
  
}


