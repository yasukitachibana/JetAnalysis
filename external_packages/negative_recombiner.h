#include "fastjet/ClusterSequence.hh"
#include <iostream> // needed for io
#include <cstdio>   // needed for io
#include <fstream>
#include <math.h>
#include <vector>


using namespace std;
using namespace fastjet;

typedef fastjet::JetDefinition::Recombiner Recombiner;
/// Recombiner class that propagates the user index and arranges the
/// recombination accordingly
class NegativeEnergyRecombiner : public  Recombiner {
public:
  NegativeEnergyRecombiner(const int ui) : _ui(ui) {}
  
  virtual std::string description() const {return "E-scheme Recombiner that checks a flag for a 'negative momentum' particle, and subtracts the 4-momentum in recombinations.";}
  
  /// recombine pa and pb and put result into pab
  virtual void recombine(const fastjet::PseudoJet & pa,
                         const fastjet::PseudoJet & pb,
                         fastjet::PseudoJet & pab) const {
    
    int ai=1,bi=1;
    
    // If a particle is flagged, restore its real negative energy.
    // The -1 will flip the full 4-momentum, reversing the convention for
    // negative energy particles.
    if (pa.user_index() < 0) { ai = -1;}
    if (pb.user_index() < 0) { bi = -1;}
    
    // recombine particles
    pab = ai*pa+bi*pb;
    
    // if the combination has negative energy, flip the whole 4-momentum and flag it,
    // so that we have the same convention as for input particles
    if(pab.E() < 0) {
      pab.set_user_index(_ui);
      pab.reset_momentum(-pab.px(),
                         -pab.py(),
                         -pab.pz(),
                         -pab.E());
      //        cout << "XX3 " << pab.E()  << " " << tmppa.E() << " " << tmppb.E() << endl;
    } else { pab.set_user_index(0);}
    
  }
  
private:
  const int _ui;
};
