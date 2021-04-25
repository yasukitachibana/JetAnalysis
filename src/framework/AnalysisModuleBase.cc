#include "AnalysisModuleBase.h"
#include "SetXML.h"
#include "SetFile.h"

//###############################################################################################################
// Initialize static MakeUniqueHelper.here
Pythia8::Pythia AnalysisModuleBase::InternalHelperPythia("IntentionallyEmpty",false);
//###############################################################################################################

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
  
  InitMixedEvent();
}

void AnalysisModuleBase::Analyze(std::string input_file_name){
  
  std::cout
  << "[AnalysisModuleBase] Analyze " << Name()
  <<" ("<<std::to_string(getMemoryUsage()) <<"MB) ..."
  << std::endl;
  
  load_ptr->Load(input_file_name);
  std::vector<std::shared_ptr<Particle>> particle_list;
  
  int event_num = 0;
  while( load_ptr->GetLine() ){
//    std::cout << "main" << std::endl;
//    load_ptr->ShowLine();

    if( load_ptr->EventEnd() ){
      //**************
      EventEndMark( particle_list, event_num );
      //**************
    }else if( load_ptr->ValidLine() ){
      //**************
      auto p = load_ptr->GetParticle();
      
      //std::cout << p->pid() << " " << <<std::endl;
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
  std::cout
  << "\n[AnalysisModuleBase] Last Event" << event_num
  <<" -- DONE! ("<<std::to_string(getMemoryUsage())<<"MB) ..."
  << std::endl;
  //*******************************************************************************************
  load_ptr->Clear();
  
}


void AnalysisModuleBase::Set(double ptHatMin, double ptHatMax){
  GenerateHist(ptHatMin, ptHatMax);
}

void AnalysisModuleBase::Clear(){
  for( auto hist: hist_list){
    hist->Print();
  }
  DeleteHist();
}

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
  
  CombineFinisher();
  
}

//###############################################################################################################

void AnalysisModuleBase::ReadParametersFromXML(){
  //###############################################################################################################
  nJetEv = SetXML::Instance()->GetElementInt({"jetReco","nJetEv"});
  jetR = SetXML::Instance()->GetElementVectorDouble({"jetReco","jetR","Item"});
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

//###############################################################################################################
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
  double factor = 0.6;
  if(reco_ptr->Name()=="NegativeReco"){
    factor = 1.0;
  }
  jetPtCut = factor*jetPtCut;
  reco_ptr->SetJetPtCut(jetPtCut);
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
                
                //
              }//option parameters
            }//had_rap
          }//had_pt
        }//jet_rap
      }//jet_pt
    }//jetR
  }//variable
  
  std::cout << "[AnalyzeBase] number of generated histogram: " << nHist << std::endl;
  
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

void AnalysisModuleBase::DeleteHist(){
  for( auto hist: hist_list ){
    hist->DeleteTH();
  }
  hist_list.clear();
  hist_list.shrink_to_fit();
}

std::string AnalysisModuleBase::GetHistName( double ptHatMin, double ptHatMax,
                                            int iv,int ir,int ijp, int ijr,int ipp,int ipr,int ip){
  
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
  return nParams*(particleRapMin.size()*(particlePtMin.size()*(jetRapMin.size()*(jetPtMin.size()*(jetR.size()*(iv)+ir)+ijp)+ijr)+ipp)+ipr)+ip;
}

std::shared_ptr<Histogram> AnalysisModuleBase::CreateHist( std::string hist_name, int iv ){
  return std::make_shared<Hist1D>(hist_name, binSettings[iv]);
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
bool AnalysisModuleBase::ChargedCheck(std::shared_ptr<Particle> p){
  double charge = InternalHelperPythia.particleData.charge( p->pid() );
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
bool AnalysisModuleBase::StatCheck(std::shared_ptr<Particle> p){
  
  for(auto st:statParticle){
    if( st==p->pstat() ){
      return true;
    }
  }
  
  return false;
}
//###############################################################################################################
bool AnalysisModuleBase::JetTrigger(fastjet::PseudoJet jet, int ir, int ijp, int ijr ){
  
  double pt_jet = jet.perp();
  double rapidity_jet = GetRapidity(jet);
  
  if( pt_jet >= jetPtMin[ijp] &&
     pt_jet < jetPtMax[ijp] &&
     fabs(rapidity_jet) >= jetRapMin[ijr] &&
     fabs(rapidity_jet) < jetRapMax[ijr] )
  {
    return true;
  }//trigger
  
  return false;
}

bool AnalysisModuleBase::ParticleTrigger(std::shared_ptr<Particle> p, int ipp, int ipr){
  
  if( ChargeTrigger(p,chParticle) && StatCheck(p) ){
    
    double pt = p->perp();
    double rapidity = GetRapidity(p);
    
    if( pt >= particlePtMin[ipp] && pt < particlePtMax[ipp] &&
       fabs(rapidity) >= particleRapMin[ipr] && fabs(rapidity) < particleRapMax[ipr] ){
      return true;
    }
    
  }
  return false;
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
