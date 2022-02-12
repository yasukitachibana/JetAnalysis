#include <iostream>
#include "JetAnalysis.h"

//#######################################################
bool run_code = false;
std::string xml_filename;
std::string input_dirname;
std::string output_dirname;
//#######################################################
void Show();
void CheckArg(int argc, char** argv);
//#######################################################
int main(int argc, char** argv)
{
  
  int StartTime = time(NULL);
  
  //#######################################################
  Show();
  CheckArg(argc, argv);
  //#######################################################
  
  //#######################################################
  // Execution
  //#######################################################
  if(run_code){
  JetAnalysis ja(xml_filename,input_dirname,output_dirname);
  ja.Init();
  ja.Exec();
  }
  //#######################################################
  
  //#######################################################
  std::cout << std::endl;
  std::cout<<"#########################################################"<<std::endl;
  int EndTime = time(NULL);
  int Hour = (EndTime-StartTime)/3600;
  int Minute = ((EndTime-StartTime)/60)-Hour*60;
  int Second = (EndTime-StartTime)-Hour*60*60 - Minute*60;
  std::cout<<"Programme run time = "<<Hour<<"::"<<Minute<<"::"<<Second<<std::endl;
  std::cout<<"#########################################################"<<std::endl;
  std::cout << std::endl;
  return 0;
}

void CheckArg(int argc, char** argv){
  std::cout <<  "=========================================================" << std::endl;
  if( argc > 1 && !strcmp(argv[1],"test") ){
    std::cout << "-->test mode<--" << std::endl;
    run_code = true;
    xml_filename = "../config/test.xml";
    input_dirname = "../test_data";
    output_dirname = "./";
  }else if(argc > 3){
    std::cout << "-->run code<--" << std::endl;
    run_code = true;
    xml_filename = argv[1];
    input_dirname = argv[2];
    output_dirname = argv[3];
  }else{
    run_code = false;
    std::cout << "Please Specify XML File, input directory and output directory" << std::endl;
    std::cout << ".Main [XML File] [input directory] [output directory]" << std::endl;
  }
  std::cout <<  "=========================================================" << std::endl;
}

void Show(){
  std::cout<<"#########################################################"<<std::endl;
  std::cout << std::endl;
  std::cout << "---------------------" << std::endl;
  std::cout << "- Jet Analysis Code -" << std::endl;
  std::cout << "  Yasuki  Tachibana " << std::endl;
  std::cout << "---------------------" << std::endl;
  std::cout << std::endl;
}

