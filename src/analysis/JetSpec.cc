#include "JetSpec.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetSpec> JetSpec::reg("JetSpec");



JetSpec::JetSpec(std::string name_in): name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetSpec::~JetSpec(){
  std::cout << "-$-Deleting " << name << std::endl;
}

void JetSpec::SetObservable
 (fastjet::PseudoJet jet,
  std::vector<std::shared_ptr<Particle>> particle_list,
  int ir, std::vector<std::array<int, 2>> i_j ){
   
   for( auto ij: i_j){
//     for( int i_had_pt = 0; i_had_pt < hadPtMin.size(); i_had_pt++ ){
//       for( int i_had_rap = 0; i_had_rap < hadRapMin.size(); i_had_rap++ ){
//         
//         GetHist(i_r_jet,ij[0],ij[1],i_had_pt,i_had_rap,0)->Fill( jet.perp(), 1.0 );
//         
//       }
//     }
   }
      
  
}
