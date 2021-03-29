#ifndef RECONSTRUCTIONMODULEBASE_H
#define RECONSTRUCTIONMODULEBASE_H

//#include "Pythia8/Pythia.h"
//#include "Particle.h"

#include "fastjet/ClusterSequence.hh"

#include "SubtractionModuleBase.h"

#include <vector>
#include <iostream>
#include <map>


class ReconstructionModuleBase{
  
public:
  
  virtual ~ReconstructionModuleBase(){}
  virtual std::string Name(){ return "N/A"; }

  virtual std::vector<fastjet::PseudoJet>
  JetReco( double r_cone, std::vector<std::shared_ptr<Particle>> particle_list );

  void Init(std::shared_ptr<SubtractionModuleBase> sub_ptr_in);

  void SetJetPtCut(double pt_jet_cut);


protected:
  std::shared_ptr<SubtractionModuleBase> sub_ptr;
  double jetPtCut;
  
private:

  
};



//###############################################################################################################
// ReconstructionModuleFactory, RegisterReconstructionModule
//###############################################################################################################
/// Template function for creating a new module. Used to register the module.
template <typename T> std::shared_ptr<ReconstructionModuleBase> createReconstructionModule() {
  return std::make_shared<T>();
}

// Factory to create and keep track of new modules
class ReconstructionModuleFactory {
public:
  virtual ~ReconstructionModuleFactory() {}
  
  typedef std::map<std::string, std::shared_ptr<ReconstructionModuleBase> (*)()> map_type;
  
  /// Creates an instance of an object based on the name if the name is registered in the map.
  static std::shared_ptr<ReconstructionModuleBase> createInstance(std::string const &s) {
    
    map_type::iterator it = getMap()->find(s);
    if (it == getMap()->end()) {
      return 0;
    }
    return it->second();
  }
  
protected:
  /// Creates and access the module map
  static map_type *getMap() {
    // We never delete the map (until program termination) because we cannot guarantee correct destruction order
    if (!moduleMap) {
      moduleMap = new map_type;
    }
    return moduleMap;
  }
  
private:
  /// Contains the map to all of the modules
  static map_type *moduleMap;
};

/**
 * @class RegisterReconstructionModule
 * @brief Registers Analysis modules in the factory map
 */
template <typename T>
class RegisterReconstructionModule : public ReconstructionModuleFactory {
public:
  /// Registers the name of the module to map to a function that can create the module
  RegisterReconstructionModule(std::string const &s) {
    //std::cout << "[RegisterReconstructionModule] Registering '" << s << "' to the map" << std::endl;
    getMap()->insert(std::make_pair(s, &createReconstructionModule<T>));
  }
};

class ShowRegisteredReconstructionModule : public ReconstructionModuleFactory {
public:
  ShowRegisteredReconstructionModule() {
    std::cout << "[JetAnalysis] *** [ReconstructionModuleFactory]"<< std::endl;
    auto begin = getMap()->begin(), end = getMap()->end();
    for (auto it = begin; it != end; it++) {
      std::cout << "[JetAnalysis] *** --" << it->first << "\n";
    }
  }
};


#endif
