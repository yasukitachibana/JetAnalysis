#include "AnalysisModuleMixedEvent.h"


void AnalysisModuleMixedEvent::InitMixedEvent(){
  
  load_mixed_ptr = LoadFileModuleFactory::createInstance( load_ptr->Name() );
  load_mixed_ptr->Init();
  std::string mixed_event_path = SetXML::Instance()->GetElementText({"observable", Name().c_str(), "pathMixedEvent"});
  std::string mixed_event_filename = SetXML::Instance()->GetElementText({"observable", Name().c_str(), "filenameMixedEvent"});
  
  
  std::string mixed_event_fullpath;
  if(mixed_event_path == "na"){
    mixed_event_fullpath =  SetFile::Instance()->GetInPath(mixed_event_filename);
  }else{
    mixed_event_fullpath = mixed_event_path+'/'+mixed_event_filename;
  }

  load_mixed_ptr->Load(mixed_event_fullpath);
  std::cout << "[AnalysisModuleMixedEvent] Loaded Mixed Event File: " << mixed_event_fullpath << std::endl;

}

void AnalysisModuleMixedEvent::Clear(int seq_loaded){
  std::cout << "[AnalysisModuleMixedEvent] Start Printing Histograms " << std::endl;
  if (seq_loaded)
  {
    for( auto hist: hist_list){
      hist->Print();
    }
  }else{
    std::cout << "[AnalysisModuleMixedEvent] Skip. " << std::endl;
  }
  DeleteHist();
  load_mixed_ptr->Reset();
}

void AnalysisModuleMixedEvent::EventEndMark
 (std::vector<std::shared_ptr<Particle>> &particle_list, int &event_num)
{
  
  if(event_num%2500==0){
    std::cout
    << "Event" << event_num
    <<" ("<<std::to_string(getMemoryUsage())<<"MB) "
    << std::flush;
    if( ((event_num/2500)+1)%5==0 ){
      std::cout<<std::endl;
    }
  }
  
  for( auto hist: hist_list){
    hist->EventCount();
  }
  
  std::vector<std::shared_ptr<Particle>> mixed_event_particle_list = GetMixedEventParticles();

  OneEventAnalysis(particle_list, mixed_event_particle_list);
  particle_list.clear();
  particle_list.shrink_to_fit();
  mixed_event_particle_list.clear();
  mixed_event_particle_list.shrink_to_fit();

  event_num++;
  
}

std::vector<std::shared_ptr<Particle>> AnalysisModuleMixedEvent::GetMixedEventParticles(){
  std::vector<std::shared_ptr<Particle>> mixed_event_particle_list;
  
  while( load_mixed_ptr->GetLine() ){
//    std::cout << "mixed" << std::endl;
//    load_mixed_ptr->ShowLine();
    if( load_mixed_ptr->EventEnd() ){
      //std::cout << "-------------------------" << std::endl;
      break;
    }else if( load_mixed_ptr->ValidLine() ){
      auto p = load_mixed_ptr->GetParticle();
      if( RapidityCut(p) && jet_charged_ptr->Trigger(p) && jet_pstat_ptr->Trigger(p) && jet_pid_ptr->Trigger(p) ){
        mixed_event_particle_list.push_back(p);
      }
    }
  }
  return mixed_event_particle_list;
}



void AnalysisModuleMixedEvent::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list, std::vector<std::shared_ptr<Particle>> mixed_event_particle_list){
  
    for(int ir = 0; ir < jetR.size(); ir++ ){
      double r_cone = jetR[ir];
      int n_jet = 0;
      // Get Jets Reconstructed with Jet Cone Size = r_cone
      std::vector <fastjet::PseudoJet> jets = reco_ptr->JetReco( r_cone, particle_list);
      for(auto j:jets){
  
        for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
          for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
  
            if( JetTrigger(j, ir, ijp, ijr) ){
  
              SetObservable(j, particle_list, mixed_event_particle_list, ir, ijp, ijr );
  
            }//trigger
  
          }
        }
        
        n_jet++;
        if(n_jet == nJetEv ){
          break;
        }
        
      }//jet
    }//jetR
  
}

std::string AnalysisModuleMixedEvent::GetParamsTag( int i ){
  if(i==1){
    return "mixed_ev";
  }else{
    return "";
  }
}
