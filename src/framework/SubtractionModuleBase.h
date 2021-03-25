#ifndef SUBTRACTIONMODULEBASE_H
#define SUBTRACTIONMODULEBASE_H

#include "Pythia8/Pythia.h"

#include "Particle.h"
#include <vector>
#include <iostream>


class SubtractionModuleBase{
  
public:
  
  virtual ~SubtractionModuleBase(){}
  virtual std::string Name(){ return "N/A"; }
  
  virtual std::vector <fastjet::PseudoJet>
  JetSub(double jetR,
         std::vector <fastjet::PseudoJet> jets,
         std::vector<std::shared_ptr<Particle>> particle_list )
  { return jets; }
  
  virtual std::vector < std::shared_ptr<fastjet::PseudoJet> >
  JetSub(double jetR,
         std::vector < std::shared_ptr<fastjet::PseudoJet> > jets,
         std::vector<std::shared_ptr<Particle>> particle_list )
  { return jets; }
  
  
  virtual double ptSub( std::shared_ptr<Particle> particle ){ return  particle->perp(); }
  virtual double nSub( std::shared_ptr<Particle> particle ){ return  1.0; }
  
private:
  
};



//###############################################################################################################
// SubtractionModuleFactory, RegisterSubtractionModule
//###############################################################################################################
/// Template function for creating a new module. Used to register the module.
template <typename T> std::shared_ptr<SubtractionModuleBase> createSubtractionModule() {
  return std::make_shared<T>();
}

// Factory to create and keep track of new modules
class SubtractionModuleFactory {
public:
  virtual ~SubtractionModuleFactory() {}
  
  typedef std::map<std::string, std::shared_ptr<SubtractionModuleBase> (*)()> map_type;
  
  /// Creates an instance of an object based on the name if the name is registered in the map.
  static std::shared_ptr<SubtractionModuleBase> createInstance(std::string const &s) {
    
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
 * @class RegisterSubtractionModule
 * @brief Registers Analysis modules in the factory map
 */
template <typename T>
class RegisterSubtractionModule : public SubtractionModuleFactory {
public:
  /// Registers the name of the module to map to a function that can create the module
  RegisterSubtractionModule(std::string const &s) {
    //std::cout << "[RegisterSubtractionModule] Registering '" << s << "' to the map" << std::endl;
    getMap()->insert(std::make_pair(s, &createSubtractionModule<T>));
  }
};

class ShowRegisteredSubtractionModule : public SubtractionModuleFactory {
public:
  ShowRegisteredSubtractionModule() {
    std::cout << "[JetAnalysis] *** [SubtractionModuleFactory]"<< std::endl;
    auto begin = getMap()->begin(), end = getMap()->end();
    for (auto it = begin; it != end; it++) {
      std::cout << "[JetAnalysis] *** --" << it->first << "\n";
    }
  }
};


#endif
