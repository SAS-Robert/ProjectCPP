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

// Filters
const int order = 8; // 4th order (=2 biquads)
Iir::Butterworth::LowPass<order> m_f;

const int order1 = 4; // 4th order (=2 biquads)
Iir::Butterworth::BandStop<order1> m_f1;

const int order2 = 4; // 4th order (=2 biquads)
Iir::Butterworth::HighPass<order2> m_f2;

const int order3 = 4; // 4th order (=2 biquads)
Iir::Butterworth::LowPass<order2> m_f3;

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
    for (i = 0; i < size; i++) {
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
    std::cout << "This just a program to test filters\n===================================="<<endl;

    std::cout << "Hasomed GmbH example filters:"<<endl;
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

    // Load raw data
    infile.open("file_filtered_20201020_1543.txt");
    std::cout << "Reading from the file" << endl;
    string data0, data1, data2, data3, data4;
    while(!infile.eof()){
      infile >> data0 >> data1 >> data2 >> data3 >> data4;
      values[0] = std::stod(data0);
      values[1] = std::stod(data1);
      values[2] = std::stod(data2);
      values[3] = std::stod(data3);
      values[4] = std::stod(data4);
      channel_raw.push_back(values[0]);
    }
    infile.close();
    std::cout << "Raw data loaded." << endl;
    //std::cout << "Values format: "<<values[0]<<"," << values[1]<<"," << values[2]<<"," << values[3]<<"," << values[4]<< endl;

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


    // Filter data
    fileName = fileDir + "output_"+ date_s.c_str() + ".txt";
    outFile.open(fileName);

    float raw_value = 0;
    std::cout << "Filtering data. Sample amount: "<< channel_raw.size() << endl;
    for(int i=0; i<channel_raw.size(); i++){
      raw_value = channel_raw[i];
      temp[0] = m_f.filter(raw_value);
      temp[1] = m_f1.filter(temp[0]);
      temp[2] = m_f2.filter(temp[1]);
      temp[3] = sqrt(temp[2] * temp[2]);
      temp[4] = m_f3.filter(temp[3]);
      // save it
      outFile << raw_value<<","<< temp[0] << ", " << temp[1] << ", " << temp[2] << ", " << temp[3] << ", " << temp[4] << "\n";

    }

    outFile.close();
    std::cout<<"Output file: "<<fileName <<endl;

}
