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

// Define Bandpass filter
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
unsigned long long int GL_processed = 0;
unsigned long long int GL_sampleNr = 0;

// Threshold processing
const unsigned int TH_TIME = 3;                    // 3 seconds to set threshold
const unsigned long long int TH_NR = TH_TIME * SAMPLINGRATE; // amount of samples for threshold
const double TH_DISCARD = SAMPLINGRATE * 1.1;                // discard first filtered samples from the threshold
double GL_thDiscard = 0;
int TH_WAIT = 20, GL_thWaitCnt = 0; // amount of mean sets before triggering
std::vector<double> calibration_rest_data;
std::vector<double> calibration_mvc_data;

threshold_Type GL_thMethod;

// Accumulate old means and sizes for the flexible window
const unsigned int FLEX_WINDOW = 3;
double old_value[FLEX_WINDOW] = {0, 0, 0};
unsigned long long int old_nr[FLEX_WINDOW] = {0, 0, 0};
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
	double mean = 0, value = 0, raw_sample = 0.0;
	double flex_num = 0.0, flex_den = 0.0;

	// Filtering + calculate mean
	mean = calculate_mean(v_size, raw_data);

	// Complete value: taking into account the old mean(s)
	flex_num = ((old_value[0] * old_nr[0]) + (old_value[1] * old_nr[1]) + (mean * N_len));
	flex_den = (old_nr[0] + old_nr[1] + N_len);
	value = (flex_num) / (flex_den);

	// Savind data in files will be eventually deleted
	fileVALUES << mean << ", 0.0, " << GL_processed << "," << v_size << "," << N_len << "\n";

	// Update GL_processed data parameters
	GL_processed = v_size;

	for (int i = 2; i >= 1; i--)
	{
		old_value[i] = old_value[i - 1];
		old_nr[i] = old_nr[i - 1];
	}
	old_value[0] = mean;
	old_nr[0] = N_len;

	return value;
}

// Threshold methods: SD
static double process_th_sd(unsigned long long int v_size, vector<double> raw_data, int factor)
{
	double sd = 0, value = 0, raw_sample = 0.0;
	unsigned long long int i = 0, N_len = v_size - GL_processed;
	int th_limit = (int)TH_DISCARD;

	// Filtering + calculate mean
	double mean = calculate_mean(v_size, raw_data, N_len);

	if (v_size > TH_DISCARD)
	{
		// Calculate standard deviation
		sd = calculate_std(v_size, raw_data, mean);

		// Calculate final threshold value
		value = (mean + sd * factor) * N_len;
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
	GL_processed = v_size;
	for (int i = 2; i >= 1; i--)
	{
		old_value[i] = old_value[i - 1];
		old_nr[i] = old_nr[i - 1];
	}
	old_value[0] = mean;
	old_nr[0] = N_len;

	return value;
}

// Threshold methods: MVC
static double process_th_mvc(unsigned long long int v_size, vector<double> raw_data, int factor)
{
	double mvc = 0, value = 0, raw_sample = 0.0;
	unsigned long long int i = 0, N_len = v_size - GL_processed;
	int th_limit = (int)TH_DISCARD;

	// Filtering + calculate mean
	double mean = calculate_mean(v_size, calibration_rest_data, N_len);

	if (v_size > TH_DISCARD)
	{
		// Calculate standard deviation
		mvc = calculate_MVC(v_size, raw_data);

		// Calculate final threshold value
		value = (mean + mvc * factor);
	}
	else
	{
		GL_thDiscard = v_size;
	}

	// Savind data in files will be eventually deleted
	if (GL_processed <= 10) {
		fileVALUES << mean << "," << mvc << "," << TH_DISCARD << "," << v_size << "," << value << "\n";
	}
	else {
		fileVALUES << mean << "," << mvc << "," << THRESHOLD << "," << v_size << "," << value << "\n";
	}

	// Update amount of GL_processed data
	GL_processed = v_size;
	for (int i = 2; i >= 1; i--)
	{
		old_value[i] = old_value[i - 1];
		old_nr[i] = old_nr[i - 1];
	}
	old_value[0] = mean;
	old_nr[0] = N_len;

	return value;
}

// Function definition: Calculate Mean
static double calculate_mean(unsigned long long int v_size, vector<double> raw_data, unsigned long long int N_len)
{
	double raw_sample, temp, mean;
	int i;

	for (i = GL_processed; i < v_size; ++i) // Loop for the length of the array
	{
		temp = preprocess_data(raw_data[i],i);
		mean += temp;

		// hold on to calibration data
		if (v_size >= TH_DISCARD && calibration_rest_data.size() < TH_NR && GL_thMethod <= 2)
		{
			calibration_rest_data.push_back(temp);
		}
		else if (v_size >= TH_DISCARD && calibration_mvc_data.size() < TH_NR && GL_thMethod > 2) 
		{
			calibration_mvc_data.push_back(temp);
		}

	}
	mean = mean / N_len;

	return mean;
}

// Function definition: Calculate STD
static double calculate_std(unsigned long long int v_size, vector<double> raw_data, double mean)
{
	double raw_sample, temp, sd;
	int i;
	unsigned long long int N_len = v_size - GL_processed;

	for (i = GL_processed; i < v_size; ++i) // Loop for the length of the array
	{
		temp = preprocess_data(raw_data[i]);
		sd += pow(temp - mean, 2);
	}

	sd = sqrt(sd / N_len);

	return sd;
}

// Function definition: Calculate STD
static double calculate_MVC(unsigned long long int v_size, vector<double> raw_data)
{
	double raw_sample, temp, tempMax;
	int i;

	for (i = GL_processed; i < v_size; ++i) // Loop for the length of the array
	{
		temp = preprocess_data(raw_data[i]);
		if (temp > tempMax)
		{
			tempMax = temp;
		}
	}

	return tempMax;
}

// Function definition: Filter and rectify data
static double preprocess_data(double raw_data, int i)
{
	double raw_sample, preprocessed_sample;

	raw_sample = raw_data * AMPLIFICATION;
	// Filter data
	bPass_result.push_back(BPass.filter(raw_sample));
	b50_result.push_back(B50.filter(bPass_result[i]));

	// Savind data in files will be eventually deleted
	fileFILTERS << raw_sample << "," << bPass_result[i] << "," << b50_result[i] << "\n";
	// Calculating mean of retified EMG
	preprocessed_sample = b50_result[i];
	if (b50_result[i] < 0)
	{
		preprocessed_sample = -b50_result[i];
	}

	return preprocessed_sample;
}



// ------------------ Objects definition ------------------

// ------------------------------------------------------------------------
#endif