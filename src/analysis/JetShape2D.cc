#include "JetShape2D.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetShape2D> JetShape2D::reg("JetShape2D");



JetShape2D::JetShape2D(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetShape2D::~JetShape2D(){
  std::cout << "-$-Deleting " << name << std::endl;
}

int JetShape2D::ReadOptionParametersFromXML(){
  
  
  nBinPhi = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinPhi"});
  boundPhi = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundPhi","Item"});
  
  nBinEta = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinEta"});
  boundEta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundEta","Item"});
  
  sidebandRap = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(),"sidebandRap","Item"});
  
  mixedEvent = SetXML::Instance()->GetElementInt({"observable", Name().c_str(),"mixedEvent"});
  
  return 1;
}

std::shared_ptr<Histogram> JetShape2D::CreateHist( std::string hist_name, int iv ){
  return std::make_shared<Hist2D>(hist_name, nBinPhi, boundPhi, nBinEta, boundEta);
}

void JetShape2D::SetObservable
 (fastjet::PseudoJet jet,
  std::vector<std::shared_ptr<Particle>> particle_list,
  int ir, std::vector<std::array<int, 2>> i_j ){
   
   for( auto& p : particle_list ){
     std::vector<std::array<int, 2>> i_p;
     if( ParticleTrigger(p, i_p)){
       
       double pt = sub_ptr->ptSub(p);
       
       double delta_eta = p->eta() - jet.eta() ;
       double delta_phi = jet.delta_phi_to( p->GetPseudoJet() );
       
       for( auto ijet: i_j){
         for( auto iparticle: i_p){
           
           for( int ip = 0; ip < nParams; ip++ ){
             for( int iv = 0; iv < variables.size(); iv++ ){
               
               hist_list[GetHistIndex(iv,ir,ijet[0],ijet[1],iparticle[0],iparticle[1],ip)]->Fill(delta_phi, delta_eta, pt);
               
             }
           }
         }
       }
       
     }// trigger
   }
   
 }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JetShape2D::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  
  std::string hist_name = GetHistName( iv, ir, ijp, ijr, ipp, ipr, ip );
  //std::cout << "[JetShape2D] hist_name = " << hist_name << std::endl;
  
  
  auto total_hist = CreateHist("2d_"+hist_name, iv);
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
  
  if(nJetTotal==0){
    std::cout << "[JetShape2D] 0-total Jet" << std::endl;
    std::cout << "[JetShape2D] Skip. "<< std::endl;
    return;
  }
  
  total_hist->Scale(1.0/nJetTotal);
  total_hist->Print("count_before_sub_");
  
  auto sideband_hist = CreateHist("sideband_"+hist_name, iv);
  sideband_hist->Init();
  sideband_hist->SetSidebandHist(total_hist,sidebandRap[0],sidebandRap[1]);
  sideband_hist->Print();
  
  total_hist->Add( sideband_hist, -1.0 );
  sideband_hist->DeleteTH();
  total_hist->Print("count_");
  

  auto jetshape_hist
  = std::make_shared<Hist1D>(hist_name, binSettings[iv]);
  jetshape_hist->Init();
  
  auto jetshape_err2_hist
  = std::make_shared<Hist1D>("err2_"+hist_name, binSettings[iv]);
  jetshape_err2_hist->Init();
  
  int n_phi = total_hist->GetNbinsX();
  int n_eta = total_hist->GetNbinsY();
  
  for(int i_phi=0; i_phi<n_phi; i_phi++){
    for(int i_eta=0; i_eta<n_eta; i_eta++){
      
      double delta_phi = total_hist->GetX(i_phi);
      double delta_eta = total_hist->GetY(i_eta);
      double val = total_hist->GetVal(i_phi,i_eta);
      double err = total_hist->GetErr(i_phi,i_eta);

      double delta_r = sqrt(delta_phi*delta_phi+delta_eta*delta_eta);

      jetshape_hist->Fill(delta_r, val);
      jetshape_err2_hist->Fill(delta_r, err*err);
      
    }
  }
  total_hist->DeleteTH();
  
  jetshape_hist->SetErrors(jetshape_err2_hist);
  jetshape_err2_hist->DeleteTH();

  jetshape_hist->Scale(1.0,"width");
  jetshape_hist->Print("jetshape_");
  jetshape_hist->Normalize("width");
  jetshape_hist->Print("normalized_jetshape_");
  jetshape_hist->DeleteTH();

}
