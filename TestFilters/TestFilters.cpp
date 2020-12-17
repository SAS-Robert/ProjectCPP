// TestFilters.cpp : To test filters' efficiency offline

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <sstream>
#include <ctype.h>
#include <vector>
#include <complex>
#include <math.h>

#include <string>
#include <algorithm>
#include <iterator>

#include "Iir.h"


using namespace std;

// Hasomed example filters
const int order = 8; // 4th order (=2 biquads)
Iir::Butterworth::LowPass<order> m_f;

const int order1 = 4; // 4th order (=2 biquads)
Iir::Butterworth::BandStop<order1> m_f1;

const int order2 = 4; // 4th order (=2 biquads)
Iir::Butterworth::HighPass<order2> m_f2;

const int order3 = 4; // 4th order (=2 biquads)
Iir::Butterworth::LowPass<order2> m_f3;

// SAS filters:
// Filters from iir.h library. All the filters require the following parameters:
// - Nr of order
// - Sampling rate
// - Central frequency (Hz)
// - Frequency width (Hz)
// - Most of these parameters are calculated from Matlab's original scripts
const int samplingrate = 1000;

const int orderButty = 4;
double Low_Hz = 20;
double High_Hz = 300;
const double B_Fq = (High_Hz + Low_Hz) / 2;
const double B_Fqw = (High_Hz - Low_Hz);
Iir::Butterworth::BandPass<orderButty> Butty;
std::vector<double> Butty_result;

const double B50_Fq = 50;
const double B100_Fq = 100;
const double B150_Fq = 150;
const double B200_Fq = 200;
const double B250_Fq = 250;

const double B50_100_Fqw = 10;
const int orderB50_100 = 2;

Iir::Butterworth::BandStop<orderB50_100> B50;
std::vector<double> B50_result;

Iir::Butterworth::BandStop<orderB50_100> B100;
std::vector<double> B100_result;

Iir::Butterworth::BandStop<orderB50_100> B150;
std::vector<double> B150_result;

Iir::Butterworth::BandStop<orderB50_100> B200;
std::vector<double> B200_result;

Iir::Butterworth::BandStop<orderB50_100> B250;
std::vector<double> B250_result;

// Lastest
const int order_N100 = 8;
const double N100_Fq = 100;
double N100_ripple = 40;

Iir::ChebyshevII::LowPass<order_N100> N100;
std::vector<double> N100_result;

// Global variables:
double temp[30] = { 0 };
double values[5] = { 0 };
std::vector<double> channel_raw;
ifstream infile;
ofstream outFile;

