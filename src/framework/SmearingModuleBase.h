#ifndef SMEARINGMODULEBASE_H
#define SMEARINGMODULEBASE_H

#include "Particle.h"
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class SmearingModuleBase
{

public:
  virtual ~SmearingModuleBase() {}
  virtual void Init() {}

  virtual std::string Name() { return "N/A"; }

  virtual void ShowSmearingSetting() {}
  virtual void PrintSmearingSetting() {}
  virtual std::string SettingTag() {return "";};  


  // Just pT------------------------------
  virtual double Smear(double pt) { return pt; }

  // One Jet or Particle------------------------------
  virtual std::shared_ptr<Particle>
  Smear(std::shared_ptr<Particle> particle) { return particle; }

  virtual fastjet::PseudoJet
  Smear(fastjet::PseudoJet jet) { return jet; }

  virtual std::shared_ptr<fastjet::PseudoJet>
  Smear(std::shared_ptr<fastjet::PseudoJet> jet) { return jet; }

  // List of Jets or Particles------------------------------
  virtual std::vector<std::shared_ptr<Particle>>
  Smear(std::vector<std::shared_ptr<Particle>> particles) { return particles; }

  virtual std::vector<fastjet::PseudoJet>
  Smear(std::vector<fastjet::PseudoJet> jets) { return jets; }

  virtual std::vector<std::shared_ptr<fastjet::PseudoJet>>
  Smear(std::vector<std::shared_ptr<fastjet::PseudoJet>> jets) { return jets; }

protected:
  int initialized = 0;

private:
};

// ###############################################################################################################
//  SmearingModuleFactory, RegisterSmearingModule
// ###############################################################################################################
/// Template function for creating a new module. Used to register the module.
template <typename T>
std::shared_ptr<SmearingModuleBase> createSmearingModule()
{
  return std::make_shared<T>();
}

// Factory to create and keep track of new modules
class SmearingModuleFactory
{
public:
  virtual ~SmearingModuleFactory() {}

  typedef std::map<std::string, std::shared_ptr<SmearingModuleBase> (*)()> map_type;

  /// Creates an instance of an object based on the name if the name is registered in the map.
  static std::shared_ptr<SmearingModuleBase> createInstance(std::string const &s)
  {

    map_type::iterator it = getMap()->find(s);
    if (it == getMap()->end())
    {
      return 0;
    }
    return it->second();
  }

protected:
  /// Creates and access the module map
  static map_type *getMap()
  {
    // We never delete the map (until program termination) because we cannot guarantee correct destruction order
    if (!moduleMap)
    {
      moduleMap = new map_type;
    }
    return moduleMap;
  }

private:
  /// Contains the map to all of the modules
  static map_type *moduleMap;
};

/**
 * @class RegisterSmearingModule
 * @brief Registers Analysis modules in the factory map
 */
template <typename T>
class RegisterSmearingModule : public SmearingModuleFactory
{
public:
  /// Registers the name of the module to map to a function that can create the module
  RegisterSmearingModule(std::string const &s)
  {
    // std::cout << "[RegisterSmearingModule] Registering '" << s << "' to the map" << std::endl;
    getMap()->insert(std::make_pair(s, &createSmearingModule<T>));
  }
};

class ShowRegisteredSmearingModule : public SmearingModuleFactory
{
public:
  ShowRegisteredSmearingModule()
  {
    std::cout << "[JetAnalysis] *** [SmearingModuleFactory]" << std::endl;
    auto begin = getMap()->begin(), end = getMap()->end();
    for (auto it = begin; it != end; it++)
    {
      std::cout << "[JetAnalysis] *** --" << it->first << "\n";
    }
  }
};

#endif
