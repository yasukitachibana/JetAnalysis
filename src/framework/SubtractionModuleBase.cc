#include "SubtractionModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// SubtractionModuleFactory, RegisterSubtractionModule
//###############################################################################################################
// Create an instance of the static map to register modules
SubtractionModuleFactory::map_type *SubtractionModuleFactory::moduleMap =
new SubtractionModuleFactory::map_type;
//###############################################################################################################





