#include "JetScapeAscii.h"

// Register the module with the base class
RegisterLoadFileModule<JetScapeAscii> JetScapeAscii::reg("JetScapeAscii");

void JetScapeAscii::TestStringStream()
{
  std::stringstream test_stream;
  std::string test_line;

  test_stream
      << "Test Line 1\n"
      << "Test Line 2\n"
      << "Test Line 3\n"
      << "Test Line 4\n"
      << "Test Line 5\n";

  std::cout << "teststream" << test_stream.str() << std::endl;

  while (getline(test_stream, test_line))
  {
    std::cout << "testline0: " << test_line << endl;
  }

  test_stream.clear();
  test_stream.seekg(0, std::ios::beg);

  getline(test_stream, test_line);
  std::cout << "testline2: " << test_line << endl;

  getline(test_stream, test_line);
  std::cout << "testline2: " << test_line << endl;

  test_stream.clear();
  test_stream.seekg(0, std::ios::beg);

  getline(test_stream, test_line);
  std::cout << "testline3: " << test_line << endl;

  std::cout << "testline3-1: " << test_line << endl;
  test_line.clear();
  test_line.shrink_to_fit();
  std::cout << "testline3-2: " << test_line << endl;

  getline(test_stream, test_line);
  std::cout << "testline3: " << test_line << endl;

  std::cout << "==============: " << endl;
  exit(-1);
}

JetScapeAscii::JetScapeAscii(std::string name_in) : name(name_in)
{
  std::cout << "-@-Creating JetScapeAscii" << std::endl;
  // TestStringStream();
}

JetScapeAscii::~JetScapeAscii()
{
  std::cout << "-$-Deleting JetScapeAscii" << std::endl;
}

bool JetScapeAscii::Load(std::string input_filename)
{

  Clear();
  std::cout << "[JetScapeAscii] Load: " << input_filename << std::endl;

  std::ifstream ifs;
  ifs.open(input_filename.c_str()); // open the input file
  input_str_stream << ifs.rdbuf();  // read the file

  if (ifs.is_open())
  {
    ifs.close();
  }
  else
  {
    ifs.close();
    std::cout << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << "[JetScapeAscii] InputFile: " << input_filename << " is not found" << std::endl;
    std::cout << "[JetScapeAscii] Skip. " << std::endl;
    std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
    std::cout << std::endl;
    return false;
  }

  std::cout << "[JetScapeAscii]  File Loaded. " << std::endl;
  return true;
}

void JetScapeAscii::Clear()
{
  //  std::cout << "[JetScapeAscii] Clear." <<std::endl;
  //
  //  std::cout << "[JetScapeAscii] Before Clear." <<std::endl;
  //  std::cout << "input_line: " << input_line <<std::endl;
  input_str_stream.str("");
  input_str_stream.clear(std::stringstream::goodbit);
  input_line.clear();
  input_line.shrink_to_fit();
  getLineStart = 0;
  //  std::cout << "[JetScapeAscii] After Clear." <<std::endl;
  //  std::cout << "input_line: " << input_line <<std::endl;
}

void JetScapeAscii::Reset()
{
  //  std::cout << "[JetScapeAscii] Reset." <<std::endl;
  //
  //  std::cout << "[JetScapeAscii] Before Reset." <<std::endl;
  //  std::cout << "input_line: " << input_line <<std::endl;
  //
  input_str_stream.clear();
  input_str_stream.seekg(0, std::ios::beg);
  input_line.clear();
  input_line.shrink_to_fit();
  getLineStart = 0;
  //
  //  std::cout << "[JetScapeAscii] After Reset." <<std::endl;
  //  std::cout << "input_line: " << input_line <<std::endl;
}

int JetScapeAscii::GetLine()
{
  if (getline(input_str_stream, input_line))
  {
    // std::cout << ">>" << input_line <<std::endl;
    return 1;
  }
  else
  {
    // std::cout << "last?" << input_line <<std::endl;
    return 0;
  }
}

void JetScapeAscii::ShowLine()
{
  std::cout << "[JetScapeAscii] Current Line: " << input_line << std::endl;
}

int JetScapeAscii::EventEnd()
{
  if (input_line.find("#") != std::string::npos)
  {

    if (getLineStart == 1)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    getLineStart = 1;
    return 0;
  }
}

int JetScapeAscii::ValidLine()
{
  if (getLineStart == 1 && !input_line.empty())
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

std::shared_ptr<Particle> JetScapeAscii::GetParticle()
{

  sscanf(input_line.data(),
         "%d %d %d %lf %lf %lf %lf %lf %lf",
         &SN, &PID, &Status, &E, &Px, &Py, &Pz, &Eta, &Phi);
  FourVector p(Px, Py, Pz, E);

  return std::make_shared<Particle>(SN, PID, Status, Status, p);
}

void JetScapeAscii::LoadSigma(std::string sigma_filename, double &sigma, double &sigma_err)
{
  // Read file
  std::ifstream ifs;
  std::stringstream str_stream;

  ifs.open(sigma_filename.c_str()); // open the input file
  str_stream << ifs.rdbuf();        // read the file
  if (ifs.is_open())
  {
    ifs.close();
  }
  else
  {
    ifs.close();
    std::cout << "[JetScapeAscii] " << sigma_filename << " NOT FOUND" << std::endl;
    return;
  }

  std::string str;

  while (getline(str_stream, str))
  {

    if (str.find("#") == std::string::npos && !str.empty())
    {

      sscanf(str.data(),
             "%lf %lf",
             &sigma, &sigma_err);
      break;
    }
  }
}
