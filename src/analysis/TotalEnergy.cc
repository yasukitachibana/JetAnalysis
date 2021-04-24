#include "TotalEnergy.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<TotalEnergy> TotalEnergy::reg("TotalEnergy");

TotalEnergy::TotalEnergy(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

TotalEnergy::~TotalEnergy(){
  std::cout << "-$-Deleting " << name << std::endl;
}

int SoftDropGroom::ReadOptionParametersFromXML(){
  return 3;
}

void TotalEnergy::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list){
  
  for( auto& p : particle_list ){
    
   
    for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
      for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
        
        if(ParticleTrigger(p,ipp,ipr)){
          for( int iv = 0; iv < variables.size(); iv++ ){
            hist_list[GetHistIndex(iv,0,0,0,ipp,ipr,0)]
            ->Fill( val, n );
          }
        }
        
      }
    }
    
  }
  
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TotalEnergy::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  
  std::string hist_name = GetHistName( iv, ir, ijp, ijr, ipp, ipr, ip );
  std::cout << "[TotalEnergy] hist_name = " << hist_name << std::endl;
  
  
  double delta_rapidity = 2.0*(particleRapMax[ijr]-particleRapMin[ijr]);
  std::cout << "[Particlepec] delta_rapidity = " << delta_rapidity << std::endl;
  
  
  auto total_hist = CreateHist(hist_name, iv);
  total_hist->Init();
  
  double nJetTotal = 0.0;
  
  for( auto hist: hist_list ){
    double n_ev = hist->Nev();
    if( n_ev != 0 ){
      nJetTotal += hist->GetNjetSigmaOverEev();
      double sigma = hist->Sigma();
      total_hist->Add(hist, sigma/n_ev );
    }
  }
  
  total_hist->Print("count_");
  
  total_hist->Scale(1.0,"width");
  total_hist->Print("TotalEnergy_dNdpt_");
  total_hist->Scale(1.0/delta_rapidity);
  total_hist->Print("TotalEnergy_dNdptdrap_");
  
  total_hist->DeleteTH();
  
  
}
