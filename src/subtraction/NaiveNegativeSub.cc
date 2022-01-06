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
    
    double e_sub=0.0, px_sub=0.0, py_sub=0.0, pz_sub=0.0;
    for( auto& p : particle_list ){
      if( p->pstat() == -1 && j.delta_R(p->GetPseudoJet()) <= jetR ){
        e_sub += p->e();
        px_sub += p->px();
        py_sub += p->py();
        pz_sub += p->pz();
      }
    }
    
    if( e_sub > DBL_EPSILON ){
      double e_j = j.e()-e_sub;
      double px_j = j.px()-px_sub;
      double py_j = j.py()-py_sub;
      double pz_j = j.pz()-pz_sub;
      j.reset( px_j, py_j, pz_j, e_j );
    }
    
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

double NaiveNegativeSub::eSub( std::shared_ptr<Particle> particle ){
  
  if( particle->pstat() != -1 ){
    return particle->e();
  }else{
    return -1.0 * particle->e();
  }
  
}

double NaiveNegativeSub::pzSub( std::shared_ptr<Particle> particle ){
  
  if( particle->pstat() != -1 ){
    return particle->pz();
  }else{
    return -1.0 * particle->pz();
  }
  
}

double NaiveNegativeSub::chargeSub( std::shared_ptr<Particle> particle )
{
  if( particle->pstat() != -1 ){
    return particle->charge();
  }else{
    return -1.0 * particle->charge();
  }
}
  
double NaiveNegativeSub::baryonSub( std::shared_ptr<Particle> particle )
{
  if( particle->pstat() != -1 ){
    return particle->baryon();
  }else{
    return -1.0 * particle->baryon();
  }
}

double NaiveNegativeSub::strangeSub( std::shared_ptr<Particle> particle )
{
  if( particle->pstat() != -1 ){
    return particle->strange();
  }else{
    return -1.0 * particle->strange();
  }
}

double NaiveNegativeSub::charmSub( std::shared_ptr<Particle> particle )
{
  if( particle->pstat() != -1 ){
    return particle->charm();
  }else{
    return -1.0 * particle->charm();
  }
}
