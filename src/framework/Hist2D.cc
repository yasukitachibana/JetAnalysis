#include "Hist2D.h"
#include "TH1.h"




Hist2D::~Hist2D(){
  //std::cout << "-$-Deleting Hist2D"<<std::endl;
}

void Hist2D::InitHist(){
  
  Hist = new TH2D( histname.c_str(), histname.c_str(),
                  nBinX, boundX[0], boundX[1],
                  nBinY, boundY[0], boundY[1] );
  
}

void Hist2D::Fill(double x, double y ){
  Hist->Fill( x, y );
  //std::cout << "[Hist2D] Fill(x1,x2) "<< std::endl;
}

void Hist2D::Fill(double x, double y, double val){
  Hist->Fill( x, y, val );
  //std::cout << "[Hist2D] Fill(x,y,val) "<< std::endl;
}

void Hist2D::Print(std::string name, bool addHistname /* = true */ ){
  
  if(addHistname){
    name += histname;
  }
  
  std::string outfile_path = SetFile::Instance()->GetOutPath( name + ".txt" );
  std::cout << "[Hist2D] Creating Output File:"<< outfile_path <<std::endl;
  
  std::ofstream ofs;
  ofs.open( outfile_path.c_str(),
           std::ios::out|std::ios::binary|std::ios::trunc);
  
  ofs << "# x_center, x_low, x_high, y_center, y_low, y_high, value, error, n_jet, n_event"<<std::endl;
  ofs <<  std::setprecision(16);
  
  int nbins_x = Hist->GetNbinsX();
  int nbins_y = Hist->GetNbinsY();
  
  for (int ix=1; ix<nbins_x+1; ix++){
    
    double x_c = Hist->GetXaxis()->GetBinCenter(ix);
    double x_w = Hist->GetXaxis()->GetBinWidth(ix);
    
    for (int iy=1; iy<nbins_y+1; iy++){
      
      double y_c = Hist->GetYaxis()->GetBinCenter(iy);
      double y_w = Hist->GetYaxis()->GetBinWidth(iy);
      
      ofs
      << x_c << " "
      << (x_c - 0.5*x_w) << " "
      << (x_c + 0.5*x_w) << " "
      << y_c << " "
      << (y_c - 0.5*y_w) << " "
      << (y_c + 0.5*y_w) << " "
      << Hist->GetBinContent(ix,iy) << " "
      << Hist->GetBinError(ix,iy) << " "
      << nJet << " "
      << nEv << "\n";
    }
  }
  
  ofs.close();
  
}

void Hist2D::LoadHistFromFile(std::string name, bool addHistname){
  
  std::string filename = name;
  if(addHistname){
    filename += histname;
  }
  filename += ".txt";
  
  
  std::string outfile_path = SetFile::Instance()->GetOutPath(filename);
  std::cout << "[Hist2D] Loading File:"<< outfile_path <<std::endl;
  
  
  
  // Read file
  std::ifstream ifs;
  std::stringstream str_stream;
  
  ifs.open(outfile_path.c_str()); //open the input file
  str_stream << ifs.rdbuf(); //read the file
  if (ifs.is_open()){
    ifs.close();
  }else{
    ifs.close();
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[Hist2D] " << outfile_path << " NOT FOUND" <<std::endl;
    std::cout << "[Hist2D] Exit. "<< std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }
  
  std::string str;
  
  double x_c, x_l, x_h, y_c, y_l, y_h, con, err, njet, nev;
  
  int ix=1;
  int iy=1;
  while ( getline( str_stream, str ) ){
    
    if( str.find("#") == std::string::npos && ! str.empty() ){
      
      sscanf(str.data(),
             "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
             &x_c, &x_l, &x_h, &y_c, &y_l, &y_h, &con, &err, &njet, &nev);
      
      
      Hist->SetBinContent(ix, iy, con);
      Hist->SetBinError(ix, iy, err);
      
      nJet = njet;
      nEv = nev;
      
      if(iy<nBinY){
        iy++;
      }else{
        iy = 1;
        ix++;
      }
    }
  }
}


