#include "SmearingModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// SmearingModuleFactory, RegisterSmearingModule
//###############################################################################################################
// Create an instance of the static map to register modules
SmearingModuleFactory::map_type *SmearingModuleFactory::moduleMap =
new SmearingModuleFactory::map_type;
//###############################################################################################################





