
/// run it with    : ./01-basic < data/single-event.dat

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




/// an example program showing how to use fastjet
int main(){
  int nevent,tstep,nt;
  double R;
  string myfile,outfile,fastin;
  // input from fast.in
  ifstream indatafile("./fast.in");
  
  indatafile >> fastin;
  cout << fastin << endl;
  indatafile >> R >> nevent >> nt;
  cout << R << " " << nevent << " " << nt << endl;
  
  indatafile >> fastin;
  cout << fastin << endl;
  indatafile >> myfile;
  cout << myfile << endl;
  
  indatafile >> fastin;
  cout << fastin << endl;
  indatafile >> outfile;
  cout << outfile << endl;
  
  ifstream datafile;
  ofstream histo;
  datafile.open(myfile.c_str());
  histo.open(outfile.c_str());
  //data array
  vector<double> dEall(nt), dEjr(nt), dEj(nt), dpt(nt), dptp(nt);
  vector< vector<double> > dnde(50,vector<double> (nt)), dndxe(50,vector<double> (nt)), dndxpl(50,vector<double> (nt)),dndxpl0(50,vector<double> (nt)); // Ejet, Ejet/Egm, Plpart/Ejet; // Ejet, Ejet/Egm, Plpart/Ejet
  vector< vector<double> > dndze(50,vector<double> (nt)); // Epart/Ejet
  vector< vector<double> > dndptp(30,vector<double> (nt)); // pt dis. of partons wrt. gamma
  vector< vector<double> > dndphi(50,vector<double> (nt)); // angle wrt. gamma
  // additive by myself
  vector< vector<double> > dndphi2(50,vector<double> (nt)); // angle for mach cone
  vector< vector<double> > dndthe(50,vector<double> (nt)); // theta of partons in leading jet wrt. initial single jet parton
  vector< vector<double> > dndpt1(100,vector<double> (nt)); // pt of partons in leading jet wrt. initial single jet parton,pt=sqrt(px**2+pz**2)
  vector< vector<double> > dndpt2(100,vector<double> (nt)); // pt of partons in leading jet wrt. initial single jet parton,pt=inclusive_jets.pt
  //
  vector< vector<double> > rho(10,vector<double> (nt)), rhoo(10,vector<double> (2)); // jet shape
  vector< vector<double> > dndxpl1(20,vector<double> (2)), dndxpl2(20,vector<double> (2)); // jet fragmentation function plpart/Ejet at different x range: 0-0.5-2
  vector<int> ndnde(nt), ndndxe(nt), ndndze(nt), ndndxpl1(2), ndndxpl2(2), ndndptp(nt);
  
  // read data from myfile
  // with nevent events
  //----------------------------------------------------------
  for (unsigned int i=0; i<nevent; i++){
    double etag, phig, Eg, Ej0;
    int ui = -123456;
    
    // gammas'  and jet partons' infromation
    datafile >> etag >> phig >> Eg;
    cout<< i << endl;
    for (unsigned int j=0; j<nt; j++){
      vector<fastjet::PseudoJet> input_particles;
      double ti;
      int npart;
      datafile >> ti >> npart;
      cout << ti << "  " << npart <<endl;
      
      vector<double> px(npart), py(npart), pz(npart), E(npart);
      vector<int> pid(npart);
      for (unsigned int k0=0; k0<npart; k0++){
        datafile >> px[k0] >> py[k0] >> pz[k0] >> E[k0] >> pid[k0];
        //exchange axis z with axis y
        double yz,zy;
        yz=pz[k0];
        zy=py[k0];
        py[k0]=yz;
        pz[k0]=-zy;
        //
        //        cout <<  px[k0] << " " << py[k0] << " " << pz[k0] << " " << E[k0] << " " << pid[k0] << endl;
        
        // If an energy is negative flip the four-momentum, but flag the particle
        // This corresponds to Xin-Nian Wang' convention for input particles: when the energy
        // is negative, the whole 4 momentum is flipped
        fastjet::PseudoJet pj;
        if ( E[k0] < 0 ) { pj.reset_momentum(-px[k0],-py[k0],-pz[k0],-E[k0]);
          pj.set_user_index(-k0); }
        else { pj.reset_momentum(px[k0],py[k0],pz[k0],E[k0]);
          pj.set_user_index(k0);
        }
        
        // add particle to input_particles
        input_particles.push_back(pj);
      }
      
      
      // create a jet definition:
      // a jet algorithm with a given radius parameter
      //----------------------------------------------------------
      //double R = 0.3;
      fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);
      // create an instance of the negative energy recombiner, with a given flag ui
      NegativeEnergyRecombiner uir(ui);
      // tell jet_def to use this new recombiner
      jet_def.set_recombiner(&uir);
      
      
      // run the jet clustering with the above jet definition
      //----------------------------------------------------------
      fastjet::ClusterSequence clust_seq(input_particles, jet_def);
      
      
      // get the resulting jets ordered in pt
      //----------------------------------------------------------
      double ptmin = 0.0;
      vector<fastjet::PseudoJet> inclusive_jets = sorted_by_pt(clust_seq.inclusive_jets(ptmin));
      //      if (j == 0)Ej0 = inclusive_jets[0].E(); // Initial jet energy
      Ej0 = -Eg; // Initial jet energy
      //        cout << "Ej0=" << Ej0;
      //      if (Ej0 ==0) cout << "jet energy 0! "<< i << " " << j << " " <<inclusive_jets.size()<<endl;
      //      cout << Ej0 << endl;
      
      // tell the user what was done
      //  - the description of the algorithm used
      //  - extract the inclusive jets with pt > 5 GeV
      //    show the output as
      //      {index, rap, phi, pt}
      //----------------------------------------------------------
      //      cout << "Ran " << jet_def.description() << endl;
      
      // label the columns
      //      printf("%5s %12s %12s %12s %12s %12s\n","jet #", "rapidity", "phi", "pt", "E", "info");
      
      // print out the details for each jet
      //      for (unsigned int k = 0; k < inclusive_jets.size(); k++) {
      // choose the leading jet with positive energy
      for (unsigned int k = 0; k < 1; k++) {
        if (inclusive_jets[k].user_index()>=0) {
          
          if (inclusive_jets[k].E()<200.0) {
            unsigned int k1 = int(inclusive_jets[k].E()/(200.0/50.0));
            if (k1 >= 50) k1 = 49;
            dnde[k1][j] += 50.0/200.0;
            //             cout << k1 << " " << j << " " << dnde[k1][j] << " " << dnde[k1+6][j] << endl;
            ndnde[j] += 1;
          }
          if (int(inclusive_jets[k].pt()/(Eg/cosh(etag)))<2.0) {
            unsigned int k1 = int(inclusive_jets[k].pt()/(Eg/cosh(etag))/(2.0/50.0));
            if (k1 >= 50) k1 = 49;
            dndxe[k1][j] += 50.0/2.0;
            ndndxe[j] += 1;
          }
          double phi = inclusive_jets[k].phi_std()-phig;
          if (phi<0) phi += 2*3.14159;
          if (phi>3.14159) phi = 2*3.14159-phi;
          unsigned int k2 = int(phi/(3.14159/50.0));
          if (k2 >=50) k2 = 49;
          dndphi[k2][j] += 50.0/3.14159;
          //
          double Jx=inclusive_jets[k].px();
          double Jy=inclusive_jets[k].py();
          double Jz=inclusive_jets[k].pz();
          double JM=sqrt(pow(Jx,2)+pow(Jy,2)+pow(Jz,2));
          double JE=inclusive_jets[k].E();
          //
          double phi2;
          if (Jy>0) {
            double tanphi=Jz/Jy;
            phi2=atan(tanphi);
          }
          if (Jy<0 && Jz>0) {
            double tanphi=abs(Jz/Jy);
            double phia=atan(tanphi);
            phi2=3.14159-phia;
          }
          if (Jy<0 && Jz<0) {
            double tanphi=abs(Jz/Jy);
            double phia=atan(tanphi);
            phi2=-3.14159+phia;
          }
          if (Jy=0 && Jz>0) phi2=3.14159/2;
          if (Jy=0 && Jz<0) phi2=-3.14159/2;
          unsigned int k7 = int((phi2+3.14159)/(2*3.14159/50.0));
          if (k7 >=50) k7 = 49;
          dndphi2[k7][j] += 50.0/(2*3.14159);
          //
          double theta=acos(inclusive_jets[k].py()/sqrt(pow(inclusive_jets[k].px(),2)+pow(inclusive_jets[k].py(),2)+pow(inclusive_jets[k].pz(),2)));
          unsigned int k4 = int(theta/(3.14159/50.0));
          if(k4 >=50) k4 = 49;
          dndthe[k4][j] +=50.0/3.14159;
          //
          double pt1=sqrt(pow(inclusive_jets[k].px(),2)+pow(inclusive_jets[k].pz(),2));
          unsigned int k5 = int(pt1/(100.0/100.0));
          if(k5 >=100) k5 = 99;
          dndpt1[k5][j] +=100.0/100.0;
          
          double pt2=inclusive_jets[k].pt();
          unsigned int k6 = int(pt2/(100.0/100.0));
          if(k6 >=100) k6 = 99;
          dndpt2[k6][j] +=100.0/100.0;;
          /*test why py>E
           if(inclusive_jets[k].py()>inclusive_jets[k].E()) dndthe[k4][j] +=0.0;
           if(inclusive_jets[k].py()<=inclusive_jets[k].E()) dndthe[k4][j] +=50.0/3.14159;
           vector<fastjet::PseudoJet> constituents = inclusive_jets[k].constituents();
           if(k4==99) {
           cout << inclusive_jets[k].py() << " " << inclusive_jets[k].E() << " " << theta << endl;
           cout << i << " " << j << endl;
           for (unsigned int k1=0; k1<constituents.size(); k1++)	{
           if (constituents[k1].user_index()<0) cout << "-1" << endl;
           if (constituents[k1].user_index()>0) cout << "+1" << endl;
           cout << constituents[k1].user_index() << endl;
           cout << " " << constituents[k1].px() << " " << constituents[k1].py() << " " << constituents[k1].pz() << " " << constituents[k1].E() << endl;
           
           }
           assert(1==0);
           }
           */
          
          //          cout << inclusive_jets[k].pt2() << (pow(inclusive_jets[k].px(),2)+pow(inclusive_jets[k].py(),2)) << endl;
          
          // pt change
          double cosgj = (pow(inclusive_jets[k].px(),2)+pow(inclusive_jets[k].py(),2)+pow(inclusive_jets[k].pz(),2)+pow(Eg,2)-pow(inclusive_jets[k].px()+Eg/cosh(etag)*cos(phig),2)-pow(inclusive_jets[k].py()+Eg/cosh(etag)*sin(phig),2)-pow(inclusive_jets[k].pz()+Eg*tanh(etag),2))/(-2*( inclusive_jets[k].px()*Eg/cosh(etag)*cos(phig)+inclusive_jets[k].py()*Eg/cosh(etag)*sin(phig)+inclusive_jets[k].pz()*Eg*tanh(etag)));
          if (cosgj < 1.0) dpt[j] += inclusive_jets[k].E()*sqrt(1.0-pow(cosgj,2));
          
          // pt on transver plane w.r.t gamma direction
          //          dpt[j] += inclusive_jets[k].pt()*sin(phi);
          //          cout << inclusive_jets[k].phi_std() << " " << phig << " " << cosgj << " " << sqrt(1.0-pow(cosgj,2)) << endl;
          // get the constituents of the jet
          vector<fastjet::PseudoJet> constituents = inclusive_jets[k].constituents();
          double Eth=0.0, Erad=0.0;
          for (unsigned int k1=0; k1<constituents.size(); k1++){
            if (pid[abs(constituents[k1].user_index())] == 1) Erad += constituents[k1].E();
            if (pid[abs(constituents[k1].user_index())] == 2) Eth += E[abs(constituents[k1].user_index())];
            //            cout << constituents[k1].user_index() << " " << pid[abs(constituents[k1].user_index())] << endl;
            // a^2=b^2+c^2-2bc cos_bc
            //            double cosab = (pow(constituents[k1].E(),2)+pow(inclusive_jets[k].E(),2)-pow(constituents[k1].px()-inclusive_jets[k].px(),2)-pow(constituents[k1].py()-inclusive_jets[k].py(),2)-pow(constituents[k1].pz()-inclusive_jets[k].pz(),2))/(2*(constituents[k1].pz()*inclusive_jets[k].pz()+constituents[k1].py()*inclusive_jets[k].py()+constituents[k1].px()*inclusive_jets[k].px()));
            // A.B=|A|.|B|.cosAB
            //		double cosab = (constituents[k1].pz()*inclusive_jets[k].pz()+constituents[k1].py()*inclusive_jets[k].py()+constituents[k1].px()*inclusive_jets[k].px())/constituents[k1].E()/inclusive_jets[k].E();
            double cosab = (constituents[k1].pz()*inclusive_jets[k].pz()+constituents[k1].py()*inclusive_jets[k].py()+constituents[k1].px()*inclusive_jets[k].px())/constituents[k1].E()/sqrt(pow(inclusive_jets[k].px(),2)+pow(inclusive_jets[k].py(),2)+pow(inclusive_jets[k].pz(),2));
            //Jet fragmentation function
            unsigned int i1 = int(fabs(constituents[k1].E()*cosab)/inclusive_jets[k].E()*50.0);
            unsigned int i0 = int(fabs(constituents[k1].E()*cosab)/Ej0*50.0);
            unsigned int i2 = int(constituents[k1].E()/inclusive_jets[k].E()*50.0);
            if (i1 >= 50) i1 = 49;
            if (i0 >= 50) i0 = 49;
            if (i2 >= 50) i2 = 49;
            if (constituents[k1].user_index()<0) {
              dndxpl[i1][j] -= 50.0;
              dndxpl0[i0][j] -= 50.0;
              dndze[i2][j] -= 50.0;
              ndndze[j] -= 1;
              // Jet fragmentation function in diferent x range
              if (j==0 || j==nt-1) {
                unsigned int i11 = int(fabs(constituents[k1].E()*cosab)/inclusive_jets[k].E()*20.0);
                if (i11 >= 20) i11 = 19;
                if (int(inclusive_jets[k].pt()/(Eg/cosh(etag)))<0.5) {
                  if (j==0) {
                    dndxpl1[i11][0] -= 20.0;
                    ndndxpl1[0] -= 1; }
                  else {
                    dndxpl1[i11][1] -= 20.0;
                    ndndxpl1[1] -= 1;
                  }
                }else if (int(inclusive_jets[k].pt()/(Eg/cosh(etag)))<2.0) {
                  if (j==0) {
                    dndxpl2[i11][0] -= 20.0;
                    ndndxpl2[0] -= 1; }
                  else {
                    dndxpl2[i11][1] -= 20.0;
                    ndndxpl2[1] -= 1;
                  }
                }
              }
            }
            else{
              dndxpl[i1][j] += 50.0;
              dndxpl0[i0][j] += 50.0;
              dndze[i2][j] += 50.0;
              ndndze[j] += 1;
              // Jet fragmentation function in diferent x range
              if (j==0 || j==nt-1) {
                unsigned int i11 = int(fabs(constituents[k1].E()*cosab)/inclusive_jets[k].E()*20.0);
                if (i11 >= 20) i11 = 19;
                if (int(inclusive_jets[k].pt()/(Eg/cosh(etag)))<0.5) {
                  if (j==0) {
                    dndxpl1[i11][0] += 20.0;
                    ndndxpl1[0] += 1; }
                  else {
                    dndxpl1[i11][1] += 20.0;
                    ndndxpl1[1] += 1;
                  }
                }else if (int(inclusive_jets[k].pt()/(Eg/cosh(etag)))<2.0) {
                  if (j==0) {
                    dndxpl2[i11][0] += 20.0;
                    ndndxpl2[0] += 1; }
                  else {
                    dndxpl2[i11][1] += 20.0;
                    ndndxpl2[1] += 1;
                  }
                }
              }
            }
            // pt of partons in the reconstructed jet w.r.t. gamma
            double cospg = (pow(constituents[k1].px(),2)+pow(constituents[k1].py(),2)+pow(constituents[k1].pz(),2)+pow(Eg,2)-pow(constituents[k1].px()+Eg/cosh(etag)*cos(phig),2)-pow(constituents[k1].py()+Eg/cosh(etag)*sin(phig),2)-pow(constituents[k1].pz()+Eg*tanh(etag),2))/(-2*( constituents[k1].px()*Eg/cosh(etag)*cos(phig)+constituents[k1].py()*Eg/cosh(etag)*sin(phig)+constituents[k1].pz()*Eg*tanh(etag)));
            if (cospg < 1.0) {
              int iptp = int(constituents[k1].E()*sqrt(1.0-pow(cospg,2))/(30.0/30.0));
              if (constituents[k1].user_index()>0) {
                dptp[j] += constituents[k1].E()*sqrt(1.0-pow(cospg,2));
                if (iptp < 30) {
                  dndptp[iptp][j] += 1.0;
                  ndndptp[j] +=1;
                }
              }
              if (constituents[k1].user_index()<0) {
                dptp[j] -= constituents[k1].E()*sqrt(1.0-pow(cospg,2));
                if (iptp < 30) {
                  dndptp[iptp][j] -= 1.0;
                  ndndptp[j] -=1;
                }
              }
              
              //               cout << cospg << " " << sqrt(1.0-pow(cospg,2)) << endl;
            }
          } // end constituents
          dEall[j] += (Ej0-inclusive_jets[k].E());
          dEjr[j] += (Ej0-(inclusive_jets[k].E()-Eth))/Ej0;
          dEj[j] += (Ej0-(inclusive_jets[k].E()-Erad-Eth))/Ej0;
          //          cout << Erad << " " << Eth << endl;
          
          //          cout << "start jet shape "<< constituents.size() << endl;
          
          // jet shape rho[r]
          //          if (j==0 || j==nt-1) {
          double ptouter = 0;
          for (unsigned int k1=0; k1<constituents.size(); k1++) {
            if (constituents[k1].user_index()<0) {
              ptouter -= constituents[k1].pt(); }
            else {
              ptouter += constituents[k1].pt();
            }
          }
          double ptjet = ptouter;
          for (int k3=9; k3>=0; k3--) {
            //            cout << k3 << endl;
            double ptinner = 0;
            for (unsigned int k1=0; k1<constituents.size(); k1++) {
              double phi = inclusive_jets[k].phi_std()-constituents[k1].phi_std();
              if (phi<0) phi += 2*3.14159;
              if (phi>3.14159) phi = 2*3.14159-phi;
              double eta = inclusive_jets[k].eta()-constituents[k1].eta();
              unsigned int ir = int(sqrt(pow(phi,2)+pow(eta,2))/(R/10.0));
              if (ir<k3) {
                if (constituents[k1].user_index()<0) {
                  ptinner -= constituents[k1].pt(); }
                else {
                  ptinner += constituents[k1].pt();
                }
              }
            }
            //            if (j==0)rho[k3][0] += (ptouter-ptinner)/ptjet/(R/10.0);
            //            if (j==nt-1)rho[k3][1] += (ptouter-ptinner)/ptjet/(R/10.0);
            rho[k3][j] += (ptouter-ptinner)/ptjet/(R/10.0);
            ptouter = ptinner;
          }
          //          } //end jet shape
          //          cout << "end jet shape" << endl;
          
          // jet shape rhoo[r] shape of pt out side the jet cone
          if (j==0 || j==nt-1) {
            double ptjet = 0;
            for (unsigned int k1=0; k1<constituents.size(); k1++) {
              if (constituents[k1].user_index()<0) {
                ptjet -= constituents[k1].pt(); }
              else {
                ptjet += constituents[k1].pt();
              }
            }
            double ptouter = 0;
            for (unsigned int k2=0; k2<inclusive_jets.size(); k2++) {
              vector<fastjet::PseudoJet> constituentsk2 = inclusive_jets[k2].constituents();
              for (unsigned int k1=0; k1<constituentsk2.size(); k1++) {
                double phi = inclusive_jets[k].phi_std()-constituentsk2[k1].phi_std();
                if (phi<0) phi += 2*3.14159;
                if (phi>3.14159) phi = 2*3.14159-phi;
                double eta = inclusive_jets[k].eta()-constituentsk2[k1].eta();
                if (sqrt(pow(phi,2)+pow(eta,2))>R && sqrt(pow(phi,2)+pow(eta,2))<1.5708) {
                  if (constituentsk2[k1].user_index()<0) {
                    ptouter -= constituentsk2[k1].pt(); }
                  else {
                    ptouter += constituentsk2[k1].pt();
                  } }
              } }
            
            for (int k3=9; k3>=0; k3--) {
              //            cout << k3 << endl;
              double ptinner = 0;
              for (unsigned int k2=0; k2<inclusive_jets.size(); k2++) {
                vector<fastjet::PseudoJet> constituentsk2 = inclusive_jets[k2].constituents();
                for (unsigned int k1=0; k1<constituentsk2.size(); k1++) {
                  double phi = inclusive_jets[k].phi_std()-constituentsk2[k1].phi_std();
                  if (phi<0) phi += 2*3.14159;
                  if (phi>3.14159) phi = 2*3.14159-phi;
                  double eta = inclusive_jets[k].eta()-constituentsk2[k1].eta();
                  if (sqrt(pow(phi,2)+pow(eta,2))>R && sqrt(pow(phi,2)+pow(eta,2))<R+k3*(1.5708-R)/10.0) {
                    if (constituentsk2[k1].user_index()<0) {
                      ptinner -= constituentsk2[k1].pt(); }
                    else {
                      ptinner += constituentsk2[k1].pt();
                    }
                  }
                } }
              if (j==0)rhoo[k3][0] += (ptouter-ptinner)/ptjet/((1.5708-R)/10.0);
              if (j==nt-1)rhoo[k3][1] += (ptouter-ptinner)/ptjet/((1.5708-R)/10.0);
              ptouter = ptinner;
            }
          } //end jet shape outer
        } //end positive energy jet
        // break when the leading jet is found
        if (inclusive_jets[k].user_index()>=0) break;
      } //end k: jet size
    } //end j: nt
  }   //end i: event
  cout << "end of event" << endl;
  for (unsigned int k=0; k<nt; k++) {
    dEall[k] /= (double)nevent;
    dEjr[k] /= (double)nevent;
    dEj[k] /= (double)nevent;
    dpt[k] /= (double)nevent;
    dptp[k] /= (double)nevent;
    for (unsigned int k1=0; k1<50; k1++) {
      dnde[k1][k] /= (double)ndnde[k];
      dndxe[k1][k] /= (double)ndndxe[k];
      dndxpl[k1][k] /= (double)ndndze[k];
      dndxpl0[k1][k] /= (double)ndndze[k];
      dndze[k1][k] /= (double)ndndze[k];
      dndphi[k1][k] /= (double)nevent;
      dndphi2[k1][k] /= (double)nevent;
    }
    //
    for(unsigned int k2=0; k2<50; k2++) {
      dndthe[k2][k] /= (double)nevent;
    }
    
    for(unsigned int k3=0; k3<100; k3++) {
      dndpt1[k3][k] /= (double)nevent;
    }
    
    for(unsigned int k4=0; k4<100; k4++) {
      dndpt2[k4][k] /= (double)nevent;
    }
    //
  }
  for(unsigned int k=0; k<10; k++) {
    for(unsigned int k1=0; k1<nt; k1++) {
      rho[k][k1] /= (double)nevent;
      //	cout << k << " " << k1 << endl;
    }
    rhoo[k][0] /= (double)nevent;
    rhoo[k][1] /= (double)nevent;
  }
  for(unsigned int k=0; k<20; k++) {
    dndxpl1[k][0] /= (double)ndndxpl1[0];
    dndxpl1[k][1] /= (double)ndndxpl1[1];
    dndxpl2[k][0] /= (double)ndndxpl2[0];
    dndxpl2[k][1] /= (double)ndndxpl2[1];
  }
  for (unsigned int k=0; k<nt; k++) {
    for (unsigned int k1=0; k1<30; k1++) {
      dndptp[k1][k] /= (double)ndndptp[k];
    }
  }
  cout << "end of data" << endl;
  cout << "begin to save to outfile" << endl;
  // save to histofile
  //---------------------------------------------------------------------------------
  //energy loss data
  histo << "!Results from " << myfile << endl;
  histo << "newpage zone 2 3 1" << endl;
  histo << "openhisto name elossjet" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp lro xrange 0 8" << endl;
  histo << "txt \"xaxis t (fm/c)\"" << endl;
  histo << "txt \"yaxis [D]E/E\?0!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<nt; k++) {
    histo << k << " " << dEj[k] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name elossjetrad" << endl;
  histo << "htyp lra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<nt; k++) {
    histo << k << " " << dEjr[k] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name elossall" << endl;
  histo << "htyp lru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<nt; k++) {
    histo << k << " " << dEall[k] << endl;
  }
  
  //dnde_jet data: Ejet
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro xrange 0 200" << endl;
  histo << "txt \"xaxis E (GeV)\"" << endl;
  histo << "txt \"yaxis dn/dE\?j!(GeV^-1!)\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt2" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt3" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt4" << endl;
  histo << "htyp hga" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt5" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt6" << endl;
  histo << "htyp hbo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt7" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndejt9" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*200.0/50.0 << " " << dnde[k][9] << endl;
  }
  
  //dndxe data x=Ejet/Egamma
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro xrange 0 2" << endl;
  histo << "txt \"xaxis x\"" << endl;
  histo << "txt \"yaxis dn/dx\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet2" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet3" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet4" << endl;
  histo << "htyp hga" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo <<  (k+0.5)*2.0/50.0 << " " << dndxe[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet5" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet6" << endl;
  histo << "htyp hbo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet7" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxet9" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2.0/50.0 << " " << dndxe[k][9] << endl;
  }
  
  histo << "endarray" << endl;
  //dndxpl data xpl=Plpart/Ejet
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt2" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt3" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt4" << endl;
  histo << "htyp hga" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt5" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt6" << endl;
  histo << "htyp hbo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt7" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxplt9" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl[k][9] << endl;
  }
  
  //dndxpl0 data xpl=Plpart/Ej0
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t1" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t2" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t3" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t4" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t5" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t6" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t7" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][7] << endl;
  }
  
  //dndxpl0 data xpl=Plpart/Ej0
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t8" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][8] << endl;
  }
  
  //dndxpl0 data xpl=Plpart/Ej0
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl0t9" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j0! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j0!dn/dP\?L!\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndxpl0[k][9] << endl;
  }
  
  //dndze data
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis z\"" << endl;
  histo << "txt \"yaxis dn/dz\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet2" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet3" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet4" << endl;
  histo << "htyp hga" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet5" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet6" << endl;
  histo << "htyp hbo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet7" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndzet9" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)/50.0 << " " << dndze[k][9] << endl;
  }
  
  //dndthe data jet theta dis wrt single initial jet parton
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 3.14159" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet2" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet3" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet4" << endl;
  histo << "htyp hga" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet5" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet6" << endl;
  histo << "htyp hbo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet7" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndthet9" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndthe[k][9] << endl;
  }
  
  //dndpt1 data jet theta dis wrt single initial jet parton
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t1" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t2" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t3" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t4" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t5" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t6" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t7" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t8" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt1t9" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt1[k][9] << endl;
  }
  
  //dndpt2 data jet theta dis wrt single initial jet parton
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t1" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t2" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t3" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t4" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t5" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t6" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t7" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t8" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndpt2t9" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 100.0" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<100; k++) {
    histo << (k+0.5)*100.0/100.0 << " " << dndpt2[k][9] << endl;
  }
  
  //dndphi data jet phi dis wrt gamma
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 3.14159" << endl;
  histo << "txt \"xaxis [f]\"" << endl;
  histo << "txt \"yaxis dn/d[f]\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit2" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit3" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit4" << endl;
  histo << "htyp hga" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit5" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit6" << endl;
  histo << "htyp hbo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit7" << endl;
  histo << "htyp hba" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphit9" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*3.14159/50.0 << " " << dndphi[k][9] << endl;
  }
  
  // dndphi2 for mach cone
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t0" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t1" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t2" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t3" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t4" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t5" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t6" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t7" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t8" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndphi2t9" << endl;
  histo << "htyp hbu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<50; k++) {
    histo << (k+0.5)*2*3.14159/50.0-3.14159 << " " << dndphi2[k][9] << endl;
  }
  
  //jet shape rho data
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0  " << R << endl;
  histo << "txt \"xaxis r\"" << endl;
  histo << "txt \"yaxis [r](r)\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot2" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][2] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot3" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][3] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot4" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][4] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot5" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][5] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot6" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][6] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot7" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][7] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot8" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][8] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhot9" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << (k+0.5)*R/10.0 << " " << rho[k][9] << endl;
  }
  
  //dndxpl data xpl=Plpart/Ejet
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl1t0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange 0 1" << endl;
  histo << "txt \"xaxis P\?L!/E\?j! (GeV)\"" << endl;
  histo << "txt \"yaxis E\?j!dn/dP\?L!\"" << endl;
  histo << "text 0.5 0.65 \"dot: x.lt.0.5; red: initial\"" << endl;
  histo << "text 0.5 0.75 \"full: x.gt.0.5; green: final\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<20; k++) {
    histo << (k+0.5)/20.0 << " " << dndxpl1[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl1t1" << endl;
  histo << "htyp hru" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<20; k++) {
    histo << (k+0.5)/20.0 << " " << dndxpl2[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl2t0" << endl;
  histo << "htyp hgo" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<20; k++) {
    histo << (k+0.5)/20.0 << " " << dndxpl1[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndxpl2t1" << endl;
  histo << "htyp hgu" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<20; k++) {
    histo << (k+0.5)/20.0 << " " << dndxpl2[k][1] << endl;
  }
  
  //jet shape rhoo data
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhoot0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp hro ymod log xrange " << R << " " << 1.5708 << endl;
  histo << "txt \"xaxis r\"" << endl;
  histo << "txt \"yaxis [r](r)\"" << endl;
  histo << "array 2"  << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << R+(k+0.5)*(1.5708-R)/10.0 << " " << rhoo[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name rhoot1" << endl;
  histo << "htyp hra" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<10; k++) {
    histo << R+(k+0.5)*(1.5708-R)/10.0 << " " << rhoo[k][1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name ptp" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp lru xrange 0 8" << endl;
  histo << "txt \"xaxis t (fm/c)\"" << endl;
  histo << "txt \"yaxis <pt>(GeV)\"" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<nt; k++) {
    histo << k+0.5 << " " << dptp[k] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndptp0" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp lro xrange 0 30" << endl;
  histo << "txt \"xaxis pt (GeV)\"" << endl;
  histo << "txt \"yaxis dn/dpt (GeV^-1!)\"" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<30; k++) {
    histo << k+0.5 << " " << dndptp[k][0] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0-" << endl;
  histo << "   " << endl;
  histo << "openhisto name dndptp1" << endl;
  histo << "set scalel 0.5" << endl;
  histo << "htyp lru xrange 0 30" << endl;
  histo << "array 2" << endl;
  
  for (unsigned int k=0; k<30; k++) {
    histo << k+0.5 << " " << dndptp[k][nt-1] << endl;
  }
  
  histo << "endarray" << endl;
  histo << "closehisto plot 0" << endl;
  
  return 0;
}
