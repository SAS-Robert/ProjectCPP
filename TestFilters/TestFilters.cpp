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

#include "E50.h"
#include "E100.h"

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

// Christian's filters:
//Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47,
//        'StopbandFrequency1', 49, 'StopbandFrequency2', 51,
//        'PassbandFrequency2', 53, 'PassbandRipple1', 1,
//        'StopbandAttenuation', 60, 'PassbandRipple2', 1,
//        'SampleRate', 1000, 'DesignMethod', 'cheby2');
const int order50 = 10;
Iir::ChebyshevII::BandStop<order50> Cheby50;

const int orderC1 = 2;
Iir::ChebyshevI::BandStop<orderC1> Cheby_i_50;

//Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97,
//          'StopbandFrequency1', 99, 'StopbandFrequency2', 101,
//          'PassbandFrequency2', 103, 'PassbandRipple1', 1,
//          'StopbandAttenuation', 60, 'PassbandRipple2', 1,
//          'SampleRate', 1000, 'DesignMethod', 'cheby2');
const int order100 = 10;
Iir::ChebyshevII::BandStop<order100> Cheby100;
Iir::ChebyshevI::BandStop<orderC1> Cheby_i_100;

const int orderButty = 4; // 4th order (=2 biquads)
Iir::Butterworth::BandPass<orderButty> Butty;

// New filters:
// Filter: setup(samplingrate, Frquency , Frequency bandwidth);
const int orderB50_100 = 1;
const double B50_Fq = 50;
const double B100_Fq = 100;
const double B50_100_Fq = 5;

Iir::Butterworth::BandStop<orderB50_100> B50;
std::vector<double> B50_result;

Iir::Butterworth::BandStop<orderB50_100> B100;
std::vector<double> B100_result;

// Filters from MicroModeller:
E50Type* E50 = E50_create();
std::vector<float> E50_result;
E100Type* E100 = E100_create();
std::vector<float> E100_result;


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

    std::cout << "Setting up Christian's filters"<<endl;
    // Christian's filters
    //Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47,
    //        'StopbandFrequency1', 49, 'StopbandFrequency2', 51,
    //        'PassbandFrequency2', 53, 'PassbandRipple1', 1,
    //        'StopbandAttenuation', 60, 'PassbandRipple2', 1,
    //        'SampleRate', 1000, 'DesignMethod', 'cheby2');
    const double Cheby50_centerFrequency = 50; // Hz
    const double Cheby50_widthFrequency = 3;
    const double Cheby50_stopBandDb = 60;
    Cheby50.setup(samplingrate, Cheby50_centerFrequency, Cheby50_widthFrequency, Cheby50_stopBandDb);
    Cheby_i_50.setup(samplingrate, Cheby50_centerFrequency, Cheby50_widthFrequency, Cheby50_stopBandDb);

    //Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97,
    //          'StopbandFrequency1', 99, 'StopbandFrequency2', 101,
    //          'PassbandFrequency2', 103, 'PassbandRipple1', 1,
    //          'StopbandAttenuation', 60, 'PassbandRipple2', 1,
    //          'SampleRate', 1000, 'DesignMethod', 'cheby2');
    const double Cheby100_centerFrequency = 100; // Hz
    const double Cheby100_widthFrequency = 3;
    const double Cheby100_stopBandDb = 60;
    Cheby100.setup(samplingrate, Cheby100_centerFrequency, Cheby100_widthFrequency, Cheby100_stopBandDb);
    Cheby_i_100.setup(samplingrate, Cheby100_centerFrequency, Cheby100_widthFrequency, Cheby100_stopBandDb);

    const int orderButty = 4;
    double Low_Hz = 20;
    double High_Hz = 300;
    const double B_Fq = (High_Hz+Low_Hz)/2;
    const double B_Fqw = (High_Hz-Low_Hz);

    Butty.setup(1000, B_Fq, B_Fqw);

    B50.setup(1000, B50_Fq, B50_100_Fq);
    B100.setup(1000, B100_Fq, 6.5);


    // Load raw data: arm sessions
    //infile.open("CA_ind1_EMG_20201027_9000.txt");
    //infile.open("CA_ind2_EMG_20201027_9005.txt");
    //infile.open("CA_ind3_EMG_20201027_9010.txt");

    //infile.open("EMG_desk_20201027_9030.txt");
    //infile.open("EMG_desk_20201027_9035.txt");
    //infile.open("EMG_desk_20201027_9040.txt");

    // Load raw data: arm sessions
    string infileName = "CUL_leg_filter_20201104_1325.txt";
    // string infileName = "CUL_leg_filter_20201104_1330.txt";
    // string infileName = "CUL_leg_filter_20201104_1336.txt";
    // string infileName = "CUL_leg_filter_20201104_1340.txt";
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

    // Test data
    /*
    string fileName_t = fileDir + "raw_" + infileName;//+date_s.c_str() + ".txt";
    ofstream outFile_t;
    outFile_t.open(fileName_t);
    for (unsigned int i = 0; i < channel_raw.size(); i++) {
        raw_value = channel_raw[i];
        outFile_t << raw_value << "\n";
    }
    outFile_t.close();
    */
    // Filter data
    fileName = fileDir + "out2_" + infileName;//+date_s.c_str() + ".txt";
    outFile.open(fileName);

    std::cout << "Filtering data. Sample amount: "<< channel_raw.size() << endl;
    for(unsigned int i=0; i<channel_raw.size(); i++){
      raw_value = channel_raw[i];
      // Hasomed filters
      // temp[0] = m_f.filter(raw_value);
      // temp[1] = m_f1.filter(temp[0]);
      // temp[2] = m_f2.filter(temp[1]);
      // temp[3] = sqrt(temp[2] * temp[2]);
      // temp[4] = m_f3.filter(temp[3]);

      // Christians' filter
      //temp[10]= Butty.filter(raw_value);
      // temp[11]= Cheby50.filter(temp[10]);
      // temp[12]= Cheby100.filter(temp[11]);

      // New filters
      temp[10]= Butty.filter(raw_value);
      temp[11]= B50.filter(temp[10]);
      temp[12]= B100.filter(temp[11]);

      //temp[10] = Butty.filter(raw_value);

      //float temp11 = (float)temp[10];
      //E50_writeInput(E50, temp11);
      //E50_result.push_back(E50_readOutput(E50));
      //temp[11] = (double)E50_readOutput(E50);

      //E100_writeInput(E100, temp11);
      //E100_result.push_back(E100_readOutput(E100));
      //temp[12] = (double) E100_readOutput(E100);

      // Type I filters
      // emp[10]= Butty.filter(raw_value);
      // temp[11]= Cheby_i_50.filter(temp[10]);
      // temp[12]= Cheby_i_100.filter(temp[11]);

      // save it
      // Hasomed
      //outFile << raw_value<<","<< temp[0] << ", " << temp[1] << ", " << temp[2] << ", " << temp[3] << ", " << temp[4] << "\n";
      outFile << raw_value<<","<< temp[10] << ", " << temp[11] << "," << temp[12] << "\n";

    }
    E50_destroy(E50);
    E100_destroy(E100);

    outFile.close();
    std::cout<<"Output file: "<<fileName <<endl;

}

// others
// std::string s = data.c_str();
// std::string delimiter = ",";
// std::cout << s << endl;
// size_t pos = 0;
// std::string token;
// while ((pos = s.find(delimiter)) != std::string::npos) {
//     token = s.substr(0, pos);
//     std::cout << token << std::endl;
//     s.erase(0, pos + delimiter.length());
// }
// std::cout << s << std::endl;
