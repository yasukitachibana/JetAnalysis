#ifndef PSTAT_H_
#define PSTAT_H_

#include "Particle.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================

class PStatBase
{

public:
  virtual ~PStatBase(){}
  virtual bool Trigger(std::shared_ptr<Particle> p) { return false; }
  virtual std::vector<int> StatList(){ return {}; }
}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) PStat Class
//===========================================================================================================================
class Selected : public PStatBase
{

public:
  Selected(std::vector<int> stat_in) : stat(stat_in){}
  ~Selected() {}
  bool Trigger(std::shared_ptr<Particle> p);
  std::vector<int> StatList(){ return stat; }
private:
  std::vector<int> stat;

};

class Inclusive : public PStatBase
{
public:
  Inclusive(){}
  ~Inclusive() {}
  bool Trigger(std::shared_ptr<Particle> p) { return true; }
};
//===========================================================================================================================

#endif
