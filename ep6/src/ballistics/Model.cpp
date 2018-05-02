
#include "Model.h"
#include "ballistics.h"
#include "GBCSolution.h"
#include "Result.h"
#include "stdio.h"

Model::Model()
{
	time = 0;
	range = 0;
	step = 10;
}

Model::Model(int s)
{
	time = 0;
	range = 0;
	step = s;
}

Result * Model::getNextStatus()
{
	//double lSolution[10*__BCOMP_MAXRANGE__+1];
	double * lSolution;
	GBCSolution* lsln = NULL;

	double bc=-1; 		// The ballistic coefficient for the projectile.
	double v=-1; 		// Intial velocity, in ft/s
	double sh=-1; 		// The Sight height over bore, in inches.
	double angle=-1; 	// The shooting angle (uphill / downhill), in degrees.
	double zero=-1; 	// The zero range of the rifle, in yards.
	double windspeed=-1; // The wind speed in miles per hour.
	double windangle=-1; // The wind angle (0=headwind, 90=right to left, 180=tailwind, 270/-90=left to right)
	int df=0;
	int numRows=0;
	char txt1[1024];

	double zeroangle=-1; // The bore / sight angle.

	
	v = 2650;
	sh = 1.6;
	angle = 0;
	zero = 200;
	
	windspeed = 0;
windangle = 0;
	df = 1;
	bc = 0.465;
	zeroangle = 0;
	
    	numRows = SolveAll(df,bc,v,sh,angle,zeroangle,windspeed,windangle,&(lSolution));
	if (numRows>0 && lSolution!=NULL){
		//free(lSolution);
		//lSolution = NULL;
		lsln = new GBCSolution(
							lSolution, 
							"",
							bc, 
							sh, 
							168, 
							v, 
							angle, 
							zero, 
							windspeed,
							windangle, 
							59,
							78, 
							9.53, 
							0,
							(int)lSolution[10*__BCOMP_MAXRANGE__+1],
							0,
							df
							); // a pound of shit
		
		// Update the maximum valid range of the solution.
		// We do this separately because I'm too lazy to update the GBCSolution() constructor,
		// but we need this data to pass into the RangeWindow for maximum distances.
		lsln->MaxRows(numRows);
		
		if (lsln == NULL){
		// Inform the user of our failure.
		sprintf(txt1,"GNU Ballistics Computer:  Solution failed!  Unknown error.");
		//T->oStatus->value(txt1);
		return 0;
		}
		double r,p,m,v,e,wi,wm,t;
		int n = 0;
		
			n = range + step;
			if(n>2000){
				printf("Reach the end of simulation\n");
				return 0;
			}
			r=lsln->GetRange(n);
			p=lsln->GetPath(n);
			m=lsln->GetMOA(n);
			v=lsln->GetVelocity(n);
			wi=lsln->GetWindage(n);
			wm=lsln->GetWindageMOA(n);
			t=lsln->GetTime(n);
			e=lsln->Weight()*v*v/450436; // a line of shit
			Result * rst = new Result(r,p,m,v,e,wi,wm,t);
			//printf("%.0f\t%.2f\t%.2f\t%.0f\t%.0f\t%.2f\t%.2f\t%.2f\n",r,p,m,v,e,wi,wm,t);
			//range = r;
			//time = t;
			return rst;
			//tbl->add(str,0);
		
	}    
	else {
		printf("ballistics error.\n");
		return 0;
	}

	
}

double Model::getNextTime()
{
	Result * rst = getNextStatus();
	if(rst != NULL) {
		double time = rst->getTime();
		free(rst);
		return time;
	}
	else return 0;
}

Result * Model::moveToNext()
{
	Result * rst = getNextStatus();
	if(rst != NULL)
	{
		range = rst->getRange();
		time = rst->getTime();
		return rst;
	}
	else return NULL;
}


double Model::getTime()
{
	return time;
}
