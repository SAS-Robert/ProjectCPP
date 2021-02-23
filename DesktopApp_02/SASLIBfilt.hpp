/** Main library for the SAS project
*/

#ifndef SASLIBfilt_H_ 
#define SASLIBfilt_H_

#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
// ------------------------------------------------------------------------

using namespace std;

// ------------------ Additional libraries ------------------

#include "Iir.h"

// ------------------ Gobal variables ------------------
// Filters from iir.h library. All the filters require the following parameters:
// - Nr of order
// - Sampling rate
// - Central frequency (Hz)
// - Frequency width (Hz)
const int samplingrate = 1000;
const int amplification = 1000;

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

// Processing and threshold pointers
double rec_threshold = 0;
unsigned long long int processed = 0;
unsigned long long int sample_nr = 0;

// Threshold processing
unsigned long long int th_time = 3;                    // 3 seconds
unsigned long long int th_nr = th_time * samplingrate; // amount of samples for threshold
double th_discard = samplingrate * 1.1;                // discard first filtered samples from the threshold
double th_discard_nr = 0;
int th_wait = 20, th_wait_cnt = 0; // amount of mean sets before triggering

// Accumulate old means and sizes for the flexible window
double old_value[5] = { 0, 0, 0, 0, 0 };
unsigned long long int old_nr[5] = { 0, 0, 0, 0, 0 };
unsigned int sample_lim = 27;

// Savind data in files will be eventually deleted
ofstream fileVALUES, fileFILTERS;
// ------------------ Functions definition ------------------
void filter_startup() {
    // Start filters
    Butty.setup(samplingrate, B_Fq, B_Fqw);
    B50.setup(samplingrate, B50_Fq, B50_100_Fqw);
    B100.setup(samplingrate, B100_Fq, B50_100_Fqw);
    B150.setup(samplingrate, B150_Fq, B50_100_Fqw);
    B200.setup(samplingrate, B200_Fq, B50_100_Fqw);
    B250.setup(samplingrate, B250_Fq, B50_100_Fqw);
    // Start internal processing variables
    Butty_result.clear();
    B50_result.clear();
    B100_result.clear();
    processed = 0;
}

static double process_data_iir(unsigned long long int v_size, vector<double> raw_data)
{
    double mean = 0, temp = 0, value = 0, raw_sample = 0.0;
    double flex_num = 0.0, flex_den = 0.0;
    unsigned long long int i = 0;
    unsigned long long int N_len = v_size - processed;
    // Filtering + calculate mean
    for (i = processed; i < v_size; ++i) // Loop for the length of the array
    {
        raw_sample = raw_data[i] * amplification; // Amplification
        // Filter data
        Butty_result.push_back(Butty.filter(raw_sample));
        B50_result.push_back(B50.filter(Butty_result[i]));
        B100_result.push_back(B100.filter(B50_result[i]));
        B150_result.push_back(B150.filter(B100_result[i]));
        B200_result.push_back(B200.filter(B150_result[i]));
        B250_result.push_back(B250.filter(B200_result[i]));
        // Savind data in files will be eventually deleted
        fileFILTERS << raw_sample << "," << Butty_result[i] << "," << B50_result[i] << "," << B100_result[i] << "," << B150_result[i] << "," << B200_result[i] << "," << B250_result[i] << "\n";

        // Calculating mean of retified EMG
        temp = B250_result[i];
        if (B100_result[i] < 0)
        {
            temp = -B250_result[i];
        }
        mean = mean + temp;
    }
    // Saving results
    mean = mean / N_len;

    // Complete value: taking into account the old mean(s)
    flex_num = ((old_value[0] * old_nr[0]) + (old_value[1] * old_nr[1]) + (mean * N_len));
    flex_den = (old_nr[0] + old_nr[1] + N_len);
    value = (flex_num) / (flex_den);

    // Savind data in files will be eventually deleted
    fileVALUES << mean << ", 0.0, " << processed << "," << v_size << "," << N_len << "\n";

    // Update processed data parameters
    processed = i;
    for (int i = 1; i < 5; i++)
    {
        old_value[i] = old_value[i - 1];
        old_nr[i] = old_nr[i - 1];
    }
    old_value[0] = mean;
    old_nr[0] = N_len;

    return value;
}

static double process_th(unsigned long long int v_size, vector<double> raw_data)
{
    double mean = 0, temp = 0, sd = 0, value = 0, raw_sample = 0.0;
    unsigned long long int i = 0;
    int th_limit = (int)th_discard;
    unsigned long long int N_len = v_size - processed;
    // Filtering + calculate mean
    for (i = processed; i < v_size; ++i) // Loop for the length of the array
    {
        raw_sample = raw_data[i] * amplification; // Amplification
        // Filter data
        Butty_result.push_back(Butty.filter(raw_sample));
        B50_result.push_back(B50.filter(Butty_result[i]));
        B100_result.push_back(B100.filter(B50_result[i]));
        B150_result.push_back(B150.filter(B100_result[i]));
        B200_result.push_back(B200.filter(B150_result[i]));
        B250_result.push_back(B250.filter(B200_result[i]));
        // Savind data in files will be eventually deleted
        fileFILTERS << raw_sample << "," << Butty_result[i] << "," << B50_result[i] << "," << B100_result[i] << "," << B150_result[i] << "," << B200_result[i] << "," << B250_result[i] << "\n";
        // Calculating mean of retified EMG
        temp = B250_result[i];
        if (B100_result[i] < 0)
        {
            temp = -B250_result[i];
        }
        mean = mean + temp;
    }
    mean = mean / N_len;
    if (v_size > th_discard)
    {
        // Calculate standard deviation
        temp = 0;
        for (i = processed; i < v_size; ++i)
        {
            temp = B250_result[i];
            if (B250_result[i] < 0)
            {
                temp = -B250_result[i];
            }
            sd += pow(temp - mean, 2);
        }
        sd = sqrt(sd / N_len);
        // Calculate final threshold value
        value = (mean + sd / 2) * N_len;
    }
    else
    {
        th_discard_nr = v_size;
    }

    // Savind data in files will be eventually deleted
    if (processed <= 10) {
        fileVALUES << mean << "," << sd << "," << th_discard << "," << v_size << "," << value << "\n";
    }
    else {
        fileVALUES << mean << "," << sd << "," << rec_threshold << "," << v_size << "," << value << "\n";
    }

    // Update amount of processed data
    processed = i;
    for (int i = 1; i < 5; i++)
    {
        old_value[i] = old_value[i - 1];
        old_nr[i] = old_nr[i - 1];
    }
    old_value[0] = mean;
    old_nr[0] = N_len;

    return value;
}

// ------------------ Objects definition ------------------

// ------------------------------------------------------------------------
#endif