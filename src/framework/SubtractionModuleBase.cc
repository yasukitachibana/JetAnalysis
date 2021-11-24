#include "SubtractionModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"


//###############################################################################################################
// Initialize static MakeUniqueHelper.here
Pythia8::Pythia SubtractionModuleBase::InternalHelperPythia("IntentionallyEmpty", false);
//###############################################################################################################

//###############################################################################################################
// SubtractionModuleFactory, RegisterSubtractionModule
//###############################################################################################################
// Create an instance of the static map to register modules
SubtractionModuleFactory::map_type *SubtractionModuleFactory::moduleMap =
new SubtractionModuleFactory::map_type;
//###############################################################################################################





