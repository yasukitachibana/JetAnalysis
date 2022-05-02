#include "CombineEvents.h"

#include <iostream>

CombineEvents::CombineEvents(std::shared_ptr<AnalysisModuleBase> obs_ptr_in)
:obs_ptr(obs_ptr_in){
  std::cout << "-@-Creating CombineEvents for "<< obs_ptr->Name() <<std::endl;
}


CombineEvents::~CombineEvents(){
    std::cout << "-$-Deleting CombineEvents"<<std::endl;
}

void CombineEvents::Init(){
  std::cout << "[CombineEvents] Intialize CombineEvents"<<std::endl;
  ptHat = SetXML::Instance()->GetElementVectorDouble({"ptHat","Item"});
}

void CombineEvents::Clear(){
  obs_ptr->Clear(1);
}

void CombineEvents::Combine(){

  std::cout <<  "---------------------------------------------------------" << std::endl;
  obs_ptr->Combine(ptHat);
  std::cout <<  "---------------------------------------------------------" << std::endl;
  std::cout << "[CombineEvents] Done CombineEvents"<<std::endl;

}

