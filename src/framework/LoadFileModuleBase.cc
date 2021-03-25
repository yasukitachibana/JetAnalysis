#include "LoadFileModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// LoadFileModuleFactory, RegisterLoadFileModule
//###############################################################################################################
// Create an instance of the static map to register modules
LoadFileModuleFactory::map_type *LoadFileModuleFactory::moduleMap =
new LoadFileModuleFactory::map_type;
//###############################################################################################################





