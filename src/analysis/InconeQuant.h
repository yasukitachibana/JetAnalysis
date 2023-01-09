#ifndef JETSHAPE_H
#define JETSHAPE_H

#include "AnalysisModuleStandard.h"
#include <iostream>

class InconeQuant : public AnalysisModuleStandard
{
private:
  // Allows the registration of the module so that it is available to be used by the Analysis framework.
  static RegisterAnalysisModule<InconeQuant> reg;
  const std::string name;

  void SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                     std::vector<std::shared_ptr<Particle>> particle_list,
                     int ir, int ijp, int ijr);

  void CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip);

  int ReadOptionParametersFromXML();

  //-------------------------------------------
  // Get Tags for Parameters
  std::string GetParamsTag(int i);
  std::string GetParamsTag(std::array<int, 2> i);
  std::string GetParamsTag(int i_q, int j_q);
  std::string GetParamsTag(std::string q1, std::string q2);

  //-------------------------------------------
  // Get Index of Tags for Parameters
  int GetParamIndex(std::array<int, 2> i);
  int GetParamIndex(int i_q1, int i_12);
  std::array<int, 2> GetParamIndex(int i);

  // List of quantity counted in this module.
  // By default, the number of jets is also counted.
  static const int n_quant = 10;
  const std::array<std::string, n_quant> c_quant =
      {"count", "charge", "baryon", "strange", "charm",
       "nparticle", "ncharge", "nbaryon", "nstrange", "ncharm"};

public:
  InconeQuant(std::string name_in = "InconeQuant");
  ~InconeQuant();
  std::string Name() { return name; }
};

#endif // JetShape_H
