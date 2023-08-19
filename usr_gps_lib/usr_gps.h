/*
 * usr_gps.h
 *
 *  Created on: Aug 2, 2023
 *      Author:  numan
 *      Caution:
 */

#ifndef USR_GPS_H_
#define USR_GPS_H_
#include "usr_general.h"
#define _io static

/***************************************/

/**
 * get all NMEA sentences
 * transmits via uart
 */
// void UL_GpsL86GetValues(void);
/*
 * get formatted location value
 */
void UL_GpsL86GetLocation(void);

#endif /* USR_GPS_H_ */
