// Parse JETSCAPE results from Yasuki into ROOT file
// Author: Ezra Lesser, June 2023

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

void jetscape_parser() {

    const std::string gr_settings[] = { "un", "" };
    //const std::string jetRs[] = { "0.2" }
    std::string jetR = "0.2";
    const int pTbins[] = { 40, 60, 80, 100, 150 };
    const std::string types[] = { "pp", "AA" };
    const std::vector<std::string> centralities { "0-5", "5-10" };

    const std::vector<std::vector<float>> binnings_ungr {
        { 0, 1.2, 1.6, 2, 2.4, 2.8, 3.2, 3.6, 4, 4.4, 5, 5.6, 6.2, 7 },
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        { 0, 2, 4, 6, 8, 12 },
        { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 16 },
    };
    const std::vector<std::vector<float>> binnings_gr {
        { 0, 0.3, 0.6, 0.9, 1.2, 1.6, 2, 2.4, 2.8, 3.2, 3.6, 4, 4.6, 5.2, 6, 7 },
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        { 0, 1.8, 3.6, 5.8, 8, 12 },
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 16 }
    };

    TFile outf("./JETSCAPE_results.root", "recreate");

    for ( const std::string & gr_setting : gr_settings ) {
        for ( int i_pTbin = 0; i_pTbin < sizeof(pTbins) / sizeof(int) - 1; i_pTbin++ ) {
            const int & pTmin = pTbins[i_pTbin];
            const int & pTmax = pTbins[i_pTbin+1];
            for ( const std::string & type : types ) {

                // Get proper binning for this observable / bin
                std::vector<float> binning;
                if (gr_setting.empty()) binning = binnings_gr[i_pTbin];
                else binning = binnings_ungr[i_pTbin];
                float binning_arr[binning.size()];
                std::copy(binning.begin(), binning.end(), binning_arr);

                // Create new TH1s for output
                TH1F* htemp = new TH1F("htemp", "htemp", binning.size() - 1, binning_arr);
                TH1F* h_out = nullptr;
                std::string h_out_name = "h_chjet_mass_" + gr_setting + "groomed_alice_R"
                    + jetR + "_pt" + std::to_string(pTmin) + "-" + std::to_string(pTmax)
                    + "_5020_0-10_" + type;

                for ( const std::string & centrality : centralities ) {
                    // Open appropriate file
                    std::string basedir = "./";
                    if (gr_setting.empty()) basedir += "ChargedMg/";
                    else basedir += "ChargedMass/";
                    basedir += "5020_";
                    if (type == "AA") basedir += "PbPb" + centrality;
                    else basedir += "PP";
                    if (gr_setting.empty()) basedir += "/SoftDropGroom_hist_total_SoftDropGroom_mG";
                    else basedir += "/jetmass_hist_total_JetMass_m";
                    if (i_pTbin) basedir += std::to_string(i_pTbin + 1);
                    basedir += "_jetr" + jetR + "_ptj" + std::to_string(pTmin) + "-"
                        + std::to_string(pTmax) + "_rapj0.0-0.7_pt0.0-2510.0_rap0.0-2.0";
                    if (gr_setting.empty()) basedir += "_beta0.00_zCut0.20";
                    basedir += ".txt";

                    // Open predictions file for testing
                    std::ifstream inf;
                    inf.open(basedir);
                    if ( ! inf.is_open() ) {
                        std::cerr << "Unable to open file: " << basedir << std::endl;
                        throw 1;
                    }

                    std::string line;
                    std::getline(inf, line); // throw out first line (just a comment)
                    std::vector<float> x_center, x_low, x_high, value, error, n_jet, n_event;
                    float holder = -1;
                    while(inf) {
                        inf >> holder;
                        if (!holder) break;
                        x_center.push_back(holder);
                        inf >> holder;
                        x_low.push_back(holder);
                        inf >> holder;
                        x_high.push_back(holder);
                        inf >> holder;
                        value.push_back(holder);
                        inf >> holder;
                        error.push_back(holder);
                        inf >> holder;
                        n_jet.push_back(holder);
                        inf >> holder >> std::ws;
                        n_event.push_back(holder);
                    }

                    // Double-check that binning is correct
                    x_low.push_back(x_high.back());
                    if ( x_low.size() != binning.size() ) {
                        std::cerr << "Expected size " << binning.size()
                            << ", got size " << x_low.size() << std::endl;

                        std::cout << "Size expected: " << binning.size() << std::endl;
                        for (int i = 0; i < binning.size(); i++) {
                            std::cout << binning[i] << ' ';
                        }
                        std::cout << std::endl;

                        std::cout << "Size found: " << x_low.size() << std::endl;
                        for (int i = 0; i < x_low.size(); i++) {
                            std::cout << x_low[i] << ' ';
                        }
                        std::cout << std::endl;

                        throw 2;
                    }
                    for (int i = 0; i < x_low.size(); i++) {
                        // Check that binning is correct again
                        if (x_low[i] - binning[i] > 0.0001 || x_low[i] - binning[i] < -0.0001) {
                            std::cerr << "Expected value " << binning[i]
                                << ", got value " << x_low[i] << std::endl;
                            throw 2;
                        }

                        // Copy values to histogram
                        htemp->SetBinContent(i, value[i]);
                        htemp->SetBinError(i, error[i]);
                    }

                    // Create hist to save if necessary, otherwise update
                    if (!h_out) {
                        h_out = (TH1F*) htemp->Clone(h_out_name.c_str());
                        h_out->SetNameTitle(h_out_name.c_str(), h_out_name.c_str());
                    } else {
                        h_out->Add(htemp);
                        //for (int i = 0; i < x_low.size(); i++)
                        //    std::cout << h_out->GetBinContent(i) << " ";
                        //std::cout << std::endl;
                    }
                } // centrality

                // Average h_out
                h_out->Scale(1. / centralities.size());
                h_out->Write();

                delete htemp;

            } // type
        } // pT_bin
    } // gr_setting

}
