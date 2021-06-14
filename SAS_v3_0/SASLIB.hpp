/** Main library for the SAS project
*/

#ifndef SASLIB_H_ // Include guards
#define SASLIB_H_

// SAS Project secondary libraries:

// UDP and TCP communication
#include "SASLIBcom.hpp"
// Basic functionalities
#include "SASLIBbasic.hpp"
// Stimulator and recorder
#include "SASLIBdev.hpp"
// Filtering and processing
#include "SASLIBfilt.hpp"
// ------------------------------------------------------------------------
#endif

/* Notas en la estructura de las librerias:
* - Se han hecho como un header .hpp y no un header + script (.h + .cpp)
* para que sean faciles de integrar en otras plataformas
* - La libreria com.hpp esta declarada la primera para evitar conflictos
*/