#ifndef EVENTANALYSIS_H
#define EVENTANALYSIS_H

#include "AnalysisModuleBase.h"

#include <iostream>
#include <vector>


class EventAnalysis{

public:

  EventAnalysis(std::shared_ptr<AnalysisModuleBase> obs_ptr_in);
  ~EventAnalysis();

  void Init();
  void Analyze();
  void Clear();

private:
  
  std::shared_ptr<AnalysisModuleBase> obs_ptr;
  std::vector<double> ptHat;
  int nSeq;








};

#endif
