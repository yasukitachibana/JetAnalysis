#ifndef ANALYSISMODULEMIXEDEVENT_H
#define ANALYSISMODULEMIXEDEVENT_H


#include "AnalysisModuleBase.h"

class AnalysisModuleMixedEvent: public AnalysisModuleBase{
  
public:
  void Clear();
  
protected:
  
  
private:
  void InitMixedEvent();
  std::shared_ptr<LoadFileModuleBase> load_mixed_ptr;
  virtual std::string GetParamsTag( int i );
  virtual int ReadOptionParametersFromXML(){return 2;}
  
  void EventEndMark(std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num);
  virtual void OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list,
                                std::vector<std::shared_ptr<Particle>> mixed_event_particle_list);
  virtual void SetObservable(fastjet::PseudoJet jet,
                             std::vector<std::shared_ptr<Particle>> particle_list,
                             std::vector<std::shared_ptr<Particle>> mixed_event_particle_list,
                             int ir, int ijp, int ijr ){}
  
  std::vector<std::shared_ptr<Particle>> GetMixedEventParticles();
  
};

#endif
