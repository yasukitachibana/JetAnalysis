#include "NaiveNegativeSub.h"


// Register the module with the base class
RegisterSubtractionModule<NaiveNegativeSub> NaiveNegativeSub::reg("NaiveNegativeSub");

NaiveNegativeSub::NaiveNegativeSub(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating NaiveNegativeSub" << std::endl;
}

NaiveNegativeSub::~NaiveNegativeSub(){
  std::cout << "-$-Deleting NaiveNegativeSub"<<std::endl;
}

//###############################################################################################################
// Jet
//###############################################################################################################
std::vector < std::shared_ptr<fastjet::PseudoJet> >
NaiveNegativeSub::JetSub(double jetR,
                         std::vector < std::shared_ptr<fastjet::PseudoJet> > jets,
                         std::vector<std::shared_ptr<Particle>> particle_list ){
  
  std::vector<std::shared_ptr<fastjet::PseudoJet>> jets_out;
  
  for( auto &j : jets ){

    double e_j = j->e();
    double px_j = j->px();
    double py_j = j->py();
    double pz_j = j->pz();

    for( auto& p : particle_list ){
      if( p->pstat() == -1 && j->delta_R(p->GetPseudoJet()) <= jetR ){
        e_j -= p->e();
        px_j -= p->px();
        py_j -= p->py();
        pz_j -= p->pz();
      }
  }
  
    j->reset( px_j, py_j, pz_j, e_j );
    jets_out.push_back(j);

  }
  return jets_out;
}

std::vector <fastjet::PseudoJet> NaiveNegativeSub::JetSub(double jetR,
                                                          std::vector <fastjet::PseudoJet> jets,
                                                          std::vector<std::shared_ptr<Particle>> particle_list ){
  
  
  std::vector <fastjet::PseudoJet> jets_out;
  
  for( auto &j : jets ){
    
    double e_j = j.e();
    double px_j = j.px();
    double py_j = j.py();
    double pz_j = j.pz();
    
    for( auto& p : particle_list ){
      if( p->pstat() == -1 && j.delta_R(p->GetPseudoJet()) <= jetR ){
        e_j -= p->e();
        px_j -= p->px();
        py_j -= p->py();
        pz_j -= p->pz();
      }
    }
    
    j.reset( px_j, py_j, pz_j, e_j );
    jets_out.push_back(j);
    
  }
  return jets_out;
}


//###############################################################################################################
// Particles
//###############################################################################################################
double NaiveNegativeSub::ptSub( std::shared_ptr<Particle> particle ){
    
    if( particle->pstat() != -1 ){
        return particle->perp();
    }else{
        return -1.0 * particle->perp();
    }
    
}

double NaiveNegativeSub::nSub( std::shared_ptr<Particle> particle ){
    
    if( particle->pstat() != -1 ){
        return 1.0;
    }else{
        return -1.0;
    }
    
}
