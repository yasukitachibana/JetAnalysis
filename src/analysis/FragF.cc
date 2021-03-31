#include "FragF.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<FragF> FragF::reg("FragF");



FragF::FragF(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

FragF::~FragF(){
  std::cout << "-$-Deleting " << name << std::endl;
}

void FragF::SetObservable
(fastjet::PseudoJet jet,
 std::vector<std::shared_ptr<Particle>> particle_list,
 int ir, std::vector<std::array<int, 2>> i_j ){
  
  for( auto& p : particle_list ){
    std::vector<std::array<int, 2>> i_p;
    if( ParticleTrigger(p, i_p)){
      
      double delta_eta = p->eta() - jet.eta() ;
      double delta_phi = jet.delta_phi_to( p->GetPseudoJet() );
      double delta_r = TMath::Sqrt( delta_eta*delta_eta + delta_phi*delta_phi);
      
      if( delta_r <= jetR[ir] ){
        
        for( auto ijet: i_j){
          for( auto iparticle: i_p){
            
            for( int ip = 0; ip < nParams; ip++ ){
              for( int iv = 0; iv < variables.size(); iv++ ){
                
                double val = p->perp(); // particle pt
                double n = sub_ptr->nSub(p);
                
                if( variables[iv] == "z" ){
                  double jpt = jet.perp();
                  val =val*cos(delta_r)/jpt;
                }
                
                hist_list[GetHistIndex(iv,ir,ijet[0],ijet[1],iparticle[0],iparticle[1],ip)]->Fill( val, n );
                
                
              }
            }
          }
        }
        
        
      }// inside th jet cone
      
    }// trigger
  }
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FragF::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  
  std::string hist_name = GetHistName( iv, ir, ijp, ijr, ipp, ipr, ip );
  std::cout << "[FragF] hist_name = " << hist_name << std::endl;
  
  
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
  
  if(nJetTotal!=0){
    total_hist->Scale(1.0/nJetTotal,"width");
    total_hist->Print("fragf_");
  }else{
    std::cout << "[FragF] 0-total Jet" << std::endl;
    std::cout << "[FragF] Skip. "<< std::endl;
  }
  
  total_hist->DeleteTH();
  
  
}
