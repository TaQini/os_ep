#include "GBCSolution.h"

GBCSolution::GBCSolution(void){

}

GBCSolution::GBCSolution(double* isolution, const char* iname, double ibc, double isightheight, int iweight, int imv, int iangle, int izerorange, int iwindspeed, int iwindangle, int itemp, int ihumidity, double ipressure, int ialtitude, int entries, int useweather, int idf){

	sln=isolution;
	strcpy(name,iname);
	bc=ibc;
	sightheight=isightheight;
	weight=iweight;
	mv=imv;
	angle=iangle;
	zerorange=izerorange;
	windspeed=iwindspeed;
	windangle=iwindangle;
	temp=itemp;
	humidity=ihumidity;
	pressure=ipressure;
	altitude=ialtitude;
	rows=entries;
	ckweather=useweather;
	df = idf;
	
}



GBCSolution::~GBCSolution(){

	if (this->sln != NULL) {
		free(this->sln);
	}	

}
		
		
const char* GBCSolution::Name(void){
	return name;
}
	
double GBCSolution::BC(void){
	return bc;
}

double GBCSolution::SightHeight(void){
	return sightheight;
}

int GBCSolution::MuzzleVelocity(void){
	return mv;
}

int GBCSolution::ShootingAngle(void){
	return angle;
}
int GBCSolution::ZeroRange(void){
	return zerorange;
}

int GBCSolution::WindSpeed(void){
	return windspeed;
}

int GBCSolution::WindAngle(void){
	return windangle;
}

int GBCSolution::Temp(void){
	return temp;
}

double GBCSolution::Pressure(void){
	return pressure;
}

int GBCSolution::Humidity(void){
	return humidity;
}

int GBCSolution::Altitude(void){
	return altitude;
}



double GBCSolution::GetRange(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage];
	}
	else return 0;
}

double GBCSolution::GetPath(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+1];
	}
	else return 0;
}

double GBCSolution::GetMOA(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+2];
	}
	else return 0;
}


double GBCSolution::GetTime(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+3];
	}
	else return 0;
}

double GBCSolution::GetWindage(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+4];
	}
	else return 0;
}

double GBCSolution::GetWindageMOA(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+5];
	}
	else return 0;
}

double GBCSolution::GetVelocity(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+6];
	}
	else return 0;
}

double GBCSolution::GetVx(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+7];
	}
	else return 0;
}

double GBCSolution::GetVy(int yardage){
	double size=sln[__BCOMP_MAXRANGE__*10+1];
	if (yardage<size){
		return sln[10*yardage+8];
	}
	else return 0;
}

double GBCSolution::GetEnergy(int k){
	return (double)weight*GetVelocity(k)*GetVelocity(k)/450436;
}


int GBCSolution::MaxRows(void){
	return rows;
}

int GBCSolution::MaxRows(int mr){
	rows=mr;
	return rows;
}

int GBCSolution::Weight(void){
	return weight;
}

int GBCSolution::UseWeather(void){
	return ckweather;
}

