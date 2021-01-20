#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
//#include <cstdlib>
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
#include "SASTEST.h"


// -----------------------------------------------------------------------------
TCPServer SCREEN;

int main() {
	printHello();

	SCREEN.start();
	int i = 0;
	do {

	} while (i < 10);

	SCREEN.end();

	printHello();
	return 0;
}
