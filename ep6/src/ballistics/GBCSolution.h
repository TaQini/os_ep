#ifndef GBCSOLUTION_H
#define GBCSOLUTION_H

#include "ballistics.h"
#include <stdio.h>
#include <string.h>

class GBCSolution {

   public:
		GBCSolution(void);
		GBCSolution(
				double* isolution, 
				const char* name, 
				double ibc, 
				double isightheight, 
				int iweight, 
				int	imv, 
				int iangle, 
				int izerorange, 
				int iwindspeed, 
				int iwindangle, 
				int itemp, 
				int ihumidity, 
				double ipressure, 
				int ialtitude,
				int rows,
				int ckweather,
				int df
			);
		
		~GBCSolution();

		double* sln;
		const char* Name(void);
		double BC(void);
		double SightHeight(void);
		int MuzzleVelocity(void);
		int ShootingAngle(void);
		int ZeroRange(void);
		int WindSpeed(void);
		int WindAngle(void);
		int Temp(void);
		double Pressure(void);
		int Humidity(void);
		int Altitude(void);
		int Weight(void);
		int MaxRows(void);
		int MaxRows(int);
		int UseWeather(void);
		
		double GetRange(int yardage);
		double GetPath(int yardage);
		double GetMOA(int yardage);
		double GetTime(int yardage);
		double GetWindage(int yardage);
		double GetWindageMOA(int yardage);
		double GetVelocity(int yardage);
		double GetVx(int yardage);
		double GetVy(int yardage);
		double GetEnergy(int yardage);
		
		int df;

   private:
		double* solution;
		char name[255];
		int weight;
		double bc;
		double sightheight;
		int mv;
		int angle;
		int zerorange; 
		int windspeed;
		int windangle;
		int temp;
		int ckweather;

		double pressure;
		int humidity;
		int altitude;
		int rows;
		
};

#endif
