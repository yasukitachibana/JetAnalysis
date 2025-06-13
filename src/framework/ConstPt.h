#ifndef CONSTPT_H_
#define CONSTPT_H_

#include "Particle.h"

//===========================================================================================================================
// BASE
//===========================================================================================================================

class ConstPtBase
{

public:
  virtual ~ConstPtBase(){}
  virtual bool Trigger(std::shared_ptr<Particle> p){return false;}
  virtual bool Selected() {return false;}
  virtual double Min() {return 0;}  
  virtual double Max() {return 0;}    
}; // END BASE CLASS

//===========================================================================================================================

//===========================================================================================================================
// (Simple) ConstPt Class
//===========================================================================================================================
class ConstPtSelected : public ConstPtBase
{

public:
  ConstPtSelected( double ptmin_in, double ptmax_in) : ptmin(ptmin_in), ptmax(ptmax_in) {}
  ~ConstPtSelected(){}
  bool Trigger(std::shared_ptr<Particle> p);
  bool Selected() {return true;}  
  double Min() {return ptmin;}  
  double Max() {return ptmax;}    

private:
  const double ptmin, ptmax;  
};

class ConstPtInclusive : public ConstPtBase
{
public:
  ConstPtInclusive(){}
  ~ConstPtInclusive(){}
  bool Trigger(std::shared_ptr<Particle> p){return true;}
    
};
//===========================================================================================================================

#endif
