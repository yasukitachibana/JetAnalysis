#ifndef ANALYSISMODULEBASE_H
#define ANALYSISMODULEBASE_H


#include <string>
#include <memory>
#include <random>
#include <map>
#include <iostream>
#include <array>

#include "Pythia8/Pythia.h"

#include "SubtractionModuleBase.h"
#include "ReconstructionModuleBase.h"
#include "LoadFileModuleBase.h"

#include "Histogram.h"


class AnalysisModuleBase{

public:
  
  virtual ~AnalysisModuleBase(){}

  void Init(std::shared_ptr<ReconstructionModuleBase> reco_ptr_in,
            std::shared_ptr<SubtractionModuleBase> sub_ptr_in,
            std::shared_ptr<LoadFileModuleBase> load_ptr_in);
  
  void Set(double ptHatMin, double ptHatMax);
  void Analyze(std::string input_file_name);
  void Clear();
  
  virtual std::string Name(){ return "N/A"; }

protected:
  
  std::vector<std::shared_ptr<Histogram>> hist_list;
  
  std::shared_ptr<ReconstructionModuleBase> reco_ptr;
  std::shared_ptr<SubtractionModuleBase> sub_ptr;

private:
  std::shared_ptr<LoadFileModuleBase> load_ptr;
  //=========================================
  virtual void ReadOptionParametersFromXML(){}
  virtual void ShowOptionSetting(){}
  virtual void GenerateHist(double ptHatMin, double ptHatMax);

  void ReadParametersFromXML();
  void ShowObservableSetting();
  void ShowJetSetting();
  void ShowParticleSetting();
  void SetLargestRapidity();
  void InitHist();
  //=========================================
  Pythia8::Pythia pythia;
  //=========================================
  std::vector<double> jetR;
  int chJet;
  std::vector<int> statJet;
  int jetRapidity;
  std::vector<double> jetRapMin;
  std::vector<double> jetRapMax;
  std::vector<double> jetPtMin;
  std::vector<double> jetPtMax;
  //--
  int chParticle;
  std::vector<int> statParticle;
  int particleRapidity;
  std::vector<double> particleRapMin;
  std::vector<double> particleRapMax;
  std::vector<double> particlePtMin;
  std::vector<double> particlePtMax;
  //=========================================
  std::vector<std::string> variables;
  std::vector< std::vector<double> > binSettings;
  //=========================================
  double largestRapidity;
  //=========================================
  std::string RapType(int rap){
    if(rap == 1){
      return "Eta";
    }else{
      return "Y";
    }
  }
  std::string JetType(){
    if(chJet == 1){
      return "Charged Jet";
    }else{
      return "Full Jet";
    }
  }
  std::string ParticleType(){
    if(chParticle == 1){
      return "Charged Particles";
    }else{
      return "Charged+Neutral Particles";
    }
  }
  std::string JetRapType(){return RapType(jetRapidity);}
  std::string ParticleRapType(){return RapType(particleRapidity);}

  //=========================================
  static const int nNeutrino = 4;
  std::array<int, nNeutrino> pidNeutrino{12, 14, 16, 13};//abosolute values of neutrino/anti-neutrino pids.
  //=========================================

};








//###############################################################################################################
// analysismodulefactory, registeranalysismodule
//###############################################################################################################
/// template function for creating a new module. used to register the module.
template <typename t> std::shared_ptr<AnalysisModuleBase> createAnalysisModule() {
  return std::make_shared<t>();
}

// factory to create and keep track of new modules
class AnalysisModuleFactory {
public:
  virtual ~AnalysisModuleFactory() {}

  typedef std::map<std::string, std::shared_ptr<AnalysisModuleBase> (*)()> map_type;

  /// creates an instance of an object based on the name if the name is registered in the map.
  static std::shared_ptr<AnalysisModuleBase> createInstance(std::string const &s) {
    
    
    map_type::iterator it = getMap()->find(s);
    if (it == getMap()->end()) {
      return 0;
    }
    return it->second();
  }

protected:
  /// creates and access the module map
  static map_type *getMap() {
    // we never delete the map (until program termination) because we cannot guarantee correct destruction order
    if (!moduleMap) {
      moduleMap = new map_type;
    }
    return moduleMap;
  }

private:
  /// contains the map to all of the modules
  static map_type *moduleMap;
};

/**
 * @class registeranalysismodule
 * @brief registers analysis modules in the factory map
 */
template <typename t>
class RegisterAnalysisModule : public AnalysisModuleFactory {
public:
  /// registers the name of the module to map to a function that can create the module
  RegisterAnalysisModule(std::string const &s) {
    //std::cout << "[registeranalysismodule] registering '" << s << "' to the map" << std::endl;
    getMap()->insert(std::make_pair(s, &createAnalysisModule<t>));
  }
};

class ShowRegisteredAnalysisModule : public AnalysisModuleFactory {
public:
  ShowRegisteredAnalysisModule() {
    std::cout << "[JetAnalysis] *** [AnalysisModuleFactory]"<< std::endl;
    auto begin = getMap()->begin(), end = getMap()->end();
    for (auto it = begin; it != end; it++) {
      std::cout << "[JetAnalysis] *** --" << it->first << "\n";
    }
  }
};


#endif
