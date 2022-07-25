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
  int ir, int ijp, int ijr ){
   
   //--------------------------------------------------------------------------------------------------
   for( int iv = 0; iv < variables.size(); iv++ ){
     for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
       for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
         
         int index = GetHistIndex(iv,ir,ijp,ijr,ipp,ipr,0);
         hist_list[index]->JetTriggered();

         
         for( auto p : particle_list ){
           if( ParticleTrigger(p, ipp, ipr) ){


             
             double delta_eta = p->eta() - jet.eta() ;
             double delta_phi = jet.delta_phi_to( p->GetPseudoJet() );
             double delta_r = TMath::Sqrt( delta_eta*delta_eta + delta_phi*delta_phi);
             
              //--------------------------------------------------------------------------------------------------
             if( delta_r <= jetR[ir] ){
               
               double val = p->perp(); // particle pt
               double n = sub_ptr->nSub(p);

               
               if( variables[iv] == "z" ){
                 double jpt = jet.perp();
                 val =val*cos(delta_r)/jpt;
               }
               
               hist_list[index]->Fill( val, n );

              // std::cout << "p=" << p->perp()
              // << " y=" << particle_rap_ptr->Val(p) << std::endl;
              // std::cout << " eta=" << p->eta() << std::endl;
              // std::cout << " dr=" << delta_r << std::endl;              
              // std::cout << "pid=" << p->pid() << std::endl;

               
             }
              //--------------------------------------------------------------------------------------------------

           }
         }
         
       }//ipr
     }//ipp
   }//iv
   //--------------------------------------------------------------------------------------------------
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
