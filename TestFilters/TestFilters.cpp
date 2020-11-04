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

// Christian's filters:
//Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47,
//        'StopbandFrequency1', 49, 'StopbandFrequency2', 51,
//        'PassbandFrequency2', 53, 'PassbandRipple1', 1,
//        'StopbandAttenuation', 60, 'PassbandRipple2', 1,
//        'SampleRate', 1000, 'DesignMethod', 'cheby2');
const int order50 = 10;
Iir::ChebyshevII::BandStop<order50> Cheby50;

//Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97,
//          'StopbandFrequency1', 99, 'StopbandFrequency2', 101,
//          'PassbandFrequency2', 103, 'PassbandRipple1', 1,
//          'StopbandAttenuation', 60, 'PassbandRipple2', 1,
//          'SampleRate', 1000, 'DesignMethod', 'cheby2');
const int order100 = 10;
Iir::ChebyshevII::BandStop<order100> Cheby100;

const int orderButty = 4; // 4th order (=2 biquads)
Iir::Butterworth::BandPass<orderButty> Butty;

// Global variables:
float temp[30] = { 0 };
double values[5] = { 0 };
std::vector<float> channel_raw;
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
    const float samplingrate = 4000; // Hz
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

    //Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97,
    //          'StopbandFrequency1', 99, 'StopbandFrequency2', 101,
    //          'PassbandFrequency2', 103, 'PassbandRipple1', 1,
    //          'StopbandAttenuation', 60, 'PassbandRipple2', 1,
    //          'SampleRate', 1000, 'DesignMethod', 'cheby2');
    const double Cheby100_centerFrequency = 100; // Hz
    const double Cheby100_widthFrequency = 3;
    const double Cheby100_stopBandDb = 60;
    Cheby100.setup(samplingrate, Cheby100_centerFrequency, Cheby100_widthFrequency, Cheby100_stopBandDb);

    const double Low_Hz = 30;
    const double High_Hz = 200;
    const double Butty_centerFrequency = (High_Hz+Low_Hz)/2;
    const double Butty_widthFrequency = (High_Hz-Low_Hz);
    Butty.setup(samplingrate, Butty_centerFrequency, Butty_widthFrequency);
    // Load raw data
    //infile.open("CA_ind1_EMG_20201027_9000.txt");
    //infile.open("CA_ind2_EMG_20201027_9005.txt");
    //infile.open("CA_ind3_EMG_20201027_9010.txt");

    //infile.open("EMG_desk_20201027_9030.txt");
    //infile.open("EMG_desk_20201027_9035.txt");
    infile.open("EMG_desk_20201027_9040.txt");


    std::cout << "Reading from the file" << endl;
    string data0, data1, data2, data3, data4;
    while(!infile.eof()){
      infile >> data0 >> data1 >> data2 >> data3 >> data4;
      values[0] = std::stod(data0);
      values[1] = std::stod(data1);
      values[2] = std::stod(data2);
      values[3] = std::stod(data3);
      values[4] = std::stod(data4);
      channel_raw.push_back(values[1]);
    }
    infile.close();
    std::cout << "Raw data loaded." << endl;
    //std::cout << "Values format: "<<values[0]<<"," << values[1]<<"," << values[2]<<"," << values[3]<<"," << values[4]<< endl;

    // Filter data
    fileName = fileDir + "desk3_Hasomed.txt";//+date_s.c_str() + ".txt";
    outFile.open(fileName);

    float raw_value = 0;
    std::cout << "Filtering data. Sample amount: "<< channel_raw.size() << endl;
    for(int i=0; i<channel_raw.size(); i++){
      raw_value = channel_raw[i];
      // Hasomed filters
      temp[0] = m_f.filter(raw_value);
      temp[1] = m_f1.filter(temp[0]);
      temp[2] = m_f2.filter(temp[1]);
      temp[3] = sqrt(temp[2] * temp[2]);
      temp[4] = m_f3.filter(temp[3]);

      // Christians' filter
      temp[10]= Butty.filter(raw_value);
      temp[11]= Cheby50.filter(temp[10]);
      temp[12]= Cheby100.filter(temp[11]);

      // save it
      // Hasomed
      //outFile << raw_value<<","<< temp[0] << ", " << temp[1] << ", " << temp[2] << ", " << temp[3] << ", " << temp[4] << "\n";
      outFile << raw_value<<","<< temp[10] << ", " << temp[11] << "," << temp[12] << "\n";

    }

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
