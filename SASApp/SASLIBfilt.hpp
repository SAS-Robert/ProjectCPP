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
const int SAMPLINGRATE = 1000;
const int AMPLIFICATION = 1000;

// Define Bandpass Filter
const int ORDER_PASS = 4;
double LOWHZ = 20;
double HIGHHz = 40;
const double BFQ = (HIGHHz + LOWHZ) / 2;
const double BFW = (HIGHHz - LOWHZ);

Iir::Butterworth::BandPass<ORDER_PASS> BPass;
std::vector<double> bPass_result;

// Define Notch filter
const double B50FQ = 50;
const double BSTOP_FW = 4;
const int ORDER_STOP = 2;

Iir::Butterworth::BandStop<ORDER_STOP> B50;
std::vector<double> b50_result;

// Processing and threshold pointers
double THRESHOLD = 0;
double MEAN = 0;
double MVC = 0;
unsigned long long int GL_processed = 0;
unsigned long long int GL_sampleNr = 0;

// Threshold processing
const unsigned int TH_TIME = 3;                    // 3 seconds to set threshold
const unsigned long long int TH_NR = TH_TIME * SAMPLINGRATE; // amount of samples for threshold
const double TH_DISCARD = SAMPLINGRATE * 1.1;                // discard first filtered samples from the threshold
double GL_thDiscard = 0;
int TH_WAIT = 20, GL_thWaitCnt = 0; // amount of mean sets before triggering

// Accumulate old means and sizes for the flexible window
const unsigned int FLEX_WINDOW = 5;
double old_value[FLEX_WINDOW] = { 0, 0, 0, 0, 0 };
unsigned long long int old_nr[FLEX_WINDOW] = { 0, 0, 0, 0, 0 };
const unsigned int SAMPLE_LIM = 27;

