#include "LeadingParticle.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================
// Initialize static MakeUniqueHelper.here
Pythia8::Pythia LeadingParticleBase::InternalHelperPythia("IntentionallyEmpty", false);

// bool LeadingParticleBase::LeadingParticleCheck(std::shared_ptr<Particle> p)
// {
//   double charge = InternalHelperPythia.particleData.charge(p->pid());
//   if (fabs(charge) > 0.0001 /* min. value of charge */)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }
//===========================================================================================================================

//===========================================================================================================================
// (Simple) LeadingParticle Class
//===========================================================================================================================
// bool LeadingParticleOnly::Trigger(std::shared_ptr<Particle> p)
// {
//   return LeadingParticleCheck(p);
// }
//===========================================================================================================================
