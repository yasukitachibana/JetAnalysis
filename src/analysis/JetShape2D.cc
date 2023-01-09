#include "JetShape2D.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetShape2D> JetShape2D::reg("JetShape2D");

JetShape2D::JetShape2D(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetShape2D::~JetShape2D()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int JetShape2D::ReadOptionParametersFromXML()
{
  nBinPhi = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinPhi"});
  boundPhi = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundPhi", "Item"});
  nBinEta = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinEta"});
  boundEta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundEta", "Item"});
  sidebandRap = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "sidebandRap", "Item"});
  return 2;
}

////int JetShape2D::GetMixedEvIndex( int iv, int ir, int ijp, int ijr, int ipp, int ipr){
//
//  return
//  particleRapMin.size()*
//  (particlePtMin.size()*
//   (jetRapMin.size()*
//    (jetPtMin.size()*
//     (jetR.size()*
//      (iv)
//      +ir)
//     +ijp)
//    +ijr)
//   +ipp)
//  +ipr;
//}
//
//
void JetShape2D::ShowParamsSetting()
{
  std::cout << "[JetShape2D ] ***-------------------------------------------" << std::endl;
  std::cout << "[JetShape2D ] *** [SideBand]" << std::endl;
  std::cout << "[JetShape2D ] *** " << sidebandRap[0] << " < |Eta| < " << sidebandRap[1] << std::endl;
}

std::shared_ptr<Histogram> JetShape2D::CreateHist(std::string hist_name, int iv)
{
  return std::make_shared<Hist2D>(hist_name, nBinPhi, boundPhi, nBinEta, boundEta);
}

void JetShape2D::SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                               std::vector<std::shared_ptr<Particle>> particle_list,
                               std::vector<std::shared_ptr<Particle>> mixed_event_particle_list,
                               int ir, int ijp, int ijr)
{

  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    {
      for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
      {

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Main particles
        int index = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, 0);
        hist_list[index]->JetTriggered();
        //--
        for (auto p : particle_list)
        {
          if (ParticleTrigger(p, ipp, ipr))
          {
            double pt = sub_ptr->ptSub(p);
            double delta_eta = p->eta() - jet.eta();
            double delta_phi = jet.delta_phi_to(p->GetPseudoJet());

            hist_list[index]->Fill(delta_phi, delta_eta, pt);
          }
        }
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Mixed Event particles
        index = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, 1);
        hist_list[index]->JetTriggered();
        //-
        for (auto p : mixed_event_particle_list)
        {
          if (ParticleTrigger(p, ipp, ipr))
          {
            double n = sub_ptr->nSub(p);
            double delta_eta = p->eta() - jet.eta();
            double delta_phi = jet.delta_phi_to(p->GetPseudoJet());

            hist_list[index]->Fill(delta_phi, delta_eta, n);
          }
        }
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JetShape2D::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);

  auto total_hist = CreateHist("2d_" + hist_name, iv);
  total_hist->Init();

  double nJetTotal = 0.0;

  for (auto hist : hist_list)
  {
    double n_ev = hist->Nev();
    if (n_ev != 0)
    {
      nJetTotal += hist->GetNjetSigmaOverEev();
      double sigma = hist->Sigma();
      total_hist->Add(hist, sigma / n_ev);
    }
  }

  if (nJetTotal == 0)
  {
    std::cout << "[JetShape2D] 0-total Jet" << std::endl;
    std::cout << "[JetShape2D] Skip. " << std::endl;
    return;
  }

  total_hist->Scale(1.0 / nJetTotal);
  total_hist->Print("raw_");

  total_hist->DeleteTH();
}

void JetShape2D::CombineFinisher()
{
  std::cout << "[JetShape2D] Finisher" << std::endl;
  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ir = 0; ir < jetR.size(); ir++)
    {
      for (int ijp = 0; ijp < jetPtMin.size(); ijp++)
      {
        for (int ijr = 0; ijr < jetRapMin.size(); ijr++)
        {
          for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
          {
            for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
            {

              GetJetShape(iv, ir, ijp, ijr, ipp, ipr);

            } // had_rap
          }   // had_pt
        }     // jet_rap
      }       // jet_pt
    }         // jetR
  }           // variable
}

void JetShape2D::GetJetShape(int iv, int ir, int ijp, int ijr, int ipp, int ipr)
{
  //----------------------------------------
  std::cout << "[JetShape2D] Calculate Jet Shape Function." << std::endl;
  //----------------------------------------
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, 0);
  auto hist = CreateHist("2d_" + hist_name, iv);
  hist->Init();
  hist->LoadHistFromFile("raw_");
  //--
  std::string me_hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, 1);
  auto me_hist = CreateHist("2d_" + me_hist_name, iv);
  me_hist->Init();
  me_hist->LoadHistFromFile("raw_");

  //----------------------------------------
  // Mixed Event Method
  //----------------------------------------

  // average in phi
  me_hist->AverageInX();

  double ix0 = me_hist->FindBinX(0.0);
  double iy0 = me_hist->FindBinY(0.0);
  //  std::cout
  //  << "x0=" << me_hist->GetX(ix0)
  //  << ", y0=" << me_hist->GetY(iy0)
  //  << std::endl;
  double me00 = me_hist->GetVal(ix0, iy0);
  double me00_err = me_hist->GetErr(ix0, iy0);
  //  std::cout
  //  << "M(0,0)=" << me00
  //  << ", Err_M(0,0)=" << me00_err
  //  << std::endl;
  me_hist->DivideWithError(me00, me00_err);

  me_hist->Print("scaled_");
  // hist->Divide(me_hist);
  me_hist->DeleteTH();
  hist->Print("corrected_acceptance_");
  //----------------------------------------

  //----------------------------------------
  // Sideband Subtraction
  //----------------------------------------
  auto sideband_hist = CreateHist("sideband_2d_" + hist_name, iv);
  sideband_hist->Init();
  sideband_hist->SetSidebandHist(hist, sidebandRap[0], sidebandRap[1]);
  sideband_hist->Print();
  //----------------------------------------
  hist->Add(sideband_hist, -1.0);
  sideband_hist->DeleteTH();
  hist->Print();
  //----------------------------------------

  //----------------------------------------
  // Jet Shape
  //----------------------------------------
  auto jetshape_hist = std::make_shared<Hist1D>(hist_name, binSettings[iv]);
  jetshape_hist->Init();

  auto jetshape_err2_hist = std::make_shared<Hist1D>("err2_" + hist_name, binSettings[iv]);
  jetshape_err2_hist->Init();

  int n_phi = hist->GetNbinsX();
  int n_eta = hist->GetNbinsY();

  for (int i_phi = 0; i_phi < n_phi; i_phi++)
  {
    for (int i_eta = 0; i_eta < n_eta; i_eta++)
    {

      double delta_phi = hist->GetX(i_phi);
      double delta_eta = hist->GetY(i_eta);
      double val = hist->GetVal(i_phi, i_eta);
      double err = hist->GetErr(i_phi, i_eta);
      double delta_r = sqrt(delta_phi * delta_phi + delta_eta * delta_eta);

      jetshape_hist->Fill(delta_r, val);
      jetshape_err2_hist->Fill(delta_r, err * err);
    }
  }
  hist->DeleteTH();

  jetshape_hist->SetErrors(jetshape_err2_hist);
  jetshape_err2_hist->DeleteTH();

  jetshape_hist->Scale(1.0, "width");
  jetshape_hist->Print("jetshape_");
  jetshape_hist->Normalize("width");
  // jetshape_hist->Print("normalized_jetshape_");
  jetshape_hist->DeleteTH();
}
