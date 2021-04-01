#include "LegoParticle.h"
//#include "SetXML.h"
//#include "SetFile.h"

#include <vector>

// Initialize static MakeUniqueHelper.here
Pythia8::Pythia LegoParticle::InternalHelperPythia("IntentionallyEmpty", false);

void LegoParticle::Load( std::string filename ){
  
    // Read file
    std::ifstream ifs;
    ifs.open(filename.c_str()); //open the input file
    input_str_stream<< ifs.rdbuf(); //read the file
  
    if (ifs.is_open()){
      ifs.close();
    }else{
      ifs.close();
      std::cout << std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << "[LegoParticle] " << filename << " NOT FOUND" <<std::endl;
      std::cout << "[LegoParticle] Exit. "<< std::endl;
      std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
      std::cout << std::endl;
      exit(-1);
    }

}

std::vector<std::shared_ptr<Particle>> LegoParticle::GetParticles(){
  
  std::vector<std::shared_ptr<Particle>> particle_list;
  std::string str;
  
  int sn = 0;
  int pid = 211;// dummy, charged pion (+) pid
  int status = 0;
  double restmass = 0.0;// dummy
  double pt = 10.0;// dummy
  
  double phi, phi_l, phi_h, eta, eta_l, eta_h, n, err, njet, nev;
  
  while ( getline( input_str_stream, str ) ){
    
    if( str.find("#") == std::string::npos && ! str.empty() ){
      
      sscanf(str.data(),
             "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
             &phi, &phi_l, &phi_h, &eta, &eta_l, &eta_h, &n, &err, &njet, &nev);

      double e = pt*cosh(eta);// massless
      auto particle = std::make_shared<Particle>(sn,pid,status,status,pt,eta,phi,e,n);      
      particle_list.push_back(particle);
      
      sn++;
    }
  }

  return particle_list;
}

void LegoParticle::Clear(){
  input_str_stream.str("");
  input_str_stream.clear(std::stringstream::goodbit);
}
