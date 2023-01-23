#ifndef TAGGEDJETPAIR_H
#define TAGGEDJETPAIR_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class TaggedJetPair : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<TaggedJetPair> reg;
  const std::string name;

  void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, int i_tag_particle);
  // void SetObservable (int i_tag_particle, fastjet::PseudoJet jet,
  //                     std::vector<std::shared_ptr<Particle>> particle_list,
  //                     int ir, int ijp, int ijr );

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  //----------------------------------------------------------------------------
  int ReadOptionParametersFromXML();

  int ReadVariablesFromXML(std::string tag);

  std::vector<double> rMin;
  std::vector<double> rMax;

  std::vector<double> aMin;
  std::vector<double> aMax;  

  static const int n_var = 3;
  // 0:"xJJTag", 1:"rJJ", 2:"aJJTag"
  std::array<std::vector<int>, n_var> i_var;
  std::array<std::string, n_var> varNames = {"xJJTag", "rJJ", "aJJTag"};
  //----------------------------------------------------------------------------  

public:
  TaggedJetPair(std::string name_in = "TaggedJetPair");
  ~TaggedJetPair();
  std::string Name() { return name; }
};

#endif // TaggedJetPair_H
