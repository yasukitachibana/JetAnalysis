#ifndef PIDENTIFY_H_
#define PIDENTIFY_H_

#include "Particle.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================

class PIdentifyBase
{

public:
  virtual ~PIdentifyBase() {}
  virtual bool Trigger(std::shared_ptr<Particle> p) { return false; }
  virtual std::vector<int> PIDList() { return {}; }
}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) PIdentify Class
//===========================================================================================================================
class PIDSelected : public PIdentifyBase
{

public:
  PIDSelected(std::vector<int> pid_in) : pid(pid_in) {}
  ~PIDSelected() {}
  std::vector<int> PIDList() { return pid; }
  bool Trigger(std::shared_ptr<Particle> p);

private:
  std::vector<int> pid;
};

class PIDInclusive : public PIdentifyBase
{

public:
  PIDInclusive() {}
  ~PIDInclusive() {}
  bool Trigger(std::shared_ptr<Particle> p);

private:
  const std::array<int, 3> pidNeutrino{12, 14, 16}; // abosolute values of neutrino/anti-neutrino pids.
};
//===========================================================================================================================

#endif
