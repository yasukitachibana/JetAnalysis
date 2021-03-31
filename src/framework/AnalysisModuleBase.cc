#include "AnalysisModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// AnalysisModuleFactory, RegisterAnalysisModule
//###############################################################################################################
// Create an instance of the static map to register modules
AnalysisModuleFactory::map_type *AnalysisModuleFactory::moduleMap =
new AnalysisModuleFactory::map_type;
//###############################################################################################################





//###############################################################################################################
// AnalysisModuleBase
//###############################################################################################################
void AnalysisModuleBase::Init(std::shared_ptr<ReconstructionModuleBase> reco_ptr_in,
                              std::shared_ptr<SubtractionModuleBase> sub_ptr_in,
                              std::shared_ptr<LoadFileModuleBase> load_ptr_in){
  
  std::cout << "[AnalyzeBase] Intialize " << Name() <<std::endl;
  
  reco_ptr = nullptr;
  sub_ptr = nullptr;
  load_ptr = nullptr;
  
  reco_ptr=reco_ptr_in;
  sub_ptr=sub_ptr_in;
  load_ptr=load_ptr_in;
  
  ReadParametersFromXML();
  SetJetPtCut();
  SetLargestRapidity();
}

void AnalysisModuleBase::ReadParametersFromXML(){
  
  //###############################################################################################################
  jetR = SetXML::Instance()->GetElementVectorDouble({"jetReco","jetR","Item"});
  //###############################################################################################################
  chJet = SetXML::Instance()->GetElementInt({"jetReco","chJet"});
  statJet = SetXML::Instance()->GetElementVectorInt({"jetReco", "statJet", "Item"});
  jetRapidity = SetXML::Instance()->GetElementInt({"jetReco","jetRapidity"});
  jetRapMin = SetXML::Instance()->GetElementVectorDouble({"jetReco","jetRapMin","Item"});
  jetRapMax = SetXML::Instance()->GetElementVectorDouble({"jetReco","jetRapMax","Item"});
  jetPtMin = SetXML::Instance()->GetElementVectorDouble({"jetReco","jetPtMin","Item"});
  jetPtMax = SetXML::Instance()->GetElementVectorDouble({"jetReco","jetPtMax","Item"});
  //###############################################################################################################
  chParticle  = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "chParticle"});
  statParticle = SetXML::Instance()->GetElementVectorInt({"observable", Name().c_str(), "statParticle", "Item"});
  particleRapidity = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "particleRapidity"});
  particleRapMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particleRapMin","Item"});
  particleRapMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particleRapMax","Item"});
  particlePtMin = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particlePtMin","Item"});
  particlePtMax = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "particlePtMax","Item"});
  //###############################################################################################################
  variables = SetXML::Instance()->GetElementNameVector({"observable", Name().c_str(), "var"});
  
  for( auto var : variables ){
    std::vector<double> bins = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "var", var.c_str(), "Item"});
    binSettings.push_back(bins);
  }
  
  //###############################################################################################################
  nParams = ReadOptionParametersFromXML();
  //###############################################################################################################
  
  //###############################################################################################################
  std::cout << std::endl;
  std::cout << "[AnalyzeBase] **********************************************"<< std::endl;
  std::cout << "[AnalyzeBase] *** Settings"<< std::endl;
  ShowObservableSetting();
  ShowJetSetting();
  ShowParticleSetting();
  ShowParamsSetting();
  std::cout << "[AnalyzeBase] **********************************************"<< std::endl;
  std::cout << std::endl;
  //###############################################################################################################
  
  if(chJet == 1 && chParticle == 0 ){
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[AnalyzeBase] Bad Combination: Charged Jet and Charged+Neutral Particles" << std::endl;
    std::cout << "[AnalyzeBase] Exit. "<< std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
  
}

void AnalysisModuleBase::ShowObservableSetting(){
  std::cout << "[AnalyzeBase] ***-------------------------------------------"<< std::endl;
  std::cout << "[AnalyzeBase] *** [" << Name()<< "]" << std::endl;
  std::cout << "[AnalyzeBase] *** variables: ";
  for( auto var : variables ){
    std::cout << var << ", ";
  }
  std::cout << "\b\b  "<< std::endl;
}

