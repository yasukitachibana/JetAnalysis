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
  ReadOptionParametersFromXML();
  //###############################################################################################################
  
  //###############################################################################################################
  std::cout << std::endl;
  std::cout << "[AnalyzeBase] **********************************************"<< std::endl;
  std::cout << "[AnalyzeBase] *** Settings"<< std::endl;
  ShowObservableSetting();
  ShowJetSetting();
  ShowParticleSetting();
  ShowOptionSetting();
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
  InitHist();
}

void AnalysisModuleBase::Clear(){
  
}

void AnalysisModuleBase::GenerateHist(double ptHatMin, double ptHatMax){
}

void AnalysisModuleBase::InitHist(){
  for( auto hist: hist_list ){
    //hist->Init();
  }
}


//###############################################################################################################
void AnalysisModuleBase::Analyze(std::string input_file_name){
  
}
//###############################################################################################################

