#include "JetShape2DNoMixedEvent.h"
#include "TMath.h"

//using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<JetShape2DNoMixedEvent> JetShape2DNoMixedEvent::reg("JetShape2DNoMixedEvent");

JetShape2DNoMixedEvent::JetShape2DNoMixedEvent(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

JetShape2DNoMixedEvent::~JetShape2DNoMixedEvent()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int JetShape2DNoMixedEvent::ReadOptionParametersFromXML()
{
  nBinPhi = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinPhi"});
  boundPhi = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundPhi", "Item"});
  nBinEta = SetXML::Instance()->GetElementInt({"observable", Name().c_str(), "nBinEta"});
  boundEta = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "boundEta", "Item"});
  sidebandRap = SetXML::Instance()->GetElementVectorDouble({"observable", Name().c_str(), "sidebandRap", "Item"}, false);

  return 4;
}

std::string JetShape2DNoMixedEvent::GetParamsTag(int i)
{

  if (i == 0)
  {
    return "e";
  }
  else if (i == 1)
  {
    return "n";
  }
  else if (i == 2)
  {
    return "pt";
  }
  else if (i == 3)
  {
    return "pz";
  }
  else
  {
    return "Error!";
  }
}

void JetShape2DNoMixedEvent::ShowParamsSetting()
{
  if (sidebandRap.size() != 0)
  {
    std::cout << "[JetShape2DNoMixedEvent ] ***-------------------------------------------" << std::endl;
    std::cout << "[JetShape2DNoMixedEvent ] *** [SideBand]" << std::endl;
    std::cout << "[JetShape2DNoMixedEvent ] *** " << sidebandRap[0] << " < |Eta| < " << sidebandRap[1] << std::endl;
  }
  else
  {
    sidebandRap.push_back(5000.0);
    sidebandRap.push_back(5000.0);
  }
}

std::shared_ptr<Histogram> JetShape2DNoMixedEvent::CreateHist(std::string hist_name, int iv)
{
  return std::make_shared<Hist2D>(hist_name, nBinPhi, boundPhi, nBinEta, boundEta);
}

void JetShape2DNoMixedEvent::SetObservable(fastjet::PseudoJet jet,
                                           std::vector<std::shared_ptr<Particle>> particle_list,
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

        std::array<int, 4> index;
        for (int i = 0; i < 4; i++)
        {
          index[i] = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, i);
          hist_list[index[i]]->JetTriggered();
        }
        //--
        for (auto p : particle_list)
        {
          if (ParticleTrigger(p, ipp, ipr))
          {

            std::array<double, 4> val = {
                sub_ptr->eSub(p),
                sub_ptr->nSub(p),
                sub_ptr->ptSub(p),
                sub_ptr->pzSub(p),
            };

            double delta_eta = p->eta() - jet.eta();
            double delta_phi = jet.delta_phi_to(p->GetPseudoJet());

            for (int i = 0; i < 4; i++)
            {
              hist_list[index[i]]->Fill(delta_phi, delta_eta, val[i]);
            }
          }
        }
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      }
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JetShape2DNoMixedEvent::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
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
    std::cout << "[JetShape2DNoMixedEvent] 0-total Jet" << std::endl;
    std::cout << "[JetShape2DNoMixedEvent] Skip. " << std::endl;
    return;
  }

  total_hist->Scale(1.0 / nJetTotal);
  total_hist->Print();

  total_hist->DeleteTH();
}

void JetShape2DNoMixedEvent::CombineFinisher()
{
  std::cout << "[JetShape2DNoMixedEvent] Finisher" << std::endl;
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
              for (int ip = 0; ip < nParams; ip++)
              {
                GetJetShape(iv, ir, ijp, ijr, ipp, ipr, ip);
              } //option parameters
            }   //had_rap
          }     //had_pt
        }       //jet_rap
      }         //jet_pt
    }           //jetR
  }             //variable
}

void JetShape2DNoMixedEvent::GetJetShape(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{
  //----------------------------------------
  std::cout << "[JetShape2DNoMixedEvent] Calculate Jet Shape Function." << std::endl;
  //----------------------------------------
  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  auto hist = CreateHist("2d_" + hist_name, iv);
  hist->Init();
  hist->LoadHistFromFile();

  //----------------------------------------
  //Sideband Subtraction
  //----------------------------------------
  if (sidebandRap.size() != 0)
  {
    auto sideband_hist = CreateHist("sideband_2d_" + hist_name, iv);
    sideband_hist->Init();
    sideband_hist->SetSidebandHist(hist, sidebandRap[0], sidebandRap[1]);
    sideband_hist->Print();
    //----------------------------------------
    hist->Add(sideband_hist, -1.0);
    sideband_hist->DeleteTH();
    hist->Print();
    //----------------------------------------
  }
  //----------------------------------------
  //Jet Shape
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
  jetshape_hist->Print("normalized_jetshape_");
  jetshape_hist->DeleteTH();
}
