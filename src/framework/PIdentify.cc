#include "PIdentify.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================

//===========================================================================================================================

//===========================================================================================================================
// (Simple) PIdentify Class
//===========================================================================================================================
bool PIDSelected::Trigger(std::shared_ptr<Particle> p)
{
  int pid_in = p->pid();
  for (auto pid_trig : pid)
  {
    if (pid_in == pid_trig)
    {
      // Trigger the particle.
      return true;
    }
  }
  // Skip.
  return false;
}

bool PIDInclusive::Trigger(std::shared_ptr<Particle> p)
{
  int pid_in = p->pid();
  for (auto pn : pidNeutrino)
  {
    if (abs(pid_in) == pn)
    {
      // The particle is a neutrino. Skip
      return false;
    }
  }
  return true;
}
//===========================================================================================================================
