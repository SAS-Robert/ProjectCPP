#pragma once
#include <random>
#include <string>
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8
#include <cstdint>        // std::uint_least32_t
#include "SASLIBbasic.hpp"
// Only use the std variable types necessary,
// if these usings are changed to using namespace std;
// the project wont compile
using std::string;
using std::wstring;

// Convert a string into wstring format
std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

// Convert a wstring into string format
std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}


// This class is a set of fields to communicate the main with the local IU
public class mainUI {
public:
	// examples 
	int cnt;
	std::wstring sampleText = L"HAHAHAHA";
	bool END_GUI;

	// ------------ SAS -> GUI
	// Status field
	state_Type status;
	string screenMessage;
	bool hmi_repeat, hmi_new, playPause, set_MVC, th1, th2;
	// Exercise parameters
	bool main_thEN, recReq, stimActive, recReady, stimReady;
	threshold_Type method, next_method;
	// Stimulation parameters
	float current, frequency;
	int ramp;

	// For new automatic calibration
	exercise_Type exercise, next_exercise;

	// ------------ GUI -> SAS
	RehaMove3_Req_Type Move3_hmi;
	User_Req_Type User_hmi;
	int ch_hmi;

	// Global additional settings:
	char PORT_STIM[5] = "COM7";
	char PORT_REC[5] = "COM4";

	// for isMoving testing
	double isVelocity;
	double isVelocity_limit;
	// Constructor
	mainUI() {
		END_GUI = false;
		// Status
		status = st_init;
		screenMessage = " ";
		hmi_new = false;
		hmi_repeat = false;
		set_MVC = false;
		// Exercise
		main_thEN = false;
		recReq = false;
		stimActive = false;
		exercise = kneeExt;
		method = th_SD05;
		// Stimulation parameters
		current = 0.0;
		frequency = 30.0;
		ramp = 3;

		recReady = false;
		stimReady = false;
		// gui -> sas
		Move3_hmi = Move3_none;
		User_hmi = User_none;
		int ch_hmi = 1;

		// For isMoving testing
		isVelocity = 0.0;
		isVelocity_limit = 100.0;
	}
};

mainUI GL_UI;

// Other variables that for some reason could not be created in the GUI declaration
string statusList[15];
string methodList[6];
string exerciseList[30];