void AnalysisModuleBase::ShowJetSetting(){
  std::cout << "[AnalyzeBase] ***-------------------------------------------"<< std::endl;
  std::cout << "[AnalyzeBase] *** [Jet]"<< std::endl;
  std::cout << "[AnalyzeBase] *** R_jet=";
  for( auto r_cone : jetR ){
    std::cout << r_cone << ", ";
  }
  std::cout << JetType() << ", Status = ";
  for( auto st : statJet ){ std::cout << st << ", ";}
  std::cout << "\b\b  " << std::endl;
  for( int i = 0; i < jetPtMin.size(); i++ ){
    std::cout << "[AnalyzeBase] *** " << jetPtMin[i] << " < pt_jet < " << jetPtMax[i] << " GeV" << std::endl;
  }
  for( int i = 0; i < jetRapMin.size(); i++ ){
    std::cout << "[AnalyzeBase] *** " << jetRapMin[i] << " < |" << JetRapType() << "_jet| < " << jetRapMax[i] << std::endl;
  }
}

void AnalysisModuleBase::ShowParticleSetting(){
  std::cout << "[AnalyzeBase] ***-------------------------------------------"<< std::endl;
  std::cout << "[AnalyzeBase] *** [Particles]"<< std::endl;
  std::cout << "[AnalyzeBase] *** "<< ParticleType() << ", Status=";
  for( auto st : statParticle ){ std::cout << st << ", ";}
  std::cout << "\b\b  " << std::endl;
  for( int i = 0; i < particlePtMin.size(); i++ ){
    std::cout << "[AnalyzeBase] *** " << particlePtMin[i] << " < pt < " << particlePtMax[i] << " GeV" << std::endl;
  }
  for( int i = 0; i < particleRapMin.size(); i++ ){
    std::cout << "[AnalyzeBase] *** " << particleRapMin[i] << " < |" << ParticleRapType() << "| < " << particleRapMax[i] << std::endl;
  }
}

void  AnalysisModuleBase::SetLargestRapidity(){
  largestRapidity=0.0;
  double rap;
  double jetRmax = *max_element(jetR.begin(), jetR.end());
  
  for( int i_jet_rap = 0; i_jet_rap < jetRapMax.size(); i_jet_rap++ ){
    rap = jetRapMax[i_jet_rap]+1.2*jetRmax;
    if( largestRapidity < rap ){
      largestRapidity = rap;
    }
  }
  
  for( int i_particle_rap = 0; i_particle_rap < particleRapMax.size(); i_particle_rap++ ){
    rap = particleRapMax[i_particle_rap];
    if( largestRapidity < rap ){
      largestRapidity = rap;
    }
  }
  std::cout << "[AnalyzeBase] Largest Rapidity: " << largestRapidity<< std::endl;
}


//###############################################################################################################
void AnalysisModuleBase::Set(double ptHatMin, double ptHatMax){
  GenerateHist(ptHatMin, ptHatMax);
}

void AnalysisModuleBase::Clear(){
  for( auto hist: hist_list){
    hist->Print();
  }
  DeleteHist();
}

void AnalysisModuleBase::DeleteHist(){
  for( auto hist: hist_list ){
    hist->DeleteTH();
  }
  hist_list.clear();
  hist_list.shrink_to_fit();
}

//###############################################################################################################

void AnalysisModuleBase::GenerateHist(double ptHatMin, double ptHatMax){
  
  int nHist = 0;
  
  for( int iv = 0; iv < variables.size(); iv++ ){
    for( int ir = 0; ir < jetR.size(); ir++ ){
      for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
        for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
          for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
            for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
              for( int ip = 0; ip < nParams; ip++ ){
                
                std::string hist_name = GetHistName( ptHatMin, ptHatMax, iv, ir, ijp, ijr, ipp, ipr, ip );
                //std::cout << "[AnalyzeBase] generate histogram #" << nHist << " " << hist_name << std::endl;
                auto hist_this_bin = CreateHist(hist_name, iv);
                hist_this_bin->Init();
                hist_list.push_back(hist_this_bin);
                nHist ++;
                //std::string hist_name_test = hist_list[GetHistIndex(iv,ir,ijp,ijr,ipp,ipr,ip)]->HistName();
                //if(hist_name != hist_name_test){ std::cout << "Error!! Exit." << std::endl; exit(-1);}
                
              }//option parameters
            }//had_rap
          }//had_pt
        }//jet_rap
      }//jet_pt
    }//jetR
  }//variable
  std::cout << "[AnalyzeBase] number of generated histogram: " << nHist << std::endl;
  
}