// Functions
void get_dir(int argc, char *argv[], string& Outdir){
  //Gets the current directory of the programme, but for files
  std::stringstream sst(argv[0]);
  //char delimeter='/'; //for Linux
  char delimeter='\\'; //for Windows
  std::string part, full="", part_prev="";
  while (std::getline(sst, part, delimeter))
  {
      full+=part_prev;
      part_prev=part+delimeter;
  }

  // Cleaning unnecessary parts
  std::string t = full;
  std::string s = "\\Release";
  std::string::size_type i = t.find(s);
  if (i != std::string::npos)
   t.erase(i, s.length());

  s = "\\Debug";
  std::string::size_type j = t.find(s);
  if (j != std::string::npos)
   t.erase(j, s.length());

  s = "\\x64";
  std::string::size_type k = t.find(s);
  if (k != std::string::npos)
   t.erase(k, s.length());

  // Output
  full = t;
  full+="output\\";
  Outdir = full;
}
void generate_date(char* outStr){
//void do not return values and a char array is a message
//So the function gets a pointer to a global char array to write the date
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[15];
  //char output;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime(buffer, sizeof(buffer),"%Y%m%d_%H%M", timeinfo);
  for(int i=0; i < 15; ++i){
    outStr[i] = buffer[i];
  }
}
// converts character array
// to string and returns it
string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size-1; i++) {
        s = s + a[i];
    }
    return s;
}
// -------------------------------------- main ------------------------------
int main(int argc, char* argv[]) {
    // initialize files names
    string fileDir, fileName, date_s;
    char date[15];
    get_dir(argc, argv, fileDir);
    generate_date(date);
    date_s = convertToString(date,sizeof(date));
    std::cout << "date_s" << date_s << endl;
    std::cout << "This just a program to test filters\n===================================="<<endl;

    std::cout << "Setting up Hasomed example filters"<<endl;
    // Filter ini
    const float samplingrate = 1000; // Hz
    const float cutoff_frequency = 90; // Hz
    m_f.setup(samplingrate, cutoff_frequency);

    const float cutoff_frequency1 = 50; // Hz
    const float width_frequency = 8;
    m_f1.setup(samplingrate, cutoff_frequency1, width_frequency);

    const float cutoff_frequency2 = 5; // Hz
    m_f2.setup(samplingrate, cutoff_frequency2);

    const float cutoff_frequency3 = 2; // Hz
    m_f3.setup(samplingrate, cutoff_frequency3);

    std::cout << "Setting up SAS filters"<<endl;

    // Start SAS filters
    Butty.setup(samplingrate, B_Fq, B_Fqw);
    B50.setup(samplingrate, B50_Fq, B50_100_Fqw);
    B100.setup(samplingrate, B100_Fq, B50_100_Fqw);
    B150.setup(samplingrate, B150_Fq, B50_100_Fqw);
    B200.setup(samplingrate, B200_Fq, B50_100_Fqw);
    B250.setup(samplingrate, B250_Fq, B50_100_Fqw);

    N100.setup (samplingrate, N100_Fq, N100_ripple);
    // Load raw data: arm sessions
    // string infileName = "CA_filter_20201113_1503.txt";
    // string infileName = "CA_filter_20201113_1506.txt";
    // string infileName = "CA_filter_20201113_1509.txt";
    string infileName = "CUL_leg_filter_20201104_1336.txt";
    // string infileName = "CUL_leg_filter_20201104_1330.txt";

    infile.open(infileName);

    std::cout << "Reading from the file" << endl;
    string data0, data1, data2, data3, data4;
    while(!infile.eof()){
      // Hasomed storing process
      // infile >> data0 >> data1 >> data2 >> data3 >> data4;
      // SAS Programme storage process
        infile >> data0;// >> data1 >> data2 >> data3;
      values[0] = std::stod(data0);
      //values[1] = std::stod(data1);
      //values[2] = std::stod(data2);
      //values[3] = std::stod(data3);
      //values[4] = std::stod(data4);
      channel_raw.push_back(values[0]);
    }
    infile.close();
    std::cout << "Raw data loaded." << endl;
    //std::cout << "Values format: "<<values[0]<<"," << values[1]<<"," << values[2]<<"," << values[3]<<"," << values[4]<< endl;
    double raw_value = 0;

    // Filter data
    fileName = fileDir + "out_" + infileName;//+date_s.c_str() + ".txt";
    outFile.open(fileName);

    std::cout << "Filtering data. Sample amount: "<< channel_raw.size() << endl;
    for(unsigned int i=0; i<channel_raw.size(); i++){
      raw_value = channel_raw[i]*1000;
      // SAS Filtering
      Butty_result.push_back(Butty.filter(raw_value));
      B50_result.push_back(B50.filter(Butty_result[i]));
      B100_result.push_back(B100.filter(B50_result[i]));
      B150_result.push_back(B150.filter(B100_result[i]));
      B200_result.push_back(B200.filter(B150_result[i]));
      B250_result.push_back(B250.filter(B200_result[i]));
      // Lastest filtering
      // N100_result.push_back(N100.filter(B100_result[i]));
      // Saving data
      outFile << raw_value << "," << Butty_result[i] << "," << B50_result[i] << "," << B100_result[i] << "," << B150_result[i] << "," << B200_result[i] << "," << B250_result[i] << "\n";

    }

    outFile.close();
    std::cout<<"Output file: "<<fileName <<endl;

}
