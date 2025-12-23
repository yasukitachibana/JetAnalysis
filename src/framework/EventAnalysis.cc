#include "EventAnalysis.h"
#include "SetXML.h"
#include "SetFile.h"
#include "SetPtHatBins.h"

#include <iostream>

EventAnalysis::EventAnalysis(std::shared_ptr<AnalysisModuleBase> obs_ptr_in) : obs_ptr(obs_ptr_in)
{
  std::cout << "-@-Creating EventAnalysis for " << obs_ptr->Name() << std::endl;
}

EventAnalysis::~EventAnalysis()
{
  std::cout << "-$-Deleting EventAnalysis for " << obs_ptr->Name() << std::endl;
}



void EventAnalysis::Init()
{

  std::cout << "[EventAnalysiss] Intialize EventAnalysiss" << std::endl;

  ptHat = SetPtHatBins::Instance()->PtHatBinList();
  runNum = SetPtHatBins::Instance()->RunNumList();
}

void EventAnalysis::Clear()
{
  ptHat.clear();
  ptHat.shrink_to_fit();
  runNum.clear();
  runNum.shrink_to_fit();
}

void EventAnalysis::Analyze()
{

  int nPtHatBins = ptHat.size() - 1;
  std::cout << "[EventAnalysiss] Start EventAnalysiss" << std::endl;
  std::cout << "[EventAnalysiss] Number of pt-hat Bins: " << nPtHatBins << std::endl;
  std::cout << "[EventAnalysiss] Bin edges: [";
  for (auto pt_hat : ptHat)
  {
    std::cout << int(pt_hat) << ", ";
  }
  std::cout << "\b\b]" << std::endl;

  for (int i_pthat_bin = 0; i_pthat_bin < nPtHatBins; i_pthat_bin++)
  {

    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "[EventAnalysiss] Start Analysis for pt_hat: " << ptHat[i_pthat_bin] << "-" << ptHat[i_pthat_bin + 1] << "GeV, RunNum: " << runNum[i_pthat_bin] << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;

    //++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++
    obs_ptr->Set(ptHat[i_pthat_bin], ptHat[i_pthat_bin + 1]);
    //++++++++++++++++++++++++++++++++++++++++++++++++
    int seq_loaded = 0;
    for (int i_seq = 0; i_seq < runNum[i_pthat_bin]; i_seq++)
    {
      std::string input_file_name = SetFile::Instance()->GetInputFileName(ptHat[i_pthat_bin], ptHat[i_pthat_bin + 1], i_seq);
      seq_loaded += obs_ptr->Analyze(input_file_name);
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++
    obs_ptr->Clear(seq_loaded);
    //++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++
  }
  std::cout << "---------------------------------------------------------" << std::endl;
  std::cout << "[EventAnalysiss] Done EventAnalysiss" << std::endl;
}
