#include "AnalysisModuleStandard.h"


void AnalysisModuleStandard::Analyze(std::string input_file_name){
  
  std::cout
  << "[AnalysisModuleStandard] Analyze " << Name()
  <<" ("<<std::to_string(getMemoryUsage()) <<"MB) ..."
  << std::endl;
  
  load_ptr->Load(input_file_name);
  std::vector<std::shared_ptr<Particle>> particle_list;
  
  int event_num = 0;
  while( load_ptr->GetLine() ){
    
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
  std::cout
  << "\n[AnalysisModuleBase] Last Event" << event_num
  <<" -- DONE! ("<<std::to_string(getMemoryUsage())<<"MB) ..."
  << std::endl;
  //*******************************************************************************************
  load_ptr->Clear();
  
}


void AnalysisModuleStandard::EventEndMark
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
  
  OneEventAnalysis(particle_list);
  particle_list.clear();
  particle_list.shrink_to_fit();
  event_num++;
  
}


void AnalysisModuleStandard::OneEventAnalysis(std::vector<std::shared_ptr<Particle>> particle_list){
  
  for(int ir = 0; ir < jetR.size(); ir++ ){
    double r_cone = jetR[ir];
    // Get Jets Reconstructed with Jet Cone Size = r_cone
    std::vector <fastjet::PseudoJet> jets = reco_ptr->JetReco( r_cone, particle_list);
    for(auto j:jets){
      
      for( int ijp = 0; ijp < jetPtMin.size(); ijp++ ){
        for( int ijr = 0; ijr < jetRapMin.size(); ijr++ ){
          
          if( JetTrigger(j, ir, ijp, ijr) ){
            
            SetObservable(j, particle_list, ir, ijp, ijr );
            
          }//trigger
          
        }
      }
    }//jet
  }//jetR
  
}
