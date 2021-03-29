#ifndef LOADFILEMODULEBASE_H
#define LOADFILEMODULEBASE_H

//#include "Pythia8/Pythia.h"
//#include "Particle.h"

#include "Particle.h"

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


class LoadFileModuleBase{
  
public:
  
  virtual ~LoadFileModuleBase(){}
  virtual void Init(){}
  
  virtual std::string Name(){ return "N/A"; }
  
  virtual void Load(std::string input_filename){}
  virtual void Clear(){}
  
  virtual void GetParticleList(){}
  virtual int GetLine(){return 0;}
  virtual void ShowLine(){}
  virtual int EventEnd(){return 0;}
  virtual int ValidLine(){return 0;}
  virtual int Last(){return 1;}
  virtual void LoadSigma(std::string sigma_filename, double &sigma, double &sigma_err){}
  
  virtual std::shared_ptr<Particle> GetParticle(){return nullptr;}


private:
  
};



//###############################################################################################################
// LoadFileModuleFactory, RegisterLoadFileModule
//###############################################################################################################
/// Template function for creating a new module. Used to register the module.
template <typename T> std::shared_ptr<LoadFileModuleBase> createLoadFileModule() {
  return std::make_shared<T>();
}

// Factory to create and keep track of new modules
class LoadFileModuleFactory {
public:
  virtual ~LoadFileModuleFactory() {}
  
  typedef std::map<std::string, std::shared_ptr<LoadFileModuleBase> (*)()> map_type;
  
  /// Creates an instance of an object based on the name if the name is registered in the map.
  static std::shared_ptr<LoadFileModuleBase> createInstance(std::string const &s) {
    
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
 * @class RegisterLoadFileModule
 * @brief Registers Analysis modules in the factory map
 */
template <typename T>
class RegisterLoadFileModule : public LoadFileModuleFactory {
public:
  /// Registers the name of the module to map to a function that can create the module
  RegisterLoadFileModule(std::string const &s) {
    //std::cout << "[RegisterLoadFileModule] Registering '" << s << "' to the map" << std::endl;
    getMap()->insert(std::make_pair(s, &createLoadFileModule<T>));
  }
};

class ShowRegisteredLoadFileModule : public LoadFileModuleFactory {
public:
  ShowRegisteredLoadFileModule() {
    std::cout << "[JetAnalysis] *** [LoadFileModuleFactory]"<< std::endl;
    auto begin = getMap()->begin(), end = getMap()->end();
    for (auto it = begin; it != end; it++) {
      std::cout << "[JetAnalysis] *** --" << it->first << "\n";
    }
  }
};


#endif
