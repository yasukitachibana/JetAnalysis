#include "ParticleSpec.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<ParticleSpec> ParticleSpec::reg("ParticleSpec");

ParticleSpec::ParticleSpec(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

ParticleSpec::~ParticleSpec(){
  std::cout << "-$-Deleting " << name << std::endl;
}

void ParticleSpec::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list){
  
  
  for( auto& p : particle_list ){
    std::vector<std::array<int, 2>> i_p;
    if( ParticleTrigger(p, i_p)){
      
      for( auto iparticle: i_p){
        for( int iv = 0; iv < variables.size(); iv++ ){
          for( int ir = 0; ir < jetR.size(); ir++ ){
            for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
              for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
                for( int ip = 0; ip < nParams; ip++ ){
                  
                  double pt = sub_ptr->ptSub(p);
                  hist_list[GetHistIndex(iv,ir,ijp,ijr,iparticle[0],iparticle[1],ip)]
                  ->Fill( pt, 1.0 );
                  
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
void ParticleSpec::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){

  std::string hist_name = GetHistName( iv, ir, ijp, ijr, ipp, ipr, ip );
  std::cout << "[ParticleSpec] hist_name = " << hist_name << std::endl;

  
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
  total_hist->Print("particlespec_dNdpt_");
  total_hist->Scale(1.0/delta_rapidity);
  total_hist->Print("particlespec_dNdptdrap_");
  
  total_hist->DeleteTH();


}