std::shared_ptr<Histogram> AnalysisModuleBase::CreateHist( std::string hist_name, int iv ){
  return std::make_shared<Hist1D>(hist_name, binSettings[iv]);
}



//###############################################################################################################

std::string AnalysisModuleBase::GetHistName( double ptHatMin, double ptHatMax,
                                            int iv,
                                            int ir,
                                            int ijp, int ijr,
                                            int ipp, int ipr,
                                            int ip){
  
  std::string tag = GetParamsTag( ip );
  
  std::string histname
  = SetFile::Instance()->GetHistName(ptHatMin, ptHatMax,
                                     Name(),
                                     variables[iv],
                                     jetR[ir],
                                     jetPtMin[ijp], jetPtMax[ijp],
                                     jetRapMin[ijr], jetRapMax[ijr],
                                     particlePtMin[ipp], particlePtMax[ipp],
                                     particleRapMin[ipr], particleRapMax[ipr], tag);
  return histname;
}

std::string AnalysisModuleBase::GetHistName( int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  
  std::string tag = GetParamsTag( ip );
  
  std::string histname
  = SetFile::Instance()->GetHistName(Name(),
                                     variables[iv],
                                     jetR[ir],
                                     jetPtMin[ijp], jetPtMax[ijp],
                                     jetRapMin[ijr], jetRapMax[ijr],
                                     particlePtMin[ipp], particlePtMax[ipp],
                                     particleRapMin[ipr], particleRapMax[ipr], tag);
  return histname;
}


int AnalysisModuleBase::GetHistIndex( int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip){
  return
  nParams*
  (particleRapMin.size()*
   (particlePtMin.size()*
    (jetRapMin.size()*
     (jetPtMin.size()*
      (jetR.size()*
       (iv)
       +ir)
      +ijp)
     +ijr)
    +ipp)
   +ipr)
  +ip;
}

//###############################################################################################################
void AnalysisModuleBase::Analyze(std::string input_file_name){
  
  std::cout << "[AnalysisModuleBase] Analyze " << Name() <<" ("<<std::to_string(getMemoryUsage())<<"MB) ..."<< std::endl;
  //std::cout << "[AnalysisModuleBase] check hist name " <<  hist_list[0]->HistName() << std::endl;
  load_ptr->Load(input_file_name);
  //*******************************************************************************************
  std::vector<std::shared_ptr<Particle>> particle_list;
  
  int event_num = 0;
  while( load_ptr->GetLine() ){
    //load_ptr->ShowLine();
    if( load_ptr->EventEnd() ){
      //**************
      EventEndMark( particle_list, event_num );
      //**************
    }else if( load_ptr->ValidLine() ){
      //**************
      auto p = load_ptr->GetParticle();
      if( RapidityCut(p) && ChargeTrigger(p, chJet) && (!NeutrinoCheck(p)) ){
        particle_list.push_back(p);
      }
      //**************
    }
    
  }
  if( load_ptr->Last() ){
    //**************
    EventEndMark( particle_list, event_num );
    //**************
  }
  std::cout << "\n[AnalysisModuleBase] Last Event" << event_num <<" -- DONE! ("<<std::to_string(getMemoryUsage())<<"MB) ..."<< std::endl;
  //*******************************************************************************************
  load_ptr->Clear();
}