void Hist2D::Scale(double factor, std::string width  /* = "" */ ){
  if(width == "width"){
    Hist->Scale(factor, "width");
  }else{
    Hist->Scale(factor);
  }
}

//void Hist2D::Normalize(std::string width  /* = "" */){
//
//  double norm, norm_error;
//
//  if( width == "width" ){
//    norm = Hist->IntegralAndError(1,nBin,norm_error,"width");
//  }else{
//    norm = Hist->IntegralAndError(1,nBin,norm_error);
//  }
//
//  DivideWithError( norm, norm_error );
//
//}

void Hist2D::DivideWithError( double norm, double norm_error )
{
  
  
  TH2D *Norm = new TH2D( "_norm_hist", "_norm_hist",
                        nBinX, boundX[0], boundX[1],
                        nBinY, boundY[0], boundY[1] );
  
  int nbins_x = Hist->GetNbinsX();
  int nbins_y = Hist->GetNbinsY();
  
  for (int ix=1; ix<nbins_x+1; ix++){
    for (int iy=1; iy<nbins_y+1; iy++){
      
      Norm->SetBinContent(ix, iy, norm);
      Norm->SetBinError(ix, iy, norm_error);
      
    }
  }
  
  Hist->Divide(Norm);
  
  delete Norm;
  
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Hist2D::Add(TH2D *h){
  Hist->Add(h);
}

void Hist2D::Add(TH2D *h, double factor){
  Hist->Add(h, factor);
}

void Hist2D::Add(std::shared_ptr<Histogram> h){
  Add(std::dynamic_pointer_cast<Hist2D>(h));
}

void Hist2D::Add(std::shared_ptr<Histogram> h, double factor){
  Add(std::dynamic_pointer_cast<Hist2D>(h), factor);
}

void Hist2D::Add(std::shared_ptr<Hist2D> h){
  Add(h->GetTH2D());
}


void Hist2D::Add(std::shared_ptr<Hist2D> h, double factor){
  Add(h->GetTH2D(), factor);
}

void Hist2D::Add(Hist2D h){
  Add(h.GetTH2D());
}

void Hist2D::Add(Hist2D h, double factor){
  Add(h.GetTH2D(), factor);
}

void Hist2D::Divide(TH2D *h){
  Hist->Divide(h);
}

void Hist2D::Divide(std::shared_ptr<Histogram> h){
  Divide(std::dynamic_pointer_cast<Hist2D>(h));
}

void Hist2D::Divide(std::shared_ptr<Hist2D> h){
  Divide(h->GetTH2D());
}


void Hist2D::Show(std::shared_ptr<Histogram> h){
  Show(std::dynamic_pointer_cast<Hist2D>(h));
}

void Hist2D::Show(std::shared_ptr<Hist2D> h){
  Show(h->GetTH2D());
}

void Hist2D::Show(){
  Show(Hist);
}

void Hist2D::Show(TH2D *h){
  
  std::cout
  << "----------------------" << std::endl;
  int nbins_x = h->GetNbinsX();
  int nbins_y = h->GetNbinsY();
  
  for (int ix=1; ix<nbins_x+1; ix++){
    
    double x_c = h->GetXaxis()->GetBinCenter(ix);
    double x_w = h->GetXaxis()->GetBinWidth(ix);
    
    for (int iy=1; iy<nbins_y+1; iy++){
      
      double y_c = h->GetYaxis()->GetBinCenter(iy);
      double y_w = h->GetYaxis()->GetBinWidth(iy);
      
      std::cout
      << x_c << " "
      << (x_c - 0.5*x_w) << " "
      << (x_c + 0.5*x_w) << " "
      << y_c << " "
      << (y_c - 0.5*y_w) << " "
      << (y_c + 0.5*y_w) << " "
      << h->GetBinContent(ix,iy) << " "
      << h->GetBinError(ix,iy) << std::endl;
    }
  }
  std::cout
  << "----------------------" << std::endl;
  
  
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void Hist2D::SetSidebandHist(std::shared_ptr<Histogram> h, double min, double max){
  SetSidebandHist(std::dynamic_pointer_cast<Hist2D>(h), min, max);
}
void Hist2D::SetSidebandHist(std::shared_ptr<Hist2D> h, double min, double max){
  SetSidebandHist( h->GetTH2D(), min, max);
}
void Hist2D::SetSidebandHist(Hist2D h, double min, double max){
  SetSidebandHist( h.GetTH2D(), min, max);
}

void Hist2D::SetSidebandHist(TH2D *h, double min, double max){
  
  TH1D *SideVal= new TH1D( "_sideband_val", "_sideband_val",
                          nBinX, boundX[0], boundX[1]);
  
  TH1D *SideErr2= new TH1D( "_sideband_err", "_sideband_err",
                           nBinX, boundX[0], boundX[1]);
  
  
  int nbins_x = Hist->GetNbinsX();
  int nbins_y = Hist->GetNbinsY();
  
  int ny_in = 0;
  for (int ix=1; ix<nbins_x+1; ix++){
    ny_in = 0;
    double x_c = Hist->GetXaxis()->GetBinCenter(ix);
    for (int iy=1; iy<nbins_y+1; iy++){
      
      double y_c = Hist->GetYaxis()->GetBinCenter(iy);
      
      if( min < fabs(y_c) && fabs(y_c) < max ){
        
        double val = h->GetBinContent(ix,iy);
        double err = h->GetBinError(ix,iy);
        SideVal->Fill(x_c, val);
        SideErr2->Fill(x_c, err*err);
        ny_in++;
        
      }
    }
  }
  
  
  if(ny_in !=0){
    for (int ix=1; ix<nbins_x+1; ix++){
      for (int iy=1; iy<nbins_y+1; iy++){
        
        double val = SideVal->GetBinContent(ix);
        double err2 = SideErr2->GetBinError(ix);
        
        Hist->SetBinContent(ix,iy,val/ny_in);
        Hist->SetBinError(ix,iy,sqrt(err2)/ny_in);
        
      }
    }
  }
  
  delete SideVal;
  delete SideErr2;
}


void Hist2D::AverageInX(){
  AverageInX(Hist);
}

void Hist2D::AverageInX(std::shared_ptr<Histogram> h){
  AverageInX(std::dynamic_pointer_cast<Hist2D>(h));
}
void Hist2D::AverageInX(std::shared_ptr<Hist2D> h){
  AverageInX( h->GetTH2D() );
}
void Hist2D::AverageInX(Hist2D h){
  AverageInX( h.GetTH2D() );
}


void Hist2D::AverageInX(TH2D *h){
  
  TH1D *Val= new TH1D( "_sideband_val", "_sideband_val",
  nBinY, boundY[0], boundY[1]);
  TH1D *Err2= new TH1D( "_sideband_err", "_sideband_err",
  nBinY, boundY[0], boundY[1]);
  
  int nbins_x = Hist->GetNbinsX();
  int nbins_y = Hist->GetNbinsY();
  
  int nx_in = 0;
  for (int iy=1; iy<nbins_y+1; iy++){
    nx_in = 0;
    double y_c = Hist->GetYaxis()->GetBinCenter(iy); // phi
    
    for (int ix=1; ix<nbins_x+1; ix++){
      //double x_c = Hist->GetXaxis()->GetBinCenter(ix);
      
      double val = h->GetBinContent(ix,iy);
      double err = h->GetBinError(ix,iy);
      
      Val->Fill(y_c, val);
      Err2->Fill(y_c, err*err);
      
    }
  }
  
  for (int ix=1; ix<nbins_x+1; ix++){
    for (int iy=1; iy<nbins_y+1; iy++){
      
      double val = Val->GetBinContent(iy);
      double err2 = Err2->GetBinError(iy);
      
      Hist->SetBinContent(ix,iy,val/nbins_x);
      Hist->SetBinError(ix,iy,sqrt(err2)/nbins_x);
      
    }
  }
  
  delete Val;
  delete Err2;
}

