#include "InconeQuant.h"
#include "TMath.h"

// using namespace Analysis;

// Register the module with the base class
RegisterAnalysisModule<InconeQuant> InconeQuant::reg("InconeQuant");

InconeQuant::InconeQuant(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating " << name << std::endl;
}

InconeQuant::~InconeQuant()
{
  std::cout << "-$-Deleting " << name << std::endl;
}

int InconeQuant::ReadOptionParametersFromXML()
{
  int n_val_total = 0;

  //--------------------------------------------
  // Products (commutative!)
  for (int iq = 0; iq < n_quant; iq++)
  {
    std::cout
        << "[InconeQuant] "
        << "Added ";
    // start from iq since the product is commutative
    for (int jq = iq; jq < n_quant; jq++)
    {
      n_val_total++;
      //-------------------------------
      // To check the index conversion
      int index = GetParamIndex(iq, jq);
      std::cout
          << index
          << "("
          << GetParamIndex(index)[0]
          << ","
          << GetParamIndex(index)[1]
          << "): "
          << "<"
          << GetParamsTag(index)
          << ">\t";
      //-------------------------------
    }
    std::cout << std::endl;
  }
  //--------------------------------------------
  // exit(-1);
  return n_val_total;
}

//-------------------------------------------
// Get Tags for Parameters
std::string InconeQuant::GetParamsTag(int i)
{
  return GetParamsTag(GetParamIndex(i));
}
std::string InconeQuant::GetParamsTag(std::array<int, 2> i)
{
  return GetParamsTag(i[0], i[1]);
}
std::string InconeQuant::GetParamsTag(int i_q, int i_q2)
{
  return GetParamsTag(c_quant[i_q], c_quant[i_q2]);
}

std::string InconeQuant::GetParamsTag(std::string q1, std::string q2)
{
  // Create Tag for the quantity
  std::string tag = q1;
  tag += "_";
  tag += q2;
  return tag;
}
//------------------------------------------------------------
// Get Index of Tags for Parameters
int InconeQuant::GetParamIndex(std::array<int, 2> i)
{
  return GetParamIndex(i[0], i[1]);
}

int InconeQuant::GetParamIndex(int i_q1, int i_q2)
{
  if (i_q1 > i_q2)
  {
    std::cout << "[InconeQuant] Bad Index of Variable Product (GetParamIndex)\n";
    exit(-1);
  }
  //
  // int zeroth = n_quant;
  // int i_q1th = n_quant - (i_q1 - 1);
  // int sum = (zeroth+i_q1th)*(i_q1)/2;
  // int j = i_q2-i_q1;
  // int index = sum + j;
  // std::cout<< "[InconeQuant] GetParamIndex Exit\n";exit(-1);
  //
  return (2 * n_quant - i_q1 + 1) * i_q1 / 2 + i_q2 - i_q1;
}

std::array<int, 2> InconeQuant::GetParamIndex(int i)
{
  int i_q1 = -1;
  int i_q2 = -1;

  for (int iq = 0; iq < n_quant; iq++)
  {
    for (int jq = iq; jq < n_quant; jq++)
    {
      if (i == GetParamIndex(iq, jq))
      {
        i_q1 = iq;
        i_q2 = jq;
        goto EXIT_RETURN_INDICES;
      }
    }
  }
  if (i_q1 < 0 || i_q2 < 0)
  {
    std::cout << "[InconeQuant] Error! Exit" << std::endl;
    exit(-1);
  }
EXIT_RETURN_INDICES:
  return std::array<int, 2>{i_q1, i_q2};
}
//------------------------------------------------------------

void InconeQuant::SetObservable(int i_tag_particle, fastjet::PseudoJet jet,
                                std::vector<std::shared_ptr<Particle>> particle_list,
                                int ir, int ijp, int ijr)
{
  // --------------------------------------------
  // Jet-pT -------------------------------------
  double jpt = jet.perp();
  //--------------------------------------------------------------------------------------------------
  for (int iv = 0; iv < variables.size(); iv++)
  {
    for (int ipp = 0; ipp < particlePtMin.size(); ipp++)
    {
      for (int ipr = 0; ipr < particleRapMin.size(); ipr++)
      {

        // --------------------------------------------
        // Initialize the quantities count(jet number), charge, baryon, strange, charm
        std::array<double, n_quant> quantities{1.};
        // for(int i =0; i<n_quant; i++){
        //   std::cout<< quantities[i] << " ";
        // }std::cout<<std::endl;
        for (auto p : particle_list)
        {
          if (ParticleTrigger(p, ipp, ipr))
          {
            //-----------------------
            // Get Radial Distance---
            double delta_eta = p->eta() - jet.eta();
            double delta_phi = jet.delta_phi_to(p->GetPseudoJet());
            double delta_r = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
            //-----------------------
            if (delta_r <= jetR[ir])
            {
              quantities[1] += sub_ptr->chargeSub(p);
              quantities[2] += sub_ptr->baryonSub(p);
              quantities[3] += sub_ptr->strangeSub(p);
              quantities[4] += sub_ptr->charmSub(p);

              quantities[5] += 1.;
              quantities[6] += (double)(p->chargeType() != 0);
              quantities[7] += (double)(p->baryonType() != 0);
              quantities[8] += (double)(p->strangeType() != 0);
              quantities[8] += (double)(p->charmType() != 0);
              // For debug -----------------
              //  if(p->pid()==130||p->pid()==310){
              // std::cout
              //     << "PID: " << p->pid()
              //     << ": " << quantities[0]
              //     << ": Q pile =" << quantities[1]
              //     << " B pile =" << quantities[2]
              //     << " S pile =" << quantities[3]
              //     << " Charm pile =" << quantities[4]
              //     << std::endl;
              // }
              //---------------------------

            } // in-cone
              //-----------------------
          }   // particle trigger
        }     // particle_list
              // --------------------------------------------

        // --------------------------------------------
        // Fill Histograms
        for (int iq = 0; iq < n_quant; iq++)
        {
          for (int jq = iq; jq < n_quant; jq++)
          {
            int ii = GetHistIndex(iv, ir, ijp, ijr, ipp, ipr, GetParamIndex(iq, jq));
            // hist_list[ii]->Show();
            hist_list[ii]->JetTriggered();
            // hist_list[ii]->Fill(quantities[iq] * quantities[jq]);
            hist_list[ii]->Fill(jpt, quantities[iq] * quantities[jq]);
            // std::cout
            //     << "JetPt = " << jpt << " GeV, "
            //     << GetParamsTag(iq, jq)
            //     << " = " << quantities[iq] * quantities[jq]
            //     << std::endl;
            // hist_list[ii]->Show();
          }
        }
        // --------------------------------------------
      } // ipr
    }   // ipp
  }     // iv
  //--------------------------------------------------------------------------------------------------
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void InconeQuant::CombineHist(int iv, int ir, int ijp, int ijr, int ipp, int ipr, int ip)
{

  std::string hist_name = GetHistName(iv, ir, ijp, ijr, ipp, ipr, ip);
  std::cout << "[InconeQuant] hist_name = " << hist_name << std::endl;

  auto total_hist = CreateHist(hist_name, iv);
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
  total_hist->Print("count_");
  total_hist->DeleteTH();
}
