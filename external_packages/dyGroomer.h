#ifndef dyGroomer_h
#define dyGroomer_h

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/WrappedStructure.hh"
#include "fastjet/contrib/RecursiveSymmetryCutBase.hh"
#include "fastjet/contrib/Nsubjettiness.hh"

// #include "jetCollection.hh"

//---------------------------------------------------------------
// Description
// This class runs Dynamical grooming on a set of jets
// Author: Alba Soto-Ontoso
//---------------------------------------------------------------

class dyGroomer
{

private:
   double a_;
   double zCut_;

   std::vector<fastjet::PseudoJet> fjInputs_;  // ungroomed jets
   std::vector<fastjet::PseudoJet> fjOutputs_; // groomed jets
   std::vector<double> zg_;                    // zg of groomed jets
   std::vector<int> drBranches_;               // dropped branches
   std::vector<double> dr12_;                  // distance between the two subjet
   std::vector<double> tau21_;                 // n-subjettiness ratio 21
   std::vector<double> tau32_;                 // n-subjettiness ratio 32

public:
   dyGroomer(double a = 1, double zCut = -1);
   void setInputJets(std::vector<fastjet::PseudoJet> v);
   std::vector<fastjet::PseudoJet> getGroomedJets() const;
   std::vector<double> getZgs() const;
   std::vector<double> getDR12() const;
   std::vector<int> getNDroppedSubjets() const;
   std::vector<double> getTau21() const;
   std::vector<double> getTau32() const;
   double getKappa(double pt, double theta, double z);
   // std::vector<fastjet::PseudoJet> doGrooming(jetCollection &c);
   std::vector<fastjet::PseudoJet> doGrooming(std::vector<fastjet::PseudoJet> v);
   std::vector<fastjet::PseudoJet> doGrooming();
   fastjet::PseudoJet doGrooming(fastjet::PseudoJet jet,
                                 fastjet::PseudoJet &daughter1,
                                 fastjet::PseudoJet &daughter2,
                                 bool &has_substructure);
};

dyGroomer::dyGroomer(double a, double zCut)
    : a_(a), zCut_(zCut)
{
}

void dyGroomer::setInputJets(const std::vector<fastjet::PseudoJet> v)
{
   fjInputs_ = v;
}

std::vector<fastjet::PseudoJet> dyGroomer::getGroomedJets() const
{
   return fjOutputs_;
}

std::vector<double> dyGroomer::getZgs() const
{
   return zg_;
}

std::vector<double> dyGroomer::getDR12() const
{
   return dr12_;
}

std::vector<double> dyGroomer::getTau21() const
{
   return tau21_;
}

std::vector<double> dyGroomer::getTau32() const
{
   return tau32_;
}

std::vector<int> dyGroomer::getNDroppedSubjets() const
{
   return drBranches_;
}

// std::vector<fastjet::PseudoJet> dyGroomer::doGrooming(jetCollection &c)
// {
//    return doGrooming(c.getJet());
// }

std::vector<fastjet::PseudoJet> dyGroomer::doGrooming(std::vector<fastjet::PseudoJet> v)
{
   setInputJets(v);
   return doGrooming();
}

double dyGroomer::getKappa(double pt, double theta, double z)
{
   double kappa = 1 / (z * (1 - z) * pt * pow(theta, a_));
   return kappa;
}

fastjet::PseudoJet dyGroomer::doGrooming(fastjet::PseudoJet jet,
                                         fastjet::PseudoJet &daughter1,
                                         fastjet::PseudoJet &daughter2,
                                         bool &has_substructure)
{
   //----------------------------------------------
   // Reclustering by C/A
   //----------------------------------------------
   std::vector<fastjet::PseudoJet> particles, ghosts;
   fastjet::SelectorIsPureGhost().sift(jet.constituents(), ghosts, particles);

   fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm, 1.);
   fastjet::ClusterSequence cs(particles, jet_def);
   vector<fastjet::ClusterSequence::history_element> cs_history = cs.history();
   std::vector<fastjet::PseudoJet> tempJets = fastjet::sorted_by_pt(cs.inclusive_jets());

   // To make the jetp_index work we need to define our jets like this
   std::vector<fastjet::PseudoJet> tempJets_two = cs.jets();
   //----------------------------------------------

   //----------------------------------------------
   // If C/A failed
   //----------------------------------------------
   if (tempJets.size() < 1)
   {
      return fastjet::PseudoJet(0., 0., 0., 0.);
   }
   //----------------------------------------------

   //----------------------------------------------
   // Find min kappa
   //----------------------------------------------
   fastjet::PseudoJet CurrentJet = tempJets[0];
   fastjet::PseudoJet piece1, piece2;
   double min_kappa = 1e8;
   double zg = -1.;
   double deltaR = -1;
   double kappa = -1;
   double pt = -1;
   int ndrop = 0;

   int current_in_ca_tree = -1; // (history) index of the current particle in the C/A tree

   // now recurse into the jet's structure to find the maximum hardness/ minimum inverse hardness
   while (CurrentJet.has_parents(piece1, piece2))
   {

      if (CurrentJet.pt2() <= 0)
         break;

      if (piece1.pt() + piece2.pt() > 0 && piece1.E() > 0. && piece2.E() > 0. && piece1.m() > 0. && piece2.m() > 0.)
      {
         pt = piece1.pt() + piece2.pt();
         zg = min(piece1.pt(), piece2.pt()) / pt;
         deltaR = piece1.delta_R(piece2);
         kappa = getKappa(pt, deltaR, zg);

         if (kappa < min_kappa && zg >= zCut_)
         {
            // std::cout << "zg= " << zg << " > " << zCut_ << endl;
            min_kappa = kappa;
            current_in_ca_tree = CurrentJet.cluster_hist_index();
         }
         else
         {
            // std::cout << "zg= " << zg << " < " << zCut_ << endl;
            ndrop++;
         }
      }

      if (piece1.pt() > piece2.pt())
         CurrentJet = piece1;
      else
         CurrentJet = piece2;
   }
   //----------------------------------------------

   //----------------------------------------------
   // If jet passed Dynamical Grooming
   //----------------------------------------------
   if (current_in_ca_tree >= 0)
   {
      has_substructure = true;
      fastjet::PseudoJet groomed_jet = tempJets_two[cs_history[current_in_ca_tree].jetp_index];
      groomed_jet.has_parents(daughter1, daughter2); // set prongs
      return groomed_jet; // put CA reclusterd jet after grooming
   }
   else
   {
      return CurrentJet;
   }
   //----------------------------------------------
}

