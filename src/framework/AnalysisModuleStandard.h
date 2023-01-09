#ifndef ANALYSISMODULESTANDARD_H
#define ANALYSISMODULESTANDARD_H

#include "AnalysisModuleBase.h"

class AnalysisModuleStandard : public AnalysisModuleBase
{

public:
protected:
private:
  void EventEndMark(std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num);
  virtual void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list,
                                int i_tag_particle);
  virtual void SetObservable(int i_tag_particle,
                             fastjet::PseudoJet jet,
                             std::vector<std::shared_ptr<Particle>> particle_list,
                             int ir, int ijp, int ijr) {}
  //================================================
  // Jet Info Output
  // std::ofstream jet_infos;
  // int current_event;
  //================================================
};

#endif
