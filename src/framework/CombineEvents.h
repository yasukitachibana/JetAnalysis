#ifndef COMBINEEVENTS_H_
#define COMBINEEVENTS_H_

#include "AnalysisModuleBase.h"


class CombineEvents{
public:
  
  CombineEvents(std::shared_ptr<AnalysisModuleBase> obs_ptr_in);
  ~CombineEvents();
  void Init();
  void Clear();
  void Combine();
  
private:
  std::shared_ptr<AnalysisModuleBase> obs_ptr;
  std::vector<double> ptHat;
  
};

#endif 
