#include "Histogram.h"


Histogram::Histogram(std::string histname_in): histname(histname_in){
  //std::cout << "-@-Creating Histogram"<<std::endl;
}

Histogram::~Histogram(){
  //std::cout << "-$-Deleting Histogram"<<std::endl;
}


std::string Histogram::HistName(){
  return histname;
}

void Histogram::Init(){
  nJet = 0.0;
  nEv = 0.0;
  InitHist();
}


void Histogram::Print(){
  Print("");
}



void Histogram::SetSigma(double sigma_in, double sigma_err_in){
  sigma = sigma_in;
  sigma_err = sigma_err_in;
}

double Histogram::GetNjetSigmaOverEev(){
  return sigma*nJet/nEv;
}
















