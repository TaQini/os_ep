#ifndef __RESULT_H__
 
#define __RESULT_H__


class Result {

public:
	Result(double r, double p, double m, double v, double e, double wi, double wm, double t)
	{
		this->r = r;
		this->p = p;
		this->m = m;
		this->v = v;
		this->e = e;
		this->wi = wi;
		this->wm = wm;
		this->t = t;
	}
	
	Result()
	{
		
	}
	
	double getTime(){return t;}
	double getRange(){return r;}


private:
double r,p,m,v,e,wi,wm,t;

};


#endif
