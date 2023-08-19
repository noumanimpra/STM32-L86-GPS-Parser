/*
 * usr_gps.c
 *
 *  Created on: Aug 2, 2023
 *      Author: numan
 */
#include "usr_general.h"
#define _buffer_size 1024
#define _max_message_size 90

extern bool GnssRx_Flag;
extern char rxData[1024];
extern UART_HandleTypeDef huart2;

/********************************************/

_io void DataParseProc(void);
_io void GpsGetRmc(void);
_io void formatLocation(float Lat, float Long);
_io void formatDataUTC(float Time);

_io char m_receiveGpsBuf[_buffer_size];
_io char m_gpsTransmitBuf[_buffer_size];

typedef bool _f; // flag type
typedef char _mBuf[100];
typedef char _lgBuf[150];
typedef char _sBuf[6];
typedef char _20_sizedBud[20];
typedef uint8_t _U8;
typedef float _fl;

_io _f rmcDataValidFlag = false;
_io _f northFlag = false;
_io _f eastFlag = false;

_io char *ptr;
_io _mBuf timeUtc;
_io _mBuf gpsPayyload;
_io _mBuf timeData;
_io _mBuf data2;
_io _mBuf dataTime;
_io _lgBuf gpsGSVPayyload;
_io _lgBuf gpsVTGPayyload;
_io _sBuf satInWiev;
_io _sBuf satId;
_io _sBuf elevation;
_io _sBuf azimuth;
_io _sBuf signalNoice;
_io _sBuf courseOG;
_io _sBuf speedOGKnot;
_io _sBuf speedOGKm;
_io _20_sizedBud longToCharLong;
_io _20_sizedBud latToCharLat;

int c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, c7 = 0, c8 = 0; // temproary counters(needed :p )
_io int Msgindex;

_io _U8 dotIndexLong;
_io _U8 dotIndexLat;
_io _U8 degreeIndexLat;
_io _U8 degreeIndexLong;
_io _U8 comacounter;

_io _fl time, latitude, longitude;
_io _fl arrLat, arrLong;

/******************************************/
/*****************user*******************/
void UL_GpsL86GetLocation(void) //? testi geçti
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        // formatData();
        formatLocation(latitude, longitude);
    }
}
void getUTC(void) // *! to be tested
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        formatDataUTC(time);
    }
}
void getLocalTr(void) // *! to be tested
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        formatDataLocalTr(time);
    }
}

/********************IO*******************/
_io void GpsGetRmc(void) //? testi geçti
{
    if (GnssRx_Flag)
    {
        Msgindex = 0;
        strcpy(m_gpsTransmitBuf, (char *)(rxData));
        ptr = strstr(m_gpsTransmitBuf, "GNRMC");

        if (*ptr == 'G')
        {
            while (1)
            {
                gpsPayyload[Msgindex] = *ptr;
                Msgindex++;
                *ptr = *(ptr + Msgindex);

                if (*ptr == '\n' || Msgindex > _max_message_size)
                {
                    Msgindex = 0;
                    for (int i = 0; i < _buffer_size; i++)
                    {
                        rxData[i] = 0;
                        m_gpsTransmitBuf[i] = 0;
                    }
                    for (int i = 0; i < 100; i++)
                    {
                        if (gpsPayyload[i] == 'N')
                            northFlag = true;
                        if (gpsPayyload[i] == 'E')
                            eastFlag = true;
                    }
                    if (strlen(gpsPayyload) > 10) // min rmc data
                    {
                        if (northFlag && eastFlag) // is data vaid
                        {
                            northFlag = false;
                            eastFlag = false;

                            sscanf(gpsPayyload, "GNRMC,%f,A,%f,N,%f,", &time, &latitude, &longitude);
                            rmcDataValidFlag = true;
                        }
                        else
                        {
                            HAL_UART_Transmit(&huart2, (uint8_t *)"Con err", 7, HAL_MAX_DELAY);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 100; i++)
                            gpsPayyload[i] = 0;
                    }
                    break;
                }
            }
        }
        GnssRx_Flag = false;
    }
}

_io void formatDataUTC(float Time) // *! to be tested
{
    Hours = (int)Time / 10000;
    Min = (int)(Time - (Hours * 10000)) / 100;
    Sec = (int)(Time - ((Hours * 10000) + (Min * 100)));
    sprintf(dataTime, "\r\nTime UTC: %d:%d:%d", Hours, Min, Sec);
    HAL_UART_Transmit(&huart2, (uint8_t *)dataTime, strlen(dataTime), HAL_MAX_DELAY);
}

_io void formatDataLocalTr(float Time) // *! to be tested
{
    Hours = (int)Time / 10000;
    Min = (int)(Time - (Hours * 10000)) / 100;
    Sec = (int)(Time - ((Hours * 10000) + (Min * 100)));
    sprintf(dataTime, "\r\nTime UTC: %d:%d:%d", Hours + 3, Min, Sec);
    HAL_UART_Transmit(&huart2, (uint8_t *)dataTime, strlen(dataTime), HAL_MAX_DELAY);
}

// _io void DataParseProc(void)
// {
// }

// _io void formatData(void)
// {
// }

_io void formatLocation(float Lat, float Long)
{
    arrLat = latitude;
    arrLong = longitude;
    sprintf(longToCharLong, "%f", arrLong);
    for (int i = 0; i < 20; i++)
        if (longToCharLong[i] == '.')
            dotIndexLong = i;

    degreeIndexLong = dotIndexLong - 2;

    for (int i = 19; i >= degreeIndexLong; i--)
        longToCharLong[i + 1] = longToCharLong[i];

    longToCharLong[degreeIndexLong] = ' ';
    // EO long

    sprintf(latToCharLat, "%f", arrLat);
    for (int i = 0; i < 20; i++)
        if (latToCharLat[i] == '.')
            dotIndexLat = i;

    degreeIndexLat = dotIndexLat - 2;
    for (int i = 19; i >= degreeIndexLat; i--)
        latToCharLat[i + 1] = latToCharLat[i];

    latToCharLat[degreeIndexLat] = ' ';
    // strcpy(testArr, (char *)(latToCharLat));
    sprintf(data2, "\r\n Lat: %s N, Long: %s E", latToCharLat, longToCharLong);

    /*
     * print location
     */
    HAL_UART_Transmit(&huart2, (uint8_t *)data2, strlen(data2), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
    for (int i = 0; i < 100; i++)
    {
        data2[i] = 0;
    }
}
