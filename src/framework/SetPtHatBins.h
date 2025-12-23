#ifndef SETPTHATBINS_H_
#define SETPTHATBINS_H_

#include<iostream>
#include <vector>

class SetPtHatBins
{
  public:
  static SetPtHatBins* Instance();
  void Init();

  std::vector<double> PtHatBinList()
  {
    return ptHatBinList;
  }

  std::vector<int> RunNumList()
  {
    return runNumList;
  }

  bool DivNum()
  {
    return divNum;
  }

  void PrintBinSettings();
  
  private: 
    SetPtHatBins(){
    std::cout << "-@-Creating SetPtHatBins"<<std::endl;
    };
    
    SetPtHatBins(SetPtHatBins const&){};

    static SetPtHatBins* m_pInstance;


    void SetPtHatBinList();
    void SetRunNumList();
    std::vector<int> GenerateConstRunNumList(int run_number);

    std::vector<double> ptHatBinList;
    std::vector<int> runNumList;
    bool ptHatBinListSet;
    bool runNumListSet;    
    int divNum;

};



#endif