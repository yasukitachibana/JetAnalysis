#ifndef HIST1D_H_
#define HIST1D_H_

#include "Histogram.h"
#include "TH1.h"

class Hist1D: public Histogram {
public:
  
  Hist1D(std::string histname_in, std::vector<double> binEdges_in);
  ~Hist1D();

  void Fill( double x );
  void Fill( double x, double val);
  void Print(std::string name = "", bool addHistname = true);
  void LoadHistFromFile(std::string name = "", bool add_histname = true);

  
  void Scale(double factor, std::string width = "");
  void Normalize(std::string width  = "");
  
  void Add(std::shared_ptr<Histogram> h);
  void Add(std::shared_ptr<Histogram> h, double factor);
  void Add(std::shared_ptr<Hist1D> h);
  void Add(std::shared_ptr<Hist1D> h, double factor);
  
  void Divide(std::shared_ptr<Histogram> h);
  void Divide(std::shared_ptr<Hist1D> h);
  
  void Show(std::shared_ptr<Histogram> h);
  void Show(std::shared_ptr<Hist1D> h);
  void Show();


  
  void SetErrors(Hist1D h_err2);
  void SetErrors(std::shared_ptr<Hist1D> h_err2);
  void SetErrors(TH1D *h_err2);

  TH1D* GetTH1D(){return Hist;}
  
  void DeleteTH(){
    delete[] binEdges;
    delete Hist;
  }
  
  double GetVal(int ix){ return Hist->GetBinContent(ix+1);}
  double GetErr(int ix){ return Hist->GetBinError(ix+1);}
  double GetX(int ix){ return Hist->GetXaxis()->GetBinCenter(ix+1);}
  int GetNbinsX(){ return Hist->GetNbinsX();}
  int FindBinX(double x){ return Hist->GetXaxis()->FindBin(x);}
  void DivideWithError( double norm, double norm_error );
  
private:
  
  TH1D* Hist;
  int nBin;
  double* binEdges;

  void InitHist();
  void Add(TH1D *h);
  void Add(TH1D *h, double factor);
  void Divide(TH1D *h);
  void Show(TH1D *h);

};

#endif 