//###############################################################################################################
void AnalysisModuleBase::EventEndMark
(std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num)
{
  
  if(event_num%2500==0){
    std::cout
    << "Event" << event_num
    <<" ("<<std::to_string(getMemoryUsage())<<"MB) "<< std::flush;
    if( ((event_num/2500)+1)%5==0 ){
      std::cout<<std::endl;
    }
  }
  
  for( auto hist: hist_list){
    hist->EventCount();
  }
  
  OneEventAnalysis(particle_list);
  particle_list.clear();
  particle_list.shrink_to_fit();
  event_num++;
}

//###############################################################################################################
void AnalysisModuleBase::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list){
  
  for(int ir = 0; ir < jetR.size(); ir++ ){
    double r_cone = jetR[ir];
    // Get Jets Reconstructed with Jet Cone Size = r_cone
    std::vector <fastjet::PseudoJet> jets = reco_ptr->JetReco( r_cone, particle_list);
    for(auto j:jets){
      std::vector<std::array<int, 2>> i_j;
      if( JetTrigger(j, i_j, ir) ){
        
        SetObservable(j, particle_list, ir, i_j);
        
      }//trigger
    }//jet
  }//jetR
  
}

//###############################################################################################################
bool AnalysisModuleBase::JetTrigger(fastjet::PseudoJet jet, std::vector<std::array<int, 2>> &i_j, int ir ){
  
  double pt_jet = jet.perp();
  double rapidity_jet = GetRapidity(jet);
  bool trigger = false;
  for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
    for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
      
      if( pt_jet >= jetPtMin[ijp] &&
         pt_jet < jetPtMax[ijp] &&
         fabs(rapidity_jet) >= jetRapMin[ijr] &&
         fabs(rapidity_jet) < jetRapMax[ijr] ){
        
        std::array<int, 2> ij = {ijp,ijr};
        i_j.push_back(ij);
        trigger = true;
        //-----------------------------------------------------------------------------------------------------
        for( int iv = 0; iv < variables.size(); iv++ ){
          for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
            for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
              for( int ip = 0; ip < nParams; ip++ ){
                hist_list[GetHistIndex(iv,ir,ijp,ijr,ipp,ipr,ip)]
                ->JetTriggered();
              }//option
            }//particle_rap
          }//particle_pt
        }//variable
        //-----------------------------------------------------------------------------------------------------
      }//trigger
    }//jet_rap
  }//jet_pt
  
  return trigger;
}

bool AnalysisModuleBase::ParticleTrigger(std::shared_ptr<Particle> p, std::vector<std::array<int, 2>> &i_p ){
  
  if( !StatCheck(p) ){ return false; }
  if( !ChargeTrigger(p,chParticle) ){ return false; }
  
  bool trigger = false;
  double pt = p->perp();
  double rapidity = GetRapidity(p);
  
  for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
    for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
      
      if( pt >= particlePtMin[ipp] && pt < particlePtMax[ipp] &&
         fabs(rapidity) >= particleRapMin[ipr] && fabs(rapidity) < particleRapMax[ipr] ){
        
        std::array<int, 2> iparticle = {ipp,ipr};
        i_p.push_back(iparticle);
        
        trigger = true;
      }
      
      
    }//rap
  }//pt
  
  return trigger;
}

//###############################################################################################################
bool AnalysisModuleBase::StatCheck(std::shared_ptr<Particle> p){
  
  for(auto st:statParticle){
    if( st==p->pstat() ){
      return true;
    }
  }
  
  return false;
}

//###############################################################################################################
bool AnalysisModuleBase::NeutrinoCheck( std::shared_ptr<Particle> p ){
  int pid = p->pid();
  for(int i =0; i < nNeutrino; i++){
    if( abs(pid) == pidNeutrino[i] ){
      return true;
    }
  }
  return false;
}

//###############################################################################################################
bool AnalysisModuleBase::ChargedCheck(std::shared_ptr<Particle> p){
  double charge = pythia.particleData.charge( p->pid() );
  if( fabs( charge ) > 0.0001 /* min. value of charge */  ){
    return true;
  }else{
    return false;
  }
}

bool AnalysisModuleBase::ChargeTrigger(std::shared_ptr<Particle> p, int charged){
  if( charged==0 || ChargedCheck(p) ){
    return true;
  }else{
    return false;
  }
}

