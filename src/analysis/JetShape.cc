#include "JetShape.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetShape> JetShape::reg("JetShape");



JetShape::JetShape(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetShape::~JetShape(){
  std::cout << "-$-Deleting " << name << std::endl;
}

void JetShape::SetObservable
(fastjet::PseudoJet jet,
 std::vector<std::shared_ptr<Particle>> particle_list,
 int ir, int ijp, int ijr ){
  
  //--------------------------------------------------------------------------------------------------
  for( int iv = 0; iv < variables.size(); iv++ ){
    for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
      for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
        
        int index = GetHistIndex(iv,ir,ijp,ijr,ipp,ipr,0);
        hist_list[index]->JetTriggered();
        
        std::cout << "############\n"
        << ", particle number: " << particle_list.size() <<std::endl;

        
        for( auto p : particle_list ){
          std::cout
            << "\n[BT] stat:" << p->pstat()
            << ", pid:"<< p->pid()
            << ", plabel:" <<p->plabel()
            << ", e:" <<p->e()
            << ", px:" <<p->px()
            << ", py:" <<p->py()
            << ", eta:"<< p->eta()
            << ", phi:" <<p->phi_std()
            << std::endl;

          if( ParticleTrigger(p, ipp, ipr) ){

            std::cout
              << "[T] stat:" << p->pstat()
              << ", pid:"<< p->pid()
              << ", plabel:" <<p->plabel()
              << ", e:" <<p->e()
              << ", px:" <<p->px()
              << ", py:" <<p->py()
              << ", eta:"<< p->eta()
              << ", phi:" <<p->phi_std()
              << std::endl;

            
            
            
            
            double pt = sub_ptr->ptSub(p);
            double delta_eta = p->eta() - jet.eta() ;
            double delta_phi = jet.delta_phi_to( p->GetPseudoJet() );
            double delta_r = TMath::Sqrt( delta_eta*delta_eta + delta_phi*delta_phi);
            
            //--------------------------------------------------------------------------------------------------
            hist_list[index]->Fill(delta_r, pt);
            //--------------------------------------------------------------------------------------------------
            
          }
        }
        
      }//ipr
    }//ipp
  }//iv
  //--------------------------------------------------------------------------------------------------
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JetShape::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  
  std::string hist_name = GetHistName( iv, ir, ijp, ijr, ipp, ipr, ip );
  std::cout << "[JetShape] hist_name = " << hist_name << std::endl;
  
  
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
    total_hist->Print("jetshape_");
    total_hist->Normalize("width");
    total_hist->Print("normalized_jetshape_");
  }else{
    std::cout << "[JetShape] 0-total Jet" << std::endl;
    std::cout << "[JetShape] Skip. "<< std::endl;
  }
  
  total_hist->DeleteTH();
  
  
}