// Savind data in files will be eventually deleted
ofstream fileVALUES, fileFILTERS;
// ------------------ Functions definition ------------------
void startup_filters() {
    // Start filters
    BPass.setup(SAMPLINGRATE, BFQ, BFW);
    B50.setup(SAMPLINGRATE, B50FQ, BSTOP_FW);

    // Start internal processing variables
    bPass_result.clear();
    b50_result.clear();

    GL_processed = 0;
    THRESHOLD = 0.0;
    for (int i = 0; i < FLEX_WINDOW; i++)
    {
        old_value[i] = 0;
        old_nr[i] = 0;
    }
}
// EMG activity
static double process_data_iir(unsigned long long int v_size, vector<double> raw_data)
{
    double mean = 0, temp = 0, value = 0, raw_sample = 0.0;
    double flex_num = 0.0, flex_den = 0.0;
    unsigned long long int i = 0;
    unsigned long long int N_len = v_size - GL_processed;
    // Filtering + calculate mean
    for (i = GL_processed; i < v_size; ++i) // Loop for the length of the array
    {
        raw_sample = raw_data[i] * AMPLIFICATION; // AMPLIFICATION
        // Filter data
        bPass_result.push_back(BPass.filter(raw_sample));
        b50_result.push_back(B50.filter(bPass_result[i]));

        // Savind data in files will be eventually deleted
        fileFILTERS << raw_sample << "," << bPass_result[i] << "," << b50_result[i] << "\n";

        // Calculating mean of retified EMG
        temp = b50_result[i];
        if (b50_result[i] < 0)
        {
            temp = -b50_result[i];
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
    fileVALUES << mean << ", 0.0, " << GL_processed << "," << v_size << "," << N_len << "\n";

    // Update GL_processed data parameters
    GL_processed = i;
    for (int i = 4; i >= 1; i--)
    {
        old_value[i] = old_value[i - 1];
        old_nr[i] = old_nr[i - 1];
    }
    old_value[0] = mean;
    old_nr[0] = N_len;

    return value;
}

// Threshold methods
static double process_th_mean(unsigned long long int v_size, vector<double> raw_data)
{
    double mean = 0, temp = 0, sd = 0, value = 0, raw_sample = 0.0;
    unsigned long long int i = 0;
    unsigned long long int N_len = v_size - GL_processed;

    // Filtering + calculate mean
    for (i = GL_processed; i < v_size; ++i) // Loop for the length of the array
    {
        raw_sample = raw_data[i] * AMPLIFICATION;
        // Filter data
        bPass_result.push_back(BPass.filter(raw_sample));
        b50_result.push_back(B50.filter(bPass_result[i]));

        // Savind data in files will be eventually deleted
        fileFILTERS << raw_sample << "," << bPass_result[i] << "," << b50_result[i] << "\n";
        // Calculating mean of retified EMG
        temp = b50_result[i];
        if (b50_result[i] < 0)
        {
            temp = -b50_result[i];
        }
        mean = mean + temp;
    }
    mean = mean / N_len;
    if (v_size > TH_DISCARD)
    {
        //// Calculate standard deviation
        //temp = 0;
        //for (i = GL_processed; i < v_size; ++i)
        //{
        //    temp = b50_result[i];
        //    if (b50_result[i] < 0)
        //    {
        //        temp = -b50_result[i];
        //    }
        //    sd += pow(temp - mean, 2);
        //}
        //sd = sqrt(sd / N_len);
        //// Calculate final threshold value
        //value = (mean + sd / 2) * N_len;
    }
    else
    {
        GL_thDiscard = v_size;
    }

    // Savind data in files will be eventually deleted
    if (GL_processed <= 10) {
        fileVALUES << mean << "," << sd << "," << TH_DISCARD << "," << v_size << "," << value << "\n";
    }
    else {
        fileVALUES << mean << "," << sd << "," << THRESHOLD << "," << v_size << "," << value << "\n";
    }

    // Update amount of GL_processed data
    GL_processed = i;
    for (int i = 4; i >= 1; i--)
    {
        old_value[i] = old_value[i - 1];
        old_nr[i] = old_nr[i - 1];
    }
    old_value[0] = mean;
    old_nr[0] = N_len;

    return mean;
}

static double process_th_sd(unsigned long long int v_size, int factor)
{
	double temp = 0, sd = 0, value = 0;
	unsigned long long int i = 0;
	unsigned long long int N_len = v_size - GL_thDiscard;
	// Filtering + calculate mean
	for (i = GL_thDiscard; i < v_size; ++i) // Loop for the length of the array
	{
		temp = b50_result[i];
		if (b50_result[i] < 0)
		{
			temp = -b50_result[i];
		}
		sd += pow(temp - MEAN, 2);
	}
	sd = sqrt(sd / N_len);
	// Calculate final threshold value
	value = MEAN + sd * factor;

	return value;
}

static double process_th_mvc(unsigned long long int v_size, vector<double> raw_data)
{
    double temp = 0, raw_sample = 0.0;
    unsigned long long int i = 0;
    unsigned long long int N_len = v_size - GL_processed;
    // Filtering + calculate mean
    for (i = GL_processed; i < v_size; ++i) // Loop for the length of the array
    {
        raw_sample = raw_data[i] * AMPLIFICATION;
        // Filter data
        bPass_result.push_back(BPass.filter(raw_sample));
        b50_result.push_back(B50.filter(bPass_result[i]));

        // Savind data in files will be eventually deleted
        fileFILTERS << raw_sample << "," << bPass_result[i] << "," << b50_result[i] << "\n";
        // Calculating mean of retified EMG
        temp = b50_result[i];
        if (b50_result[i] < 0)
        {
            temp = -b50_result[i];
        }
        if (temp > MVC)
        {
            MVC = temp;
        }
    }
    if (v_size > TH_DISCARD)
    {
        //// Calculate standard deviation
        //temp = 0;
        //for (i = GL_processed; i < v_size; ++i)
        //{
        //    temp = b50_result[i];
        //    if (b50_result[i] < 0)
        //    {
        //        temp = -b50_result[i];
        //    }
        //    sd += pow(temp - mean, 2);
        //}
        //sd = sqrt(sd / N_len);
        //// Calculate final threshold value
        //value = (mean + sd / 2) * N_len;
    }
    else
    {
        GL_thDiscard = v_size;
    }

    // Savind data in files will be eventually deleted
    if (GL_processed <= 10) {
        fileVALUES << MVC << "," << 0 << "," << TH_DISCARD << "," << v_size << "," << MVC << "\n";
    }
    else {
        fileVALUES << MVC << "," << 0 << "," << THRESHOLD << "," << v_size << "," << MVC << "\n";
    }

    // Update amount of GL_processed data
    //GL_processed = i;
    //for (int i = 4; i >= 1; i--)
    //{
    //    old_value[i] = old_value[i - 1];
    //    old_nr[i] = old_nr[i - 1];
    //}
    //old_value[0] = mean;
    //old_nr[0] = N_len;

    return MVC;
}
// ------------------ Objects definition ------------------

// ------------------------------------------------------------------------
#endif