//###############################################################################################################
bool AnalysisModuleBase::RapidityCut( std::shared_ptr<Particle> p ){
  
  double rapidity = GetRapidity(p);
  
  if( fabs(rapidity) <= largestRapidity ){
    return true;
  }else{
    return false;
  }
  
}

double AnalysisModuleBase::GetRapidity( fastjet::PseudoJet j ){
  if( particleRapidity == 0){
    return j.rapidity();
  }else{
    return j.eta();
  }
}

double AnalysisModuleBase::GetRapidity( std::shared_ptr<fastjet::PseudoJet> j ){
  if( particleRapidity == 0){
    return j->rapidity();
  }else{
    return j->eta();
  }
}

double AnalysisModuleBase::GetRapidity( std::shared_ptr<Particle> p ){
  return GetRapidity( p->GetPseudoJet() );
}

//###############################################################################################################
void AnalysisModuleBase::SetJetPtCut(){
  double jetPtCut= 10000.0;
  double pt = 0.0;
  
  for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
    pt = jetPtMin[ijp];
    if( jetPtCut > pt ){
      jetPtCut = pt;
    }
  }
  jetPtCut = 0.6*jetPtCut;
  reco_ptr->SetJetPtCut(jetPtCut);
}

//###############################################################################################################
void AnalysisModuleBase::Combine(std::vector<double> ptHat){
  
  for( int iv = 0; iv < variables.size(); iv++ ){
    for( int ir = 0; ir < jetR.size(); ir++ ){
      for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
        for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
          for( int ipp = 0; ipp < particlePtMin.size(); ipp++ ){
            for( int ipr = 0; ipr < particleRapMin.size(); ipr++ ){
              for( int ip = 0; ip < nParams; ip++ ){
                
                //_____________________________________________________________________________________
                int nPtHatBins = ptHat.size()-1;
                for(int i_pthat_bin = 0; i_pthat_bin < nPtHatBins; i_pthat_bin++ ){
                  LoadHist( ptHat[i_pthat_bin], ptHat[i_pthat_bin+1], iv, ir, ijp, ijr, ipp, ipr, ip );
                }
                //_____________________________________________________________________________________
                CombineHist( iv, ir, ijp, ijr, ipp, ipr, ip );
                DeleteHist();
                
              }//option parameters
            }//had_rap
          }//had_pt
        }//jet_rap
      }//jet_pt
    }//jetR
  }//variable
  
}


void AnalysisModuleBase::LoadHist( double ptHatMin, double ptHatMax,
                                  int iv,int ir,int ijp,int ijr,int ipp,int ipr,int ip){
  //*******************************************************************************************
  std::string hist_name = GetHistName( ptHatMin, ptHatMax, iv, ir, ijp, ijr, ipp, ipr, ip );
  auto hist_this_bin = CreateHist(hist_name, iv);
  hist_this_bin->Init();
  hist_this_bin->LoadHistFromFile();
  //*******************************************************************************************
  double sigma, sigma_err;
  std::string sigma_file_name =  SetFile::Instance()->GetSigmaFileName(ptHatMin, ptHatMax);
  std::cout << "[AnalysisModuleBase] Load Sigma File: " << sigma_file_name << std::endl;
  load_ptr->LoadSigma(sigma_file_name, sigma, sigma_err);
  std::cout << "[AnalysisModuleBase] Sigma = " << sigma << ", sigma error = " << sigma_err << std::endl;
  hist_this_bin->SetSigma(sigma, sigma_err);
  //*******************************************************************************************
  hist_list.push_back(hist_this_bin);
}


//###############################################################################################################
long AnalysisModuleBase::getMemoryUsage()
{
  struct rusage usage;
  // NOTE: Reported in kB on BSD/Linux, bytes in Mac/Darwin
  // Could try to explicitly catch __linux__ as well
  float mbsize = 1024;
#ifdef __MACH__
  mbsize = 1024 * 1024;
#endif
  
  if(0 == getrusage(RUSAGE_SELF, &usage))
    return usage.ru_maxrss/mbsize;
  else
    return 0;
}
