#include "Hist1D.h"

Hist1D::Hist1D(std::string histname_in, std::vector<double> binEdges_in) : Histogram::Histogram(histname_in)
{
  nBin = binEdges_in.size() - 1;
  binEdges = new double[nBin + 1];
  for (int i = 0; i < nBin + 1; i++)
  {
    binEdges[i] = binEdges_in[i];
  }
}

Hist1D::~Hist1D()
{
  // std::cout << "-$-Deleting Hist1D"<<std::endl;
}

void Hist1D::InitHist()
{
  Hist = new TH1D(histname.c_str(), histname.c_str(), nBin, binEdges);
}

void Hist1D::Fill(double x)
{
  Hist->Fill(x);
}

void Hist1D::Fill(double x, double val)
{
  Hist->Fill(x, val);
}

void Hist1D::Print(std::string name, bool addHistname /* = true */)
{

  if (addHistname)
  {
    name += histname;
  }

  std::string outfile_path = SetFile::Instance()->GetOutPath(name + ".txt");
  std::cout << "[Hist1D] Creating Output File:" << outfile_path << std::endl;

  std::ofstream ofs;
  ofs.open(outfile_path.c_str(),
           std::ios::out | std::ios::binary | std::ios::trunc);

  ofs << "# x_center x_low x_high value error n_jet n_event" << std::endl;
  ofs << std::setprecision(16);

  int nbins = Hist->GetNbinsX();

  for (int i = 1; i < nbins + 1; i++)
  {

    double x_c = Hist->GetBinCenter(i);
    double width = Hist->GetBinWidth(i);

    ofs
        << x_c << " "
        << (x_c - 0.5 * width) << " "
        << (x_c + 0.5 * width) << " "
        << Hist->GetBinContent(i) << " "
        << Hist->GetBinError(i) << " "
        << nJet << " "
        << nEv << "\n";
  }

  ofs.close();
}

void Hist1D::LoadHistFromFile(std::string name, bool addHistname)
{

  std::string filename = name;
  if (addHistname)
  {
    filename += histname;
  }
  filename += ".txt";

  std::string outfile_path = SetFile::Instance()->GetOutPath(filename);
  std::cout << "[Hist1D] Loading File:" << outfile_path << std::endl;

  // Read file
  std::ifstream ifs;
  std::stringstream str_stream;

  ifs.open(outfile_path.c_str()); // open the input file
  str_stream << ifs.rdbuf();      // read the file
  if (ifs.is_open())
  {
    ifs.close();
  }
  else
  {
    ifs.close();
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[Hist1D] " << outfile_path << " NOT FOUND" << std::endl;
    std::cout << "[Hist1D] Exit. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    exit(-1);
  }

  std::string str;

  double x_c, x_l, x_h, con, err, njet, nev;

  int i = 1;
  while (getline(str_stream, str))
  {

    if (str.find("#") == std::string::npos && !str.empty())
    {

      sscanf(str.data(),
             "%lf %lf %lf %lf %lf %lf %lf",
             &x_c, &x_l, &x_h, &con, &err, &njet, &nev);

      Hist->SetBinContent(i, con);
      Hist->SetBinError(i, err);
      nJet = njet;
      nEv = nev;
      i++;
    }
  }
}

void Hist1D::Scale(double factor, std::string width /* = "" */)
{
  if (width == "width")
  {
    Hist->Scale(factor, "width");
  }
  else
  {
    Hist->Scale(factor);
  }
}

void Hist1D::Normalize(std::string width /* = "" */)
{

  double norm, norm_error;

  if (width == "width")
  {
    norm = Hist->IntegralAndError(1, nBin, norm_error, "width");
  }
  else
  {
    norm = Hist->IntegralAndError(1, nBin, norm_error);
  }

  DivideWithError(norm, norm_error);
}

void Hist1D::DivideWithError(double norm, double norm_error)
{

  TH1D *Norm = new TH1D("_norm_hist", "_norm_hist", nBin, binEdges);

  int nbins = Hist->GetNbinsX();
  for (int i = 1; i < nbins + 1; i++)
  {
    Norm->SetBinContent(i, norm);
    Norm->SetBinError(i, norm_error);
  }

  Hist->Divide(Norm);
  delete Norm;
}

void Hist1D::Add(TH1D *h)
{
  Hist->Add(h);
}

void Hist1D::Add(TH1D *h, double factor)
{
  Hist->Add(h, factor);
}

void Hist1D::Add(std::shared_ptr<Histogram> h)
{
  Add(std::dynamic_pointer_cast<Hist1D>(h));
}

void Hist1D::Add(std::shared_ptr<Histogram> h, double factor)
{
  Add(std::dynamic_pointer_cast<Hist1D>(h), factor);
}

void Hist1D::Add(std::shared_ptr<Hist1D> h)
{
  Add(h->GetTH1D());
}

void Hist1D::Add(std::shared_ptr<Hist1D> h, double factor)
{
  Add(h->GetTH1D(), factor);
}

void Hist1D::Divide(TH1D *h)
{
  Hist->Divide(h);
}

void Hist1D::Divide(std::shared_ptr<Histogram> h)
{
  Divide(std::dynamic_pointer_cast<Hist1D>(h));
}

void Hist1D::Divide(std::shared_ptr<Hist1D> h)
{
  Divide(h->GetTH1D());
}

void Hist1D::Show(std::shared_ptr<Histogram> h)
{
  Show(std::dynamic_pointer_cast<Hist1D>(h));
}

void Hist1D::Show(std::shared_ptr<Hist1D> h)
{
  Show(h->GetTH1D());
}

void Hist1D::Show()
{
  Show(Hist);
}

void Hist1D::Show(TH1D *h)
{

  std::cout
      << "----------------------" << std::endl;
  std::cout
      << "- Histname:" << histname << std::endl;
  std::cout
      << "----------------------" << std::endl;

  std::cout
      << "xc xl xh y yerror" << std::endl;

  int nbins = h->GetNbinsX();

  for (int i = 1; i < nbins + 1; i++)
  {

    double x_c = h->GetBinCenter(i);
    double width = h->GetBinWidth(i);

    std::cout
        << x_c << " "
        << (x_c - 0.5 * width) << " "
        << (x_c + 0.5 * width) << " "
        << h->GetBinContent(i) << " "
        << h->GetBinError(i) << std::endl;
  }

  std::cout
      << "----------------------" << std::endl;
}

void Hist1D::SetErrors(Hist1D h_err2)
{
  SetErrors(h_err2.GetTH1D());
}

void Hist1D::SetErrors(std::shared_ptr<Hist1D> h_err2)
{
  SetErrors(h_err2->GetTH1D());
}

void Hist1D::SetErrors(TH1D *h_err2)
{

  int nbins = Hist->GetNbinsX();

  for (int i = 1; i < nbins + 1; i++)
  {

    double err2 = h_err2->GetBinContent(i);
    Hist->SetBinError(i, sqrt(err2));
  }
}

void Hist1D::ResetOutliers(double cut)
{

  int nbins = Hist->GetNbinsX();

  for (int i = 1; i < nbins + 1; i++)
  {
    double y_left = Hist->GetBinContent(i - 1);
    double y_center = Hist->GetBinContent(i);
    double y_right = Hist->GetBinContent(i + 1);

    if (fabs(y_left) <= DBL_EPSILON &&
        fabs(y_right) <= DBL_EPSILON &&
        fabs(y_center) < cut)
    {
      Hist->SetBinContent(i, 0.0);
      Hist->SetBinError(i, 0.0);
    }
  }
}