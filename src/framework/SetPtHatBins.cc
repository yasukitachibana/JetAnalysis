#include "SetPtHatBins.h"
#include "SetXML.h"
#include "VectorUtil.h"

#include <iostream>

SetPtHatBins *SetPtHatBins::m_pInstance = NULL;

SetPtHatBins *SetPtHatBins::Instance()
{
  if (!m_pInstance)
  {
    std::cout << "[SetPtHatBins] Created SetPtHatBins Instance" << std::endl;
    m_pInstance = new SetPtHatBins();
  }
  return m_pInstance;
}

void SetPtHatBins::Init()
{

  ptHatBinList.clear();
  ptHatBinList.shrink_to_fit();

  runNumList.clear();
  runNumList.shrink_to_fit();

  ptHatBinListSet = false;
  runNumListSet = false;
  SetPtHatBinList();
  SetRunNumList();
  PrintBinSettings();
}

void SetPtHatBins::SetPtHatBinList()
{
  int ptHatEvenlySpaced = SetXML::Instance()->GetElementInt({"ptHat", "evenlySpaced"}, false);

  if (ptHatEvenlySpaced == 1)
  {
    // std::cout << "[SetPtHatBins] Setting ptHat bins evenly spaced." << std::endl;
    double ptHatMin = SetXML::Instance()->GetElementDouble({"ptHat", "min"});
    double ptHatMax = SetXML::Instance()->GetElementDouble({"ptHat", "max"});
    double ptHatInterval = SetXML::Instance()->GetElementDouble({"ptHat", "interval"});
    // std::cout << "[SetPtHatBins] ptHat Min: " << ptHatMin << " GeV" << std::endl;
    // std::cout << "[SetPtHatBins] ptHat Max: " << ptHatMax << " GeV" << std::endl;
    // std::cout << "[SetPtHatBins] ptHat Interval: " << ptHatInterval << " GeV" << std::endl;
    ptHatBinList = VectorUtil::VectorArange(ptHatMin, ptHatMax + 0.5 * ptHatInterval, ptHatInterval);
  }
  else
  {
    ptHatBinList = SetXML::Instance()->GetElementVectorDouble({"ptHat", "Item"});
  }
  ptHatBinListSet = true;
}

std::vector<int> SetPtHatBins::GenerateConstRunNumList(int run_number)
{
  std::vector<int> const_run_num_list;
  const size_t n = ptHatBinList.size();
  for (size_t i = 0; i + 1 < n; ++i)
  {
    const_run_num_list.push_back(run_number);
  }
  return const_run_num_list;
}

void SetPtHatBins::SetRunNumList()
{
  if (!ptHatBinListSet)
  {
    std::cout << "[SetPtHatBins] Error: ptHatBinList is not set yet." << std::endl;
    std::cout << "[SetPtHatBins] ptHatBinList must be set before setting runNumList." << std::endl;
    exit(-1);
  }

  divNum = SetXML::Instance()->GetElementInt({"inputFiles", "divNum"});

  if (divNum < 0)
  {
    runNumList = SetXML::Instance()->GetElementVectorInt({"ptHat", "RunNum"});
  }
  else
  {
    int run_number;
    if (divNum == 0)
    {
      run_number = 1;
    }
    else
    {
      run_number = divNum;
    }
    runNumList = GenerateConstRunNumList(run_number);
  }
  runNumListSet = true;
}

void SetPtHatBins::PrintBinSettings()
{
  std::cout << "[SetPtHatBins] Bin Settings:" << std::endl;
  for (size_t i = 0; i + 1 < ptHatBinList.size(); ++i)
  {
    std::cout << "[SetPtHatBins] " << i 
    << ": ptHat " << ptHatBinList[i] << "--" << ptHatBinList[i + 1] 
    << " GeV, RunNum: " << runNumList[i]
    << std::endl;
  }
}