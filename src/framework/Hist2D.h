#ifndef HIST2D_H_
#define HIST2D_H_

#include "Histogram.h"
#include "TH2.h"

class Hist2D: public Histogram {
public:
  
  Hist2D(std::string histname_in,
                 int nBinX_in, std::vector<double> boundX_in,
                 int nBinY_in, std::vector<double> boundY_in ):
  Histogram::Histogram(histname_in),
  nBinX(nBinX_in),boundX(boundX_in),nBinY(nBinY_in),boundY(boundY_in){}

  ~Hist2D();

  void Fill( double x, double y );
  void Fill( double x, double y, double val);
  void Print(std::string name = "", bool addHistname = true);

  void LoadHistFromFile(std::string name = "", bool addHistname = true);

  void Scale(double factor, std::string width = "");
//  void Normalize(std::string width  = "");
//
  void Add(std::shared_ptr<Histogram> h);
  void Add(std::shared_ptr<Histogram> h, double factor);

  void Add(std::shared_ptr<Hist2D> h);
  void Add(std::shared_ptr<Hist2D> h, double factor);

  void Add(Hist2D h);
  void Add(Hist2D h, double factor);
  
  void Divide(std::shared_ptr<Histogram> h);
  void Divide(std::shared_ptr<Hist2D> h);
  
  void Show(std::shared_ptr<Histogram> h);
  void Show(std::shared_ptr<Hist2D> h);
  void Show();



  //
  TH2D* GetTH2D(){return Hist;}
  
  void DeleteTH(){
    delete Hist;
  }
  
  
  
  
  double GetVal( int ix, int iy ){ return Hist->GetBinContent(ix+1,iy+1);}
  double GetErr(int ix, int iy ){ return Hist->GetBinError(ix+1,iy+1);}
  double GetX( int ix ){ return Hist->GetXaxis()->GetBinCenter(ix+1);}
  double GetY( int iy ){ return Hist->GetYaxis()->GetBinCenter(iy+1);}
  int GetNbinsX(){ return Hist->GetNbinsX();}
  int GetNbinsY(){ return Hist->GetNbinsY();}
  int FindBinX(double x){ return Hist->GetXaxis()->FindBin(x)-1;}
  int FindBinY(double y){ return Hist->GetYaxis()->FindBin(y)-1;}

  
  
  void SetSidebandHist(std::shared_ptr<Histogram> h, double min, double max);
  void SetSidebandHist(std::shared_ptr<Hist2D> h, double min, double max);
  void SetSidebandHist(Hist2D h, double min, double max);
  
  void DivideWithError( double norm, double norm_error );
  
private:

  TH2D* Hist;
  int nBinX;
  int nBinY;
  std::vector<double> boundX;
  std::vector<double> boundY;

  void InitHist();

  
  void SetSidebandHist(TH2D *h, double min, double max);
  
  void Show(TH2D *h);
  void Add(TH2D *h);
  void Add(TH2D *h, double factor);
  void Divide(TH2D *h);

};

#endif 
