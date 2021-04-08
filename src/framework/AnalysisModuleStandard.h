#ifndef ANALYSISMODULESTANDARD_H
#define ANALYSISMODULESTANDARD_H


#include "AnalysisModuleBase.h"

class AnalysisModuleStandard: public AnalysisModuleBase{
  
public:
  void Analyze(std::string input_file_name);

  
protected:

  
private:
  void EventEndMark(std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num);
  virtual void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list);
  virtual void SetObservable(fastjet::PseudoJet jet,
                             std::vector<std::shared_ptr<Particle>> particle_list,
                             int ir, int ijp, int ijr ){}
  
};









#endif
