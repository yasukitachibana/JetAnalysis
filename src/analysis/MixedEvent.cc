#include "MixedEvent.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<MixedEvent> MixedEvent::reg("MixedEvent");



MixedEvent::MixedEvent(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

MixedEvent::~MixedEvent(){
  std::cout << "-$-Deleting " << name << std::endl;
}

int MixedEvent::ReadOptionParametersFromXML(){
  
  
  nBinPhi = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinPhi"});
  boundPhi = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundPhi","Item"});
  
  nBinEta = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinEta"});
  boundEta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundEta","Item"});
    
  return 1;
}

std::shared_ptr<Histogram> MixedEvent::CreateHist( std::string hist_name, int iv ){
  return std::make_shared<Hist2D>(hist_name, nBinPhi, boundPhi, nBinEta, boundEta);
}

void MixedEvent::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list){
  
  for( auto& p : particle_list ){
    std::vector<std::array<int, 2>> i_p;
    if( ParticleTrigger(p, i_p)){
      
      double n = sub_ptr->nSub(p);
      double phi = p->phi_std();
      double eta = p->eta();


      
      for( auto iparticle: i_p){
        for( int iv = 0; iv < variables.size(); iv++ ){
          for( int ir = 0; ir < jetR.size(); ir++ ){
            for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
              for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
                for( int ip = 0; ip < nParams; ip++ ){
                  
                  hist_list[GetHistIndex(iv,ir,ijp,ijr,iparticle[0],iparticle[1],ip)]
                  ->Fill(phi, eta, n);
                  
                }
              }
            }
          }
        }
      }
      
    }//trigger
  }
  
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MixedEvent::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  
  std::string hist_name = GetHistName( iv, ir, ijp, ijr, ipp, ipr, ip );

  auto total_hist = CreateHist(hist_name, iv);
  total_hist->Init();

  double nEv = 0.0;

  for( auto hist: hist_list ){
    double n_ev = hist->Nev();
    if( n_ev != 0 ){
      double sigma = hist->Sigma();
      nEv += sigma;
      total_hist->Add( hist, sigma/n_ev );
    }
  }

  if(nEv==0){
    std::cout << "[MixedEvent] 0-total Jet" << std::endl;
    std::cout << "[MixedEvent] Skip. "<< std::endl;
    return;
  }

  total_hist->Scale(1.0/nEv);
  total_hist->Print();
  total_hist->DeleteTH();

}
