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
#include "LeadingParticle.h"
#include "Charged.h"
#include "PStat.h"
#include "Rapidity.h"
#include "PIdentify.h"
#include "ConstPt.h"
#include "JetTag.h"
#include "DeltaPhiCut.h"
#include "MassAdjust.h"

#include "Histogram.h"
#include "Hist1D.h"

// #define _GNU_SOURCE
#include <sys/resource.h>

class AnalysisModuleBase
{

public:
  virtual ~AnalysisModuleBase() {}
  //=========================================
  virtual std::string Name() { return "N/A"; }
  //=========================================
  void Init(std::shared_ptr<ReconstructionModuleBase> reco_ptr_in,
            std::shared_ptr<SubtractionModuleBase> sub_ptr_in,
            std::shared_ptr<LoadFileModuleBase> load_ptr_in);
  void Set(double ptHatMin, double ptHatMax);
  int Analyze(std::string input_file_name);
  void Combine(std::vector<double> ptHat);
  //=========================================
  virtual void Clear(int seq_loaded);
  //=========================================
  static Pythia8::Pythia InternalHelperPythia;
  long getMemoryUsage();
  //=========================================

protected:
  virtual void InitMixedEvent(){};
  //=========================================
  virtual void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip) {}
  virtual void CombineFinisher() {}
  //=========================================
  virtual std::shared_ptr<Histogram> CreateHist(std::string hist_name, int iv);
  //=========================================
  std::vector<std::shared_ptr<Histogram>> hist_list;
  std::shared_ptr<ReconstructionModuleBase> reco_ptr;
  std::shared_ptr<SubtractionModuleBase> sub_ptr;
  std::shared_ptr<LoadFileModuleBase> load_ptr;
  //=========================================
  std::unique_ptr<LeadingParticleBase> lead_ptr;
  //=========================================
  std::unique_ptr<ChargedBase> jet_charged_ptr;
  std::unique_ptr<ChargedBase> particle_charged_ptr;
  //=========================================
  std::unique_ptr<PStatBase> jet_pstat_ptr;
  std::unique_ptr<PStatBase> particle_pstat_ptr;
  //=========================================
  std::unique_ptr<RapidityBase> jet_rap_ptr;
  std::unique_ptr<RapidityBase> particle_rap_ptr;
  //=========================================
  std::unique_ptr<PIdentifyBase> jet_pid_ptr;
  std::unique_ptr<PIdentifyBase> particle_pid_ptr;
  //=========================================
  std::unique_ptr<ConstPtBase> jet_constpt_ptr;
  //=========================================
  std::unique_ptr<JetTagBase> jet_tag_ptr;
  //=========================================
  std::unique_ptr<DeltaPhiCutBase> jet_deltaphi_ptr;
  //=========================================
  std::unique_ptr<MassAdjustBase> mass_adj_ptr;
  //=========================================
  std::string particles_str;

  int jet_tag;
  int nJetEv;
  std::vector<double> jetR;
  int jetRapidity;
  std::vector<double> jetRapMin;
  std::vector<double> jetRapMax;
  std::vector<double> jetPtMin;
  std::vector<double> jetPtMax;
  std::vector<double> jetPtMinForTrigger;
  std::vector<double> jetPtMaxForTrigger;

  //--
  std::vector<int>
      statParticle;
  int particleRapidity;
  std::vector<double> particleRapMin;
  std::vector<double> particleRapMax;
  std::vector<double> particlePtMin;
  std::vector<double> particlePtMax;
  //--
  int nParams;
  std::vector<std::string> variables;
  std::vector<std::vector<double>> binSettings;

  //=========================================
  std::string GetHistName(double ptHatMin, double ptHatMax,
                          int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  std::string GetHistName(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  std::vector<int> GetHistIndex(std::vector<int> iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  int GetHistIndex(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  void DeleteHist();
  //=========================================
  bool RapidityCut(std::shared_ptr<Particle> p);
  //=========================================
  bool JetTrigger(fastjet::PseudoJet jets, int ir, int ijp, int ijr);
  bool ParticleTrigger(std::shared_ptr<Particle> p, int ipp, int ipr);
  //=========================================

private:
  int p_gun;
  virtual void EventEndMark(std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num) {}
  //=========================================
  void LoadHist(double ptHatMin, double ptHatMax,
                int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);
  //=========================================
  void ReadParametersFromXML();
  //--
  void ShowObservableSetting();
  void ShowJetSetting();
  void ShowParticleSetting();
  //--
  virtual int ReadOptionParametersFromXML() { return 1; }
  virtual void ShowParamsSetting() {}
  virtual std::string GetParamsTag(int i) { return ""; }
  //=========================================
  void SetJetPtCut();
  void SetLargestRapidity();
  //=========================================
  double largestRapidity;
  //=========================================
  void GenerateHist(double ptHatMin, double ptHatMax);
  //=========================================
};

// ###############################################################################################################
//  analysismodulefactory, registeranalysismodule
// ###############################################################################################################
/// template function for creating a new module. used to register the module.
template <typename T>
std::shared_ptr<AnalysisModuleBase> createAnalysisModule()
{
  return std::make_shared<T>();
}

// factory to create and keep track of new modules
class AnalysisModuleFactory
{
public:
  virtual ~AnalysisModuleFactory() {}

  typedef std::map<std::string, std::shared_ptr<AnalysisModuleBase> (*)()> map_type;

  /// creates an instance of an object based on the name if the name is registered in the map.
  static std::shared_ptr<AnalysisModuleBase> createInstance(std::string const &s)
  {

    map_type::iterator it = getMap()->find(s);
    if (it == getMap()->end())
    {
      return 0;
    }
    return it->second();
  }

protected:
  /// creates and access the module map
  static map_type *getMap()
  {
    // we never delete the map (until program termination) because we cannot guarantee correct destruction order
    if (!moduleMap)
    {
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
template <typename T>
class RegisterAnalysisModule : public AnalysisModuleFactory
{
public:
  /// registers the name of the module to map to a function that can create the module
  RegisterAnalysisModule(std::string const &s)
  {
    // std::cout << "[registeranalysismodule] registering '" << s << "' to the map" << std::endl;
    getMap()->insert(std::make_pair(s, &createAnalysisModule<T>));
  }
};

class ShowRegisteredAnalysisModule : public AnalysisModuleFactory
{
public:
  ShowRegisteredAnalysisModule()
  {
    std::cout << "[JetAnalysis] *** [AnalysisModuleFactory]" << std::endl;
    auto begin = getMap()->begin(), end = getMap()->end();
    for (auto it = begin; it != end; it++)
    {
      std::cout << "[JetAnalysis] *** --" << it->first << "\n";
    }
  }
};

#endif
