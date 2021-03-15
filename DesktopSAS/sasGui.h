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
	bool hmi_repeat, hmi_new;
	// Exercise parameters
	bool main_thEN, recReq, stimActive;
	exercise_Type exercise;
	threshold_Type method, next_method;
	// Stimulation parameters
	float current, frequency;
	int ramp;

	// ------------ GUI -> SAS
	RehaMove3_Req_Type Move3_hmi;
	User_Req_Type User_hmi;
	int ch_hmi;

	// for isMoving testing
	double isVelocity_limit;
	// Constructor
	mainUI() {
		END_GUI = false;
		// Status
		status = st_init;
		screenMessage = " ";
		hmi_new = false;
		hmi_repeat = false;
		// Exercise
		main_thEN = false;
		recReq = false;
		stimActive = false;
		exercise = exCircuit;
		method = th_SD05;
		// Stimulation parameters
		current = 0.0;
		frequency = 30.0;
		ramp = 3;

		// gui -> sas
		Move3_hmi = Move3_none;
		User_hmi = User_none;
		int ch_hmi = 1;


		isVelocity_limit = 0.8;
	}
};

mainUI GL_UI;

// Other variables that for some reason could not be created in the GUI declaration
string statusList[10];
string methodList[3];