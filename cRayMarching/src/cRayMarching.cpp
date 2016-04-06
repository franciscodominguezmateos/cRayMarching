//============================================================================
// Name        : cRayMarching.cpp
// Author      : Francisco Dominguez
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "vector3d.h"

using namespace std;
using namespace cv;

typedef Vector3D vec3;

float sdPlane( vec3 p, vec3 n, float d )
{
  // n must be normalized
  return p*n + d;
}
float sdSphere( vec3 p, float s )
{
  return p.length()-s;
}
float udBox( vec3 p, vec3 b )
{
	vec3 d=abs(p)-b;
	return max(d,0.0).length();
}
float udRoundBox( vec3 p, vec3 b, float r )
{
  vec3 d=abs(p)-b;
  return max(d,0.0).length()-r;
}
//Union
float opU( float d1, float d2 )
{
    return min(d1,d2);
}
//Substraction
float opS( float d1, float d2 )
{
    return max(-d1,d2);
}
//Intersection
float opI( float d1, float d2 )
{
    return max(d1,d2);
}
/*
//Repetition
float opRep( vec3 p, vec3 c )
{
    vec3 q = mod(p,c)-0.5*c;
    return primitve( q );
}
//Rotation/Translation
float opTx( vec3 p, mat4 m )
{
    vec3 q = invert(m)*p;
    return primitive(q);
}
//Scale
float opScale( vec3 p, float s )
{
    return primitive(p/s)*s;
}
*/
float f(vec3 p){
	vec3 p1=-p+vec3(10,0,0);
	return opU(sdSphere(p1,2),sdSphere(p,3));
	//return sdSphere(p,4);
}
vec3 df(vec3 p){
	float eps=0.0001;
	vec3 dx(eps,0,0);
	vec3 dy(0,eps,0);
	vec3 dz(0,0,eps);
	float fp=f(p);
	float dfx=(f(p+dx)-fp)/eps;
	float dfy=(f(p+dy)-fp)/eps;
	float dfz=(f(p+dz)-fp)/eps;
	vec3 v(dfx,dfy,dfz);
	return v;
}
struct Ray {
	vec3 origin;
	vec3 direction;
};
Ray generateRayForPixel(int i,int j){
	const float xres=320/2;
	const float yres=240/2;
	float x=-xres+i;
	float y=-yres+240-j;
	vec3 p(x/40,y/40,-5);
	vec3 o(0,0,-6);
	vec3 d;
	d=p-o;
	Ray r;
	r.origin=o;
	r.direction=d;
	return r;
}
float distance2Scene(vec3 p){
	return f(p);
}
vec3 getColor(vec3 p){
	return vec3(1,0,0);
}
void renderImage( vec3 *image )
{
	const int xres=320;
	const int yres=240;
	const int maxsteps =100;;
	const float maxdist=100;
	vec3 c;
    for( int j=0; j < yres; j++ )
    for( int i=0; i < xres; i++ ){
        Ray ray = generateRayForPixel( i, j );
        //cout << ray.origin << "-" << ray.direction << endl;
        float t=0;
        float d;
        vec3 p;
        for(int s=0;s<maxsteps;s++){
        	p=ray.origin+ray.direction*t;
        	d=distance2Scene(p);
        	if(fabs(d)<0.01 || t>maxdist)
        		break;
        	t+=d;
        }
        if (t<maxdist){
        	vec3 cs=getColor(p);
        	vec3 n=df(p);
        	n.normalize();
        	vec3 vlight(1,0,-1);
        	vlight.normalize();
        	float l=vlight*n;
        	c=cs*l;
        }
        else{//background
        	c=vec3(0,0,1);
        }
        image[i+j*xres]=c;
    }
}
int main(int argc, char** argv) {
	const int xres=320;
	const int yres=240;
	//char *imagepath=argv[1];
    //Mat image = imread(imagepath, IMREAD_COLOR );
    Mat image(Size(320,240),CV_8UC3);
    vec3 *imgvec3=new vec3[320*240];
    renderImage(imgvec3);
    for( int j=0; j < yres; j++ )
    for( int i=0; i < xres; i++ ){
    	Vec3b intensity=image.at<Vec3b>(j,i);
    	int r=imgvec3[i+j*xres].x*255;
    	int g=imgvec3[i+j*xres].y*255;
    	int b=imgvec3[i+j*xres].z*255;
    	intensity.val[0]=b;
    	intensity.val[0]=g;
    	intensity.val[0]=r;
    	image.at<Vec3b>(j,i)[0]=b;
    	image.at<Vec3b>(j,i)[1]=g;
    	image.at<Vec3b>(j,i)[2]=r;
    }
    imshow("Display Image", image);
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	waitKey(0);
	return 0;
}
