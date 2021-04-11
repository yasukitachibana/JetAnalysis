#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>
#include <math.h>
#include <vector>
#include <iomanip>
#include "SetXML.h"
#include "SetFile.h"
#include <memory>



class Histogram{
public:
  Histogram(std::string histname);
  virtual ~Histogram();

  void Init();

  virtual void Fill( double ){}
  virtual void Fill( double, double ){}
  virtual void Fill( double, double, double ){}

  void Print();
  virtual void Print(std::string name, bool addHistname = true){}
  
  virtual void LoadHistFromFile(std::string name = "", bool addHistname = true){}
  
  virtual void Scale(double factor, std::string width = ""){}
  virtual void Normalize(std::string width  = ""){}
  
  void SetSigma(double sigma, double sigma_err);
  double GetNjetSigmaOverEev();
  double GetSigmaOverEev();

  
  virtual void DeleteTH(){}
  
  virtual void Add(std::shared_ptr<Histogram> h){}
  virtual void Add(std::shared_ptr<Histogram> h, double factor){}
  virtual void Divide(std::shared_ptr<Histogram> h){}
  virtual void Show(std::shared_ptr<Histogram> h){}
  virtual void Show(){}

  
  virtual std::string HistName();
  
  virtual void SetSidebandHist(std::shared_ptr<Histogram> h, double min, double max){}
  
  //------------------------------------------------------
  virtual int GetNbinsX(){return 0;}
  virtual int GetNbinsY(){return 0;}
  virtual double GetX( int ix ){return 0;}
  virtual double GetY( int iy ){return 0;}
  virtual double GetVal( int ix){return 0;}
  virtual double GetErr( int ix){return 0;}
  virtual double GetVal( int ix, int iy ){return 0;}
  virtual double GetErr( int ix, int iy ){return 0;}
  virtual int FindBinX(double x){ return 0; }
  virtual int FindBinY(double y){ return 0; }
  //------------------------------------------------------


  //+++++++++++++++++++++++++++++++++++++
  double Nev(){return nEv;}
  void EventCount(int n){nEv+=double(n);}
  void EventCount(double n){nEv+=n;}
  void EventCount(){nEv+=1.0;}
  void ResetNev(double n_total){nEv=n_total;}
  void ResetNev(int n_total){nEv=double(n_total);}
  //+++++++++++++++++++++++++++++++++++++
  double Njet(){return nJet;}
  void JetTriggered(int n){nJet+=double(n);}
  void JetTriggered(double n){nJet+=n;}
  void JetTriggered(){nJet+=1.0;}
  void ResetNjet(double n_total){nJet=n_total;}
  void ResetNjet(int n_total){nJet=double(n_total);}
  //+++++++++++++++++++++++++++++++++++++
  double Sigma(){return sigma;}
  double SigmaErr(){return sigma_err;}
  //+++++++++++++++++++++++++++++++++++++
  
  virtual void DivideWithError( double norm, double norm_error ){}

protected:
  std::string histname;
  double nJet;
  double nEv;
  double sigma;
  double sigma_err;
  
private:
  virtual void InitHist(){}

};

#endif 
