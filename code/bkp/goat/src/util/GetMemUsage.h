/*
 * GetMemUsage.h
 *
 *  Created on: Dec 14, 2020
 *      Author: kompalli
 */

#ifndef UNIQUEINT_SRC_UTIL_GETMEMUSAGE_H_
#define UNIQUEINT_SRC_UTIL_GETMEMUSAGE_H_

/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>

#endif

#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."
#endif


/**
 * Return the size of peak memory used by the process in Bytes.
 */
size_t getPeakRSS();
/**
 * Return the size of current memory used by the process in Bytes.
 */
size_t getCurrentRSS();


#endif /* UNIQUEINT_SRC_UTIL_GETMEMUSAGE_H_ */