std::vector<fastjet::PseudoJet> dyGroomer::doGrooming()
{
   fjOutputs_.reserve(fjInputs_.size());
   zg_.reserve(fjInputs_.size());
   dr12_.reserve(fjInputs_.size());
   drBranches_.reserve(fjInputs_.size());
   tau21_.reserve(fjInputs_.size());
   tau32_.reserve(fjInputs_.size());

   int ijet = -1;

   for (fastjet::PseudoJet &jet : fjInputs_)
   {
      ++ijet;
      std::vector<fastjet::PseudoJet> particles, ghosts;
      fastjet::SelectorIsPureGhost().sift(jet.constituents(), ghosts, particles);

      fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm, 1.);
      fastjet::ClusterSequence cs(particles, jet_def);
      vector<fastjet::ClusterSequence::history_element> cs_history = cs.history();
      std::vector<fastjet::PseudoJet> tempJets = fastjet::sorted_by_pt(cs.inclusive_jets());

      // To make the jetp_index work we need to define our jets like this
      std::vector<fastjet::PseudoJet> tempJets_two = cs.jets();

      if (tempJets.size() < 1)
      {
         fjOutputs_.push_back(fastjet::PseudoJet(0., 0., 0., 0.));
         zg_.push_back(-1.);
         dr12_.push_back(-1.);
         drBranches_.push_back(-1.);
         tau21_.push_back(-1);
         tau32_.push_back(-1);
         continue;
      }

      fastjet::PseudoJet CurrentJet = tempJets[0];
      fastjet::PseudoJet piece1, piece2;
      double min_kappa = 1e8;
      double zg = -1.;
      double deltaR = -1;
      double kappa = -1;
      double pt = -1;
      int ndrop = 0;
      int current_in_ca_tree = -1; // (history) index of the current particle in the C/A tree

      // now recurse into the jet's structure to find the maximum hardness/ minimum inverse hardness
      while (CurrentJet.has_parents(piece1, piece2))
      {

         if (CurrentJet.pt2() <= 0)
            break;

         if (piece1.pt() + piece2.pt() > 0 && piece1.E() > 0. && piece2.E() > 0. && piece1.m() > 0. && piece2.m() > 0.)
         {
            pt = piece1.pt() + piece2.pt();
            zg = min(piece1.pt(), piece2.pt()) / pt;
            deltaR = piece1.delta_R(piece2);
            kappa = getKappa(pt, deltaR, zg);

            if (kappa < min_kappa)
            {
               min_kappa = kappa;
               current_in_ca_tree = CurrentJet.cluster_hist_index();
            }
            else
               ndrop++;
         }

         if (piece1.pt() > piece2.pt())
            CurrentJet = piece1;
         else
            CurrentJet = piece2;
      }
      if (current_in_ca_tree >= 0)
      {
         fastjet::PseudoJet groomed_jet = tempJets_two[cs_history[current_in_ca_tree].jetp_index];

         // Obtain the (z,pT,theta) of the selected splitting
         fastjet::PseudoJet daughter1, daughter2;
         groomed_jet.has_parents(daughter1, daughter2);

         if (daughter1.pt() + daughter2.pt() > 0 && daughter1.E() > 0. && daughter2.E() > 0. && daughter1.m() > 0. && daughter2.m() > 0.)
         {
            pt = daughter1.pt() + daughter2.pt();
            zg = min(daughter1.pt(), daughter2.pt()) / pt;
            deltaR = daughter1.delta_R(daughter2);
            zg_.push_back(zg);
            dr12_.push_back(deltaR);
            drBranches_.push_back(ndrop);
         }

         // Compute the n-subjettiness ratio
         double beta = 2;
         fastjet::contrib::NsubjettinessRatio nSub21_beta2(2, 1, fastjet::contrib::OnePass_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(beta));

         fastjet::contrib::NsubjettinessRatio nSub32_beta2(3, 2, fastjet::contrib::OnePass_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(beta));

         double tau21_beta2 = nSub21_beta2(groomed_jet);
         double tau32_beta2 = nSub32_beta2(groomed_jet);

         tau21_.push_back(tau21_beta2);
         tau32_.push_back(tau32_beta2);
         fjOutputs_.push_back(groomed_jet); // put CA reclusterd jet after grooming
      }
      else
         fjOutputs_.push_back(CurrentJet);
   }
   return fjOutputs_;
}

#endif
