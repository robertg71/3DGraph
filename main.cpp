/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/
#include <mavsprintf.h>

#include <MAUtil/GLMoblet.h>
#include <GLES2/gl2.h>

//#define GLM_FORCE_INLINE
//#include <limits>
//#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <maheap.h>
#include <conprint.h>
#include <mavsprintf.h>
#include "graph.h"

using namespace MAUtil;

#define STRINGIFY(A)  #A

/**
 * A OpenGL|ES 2.0 GLMoblet application that
 * draws a quad covering the whole screen with
 * a fragment shader that generates a trippy
 * pattern. Shows how to load and create
 * shaders, do more advanced error handling,
 * draw primitives using OpenGL|ES 2.0.
 * A pretty complex shader is run per pixel on the screen
 * and might be slow on some devices with older gpu:s.
 */

/*
 *
 *
		char fragmentShaderSource[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"void main(void)\n"
		"{\n"
		"float x = gl_FragCoord.x*(resolution.x);\n"
		"float y = gl_FragCoord.y*(resolution.y);\n"
		"float c1 = sin((cos(x*6.0+sin(time+y)*7.0))*3.14159+cos(time))*0.5+0.5;\n"
		"gl_FragColor = vec4( c1*0.2,c1*0.5,c1*0.9,1.0);\n"
		"}\n";

 */
/* maintains symetry zoom fx*/
		char fragmentShaderSource_plasma_colorbars1[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"void main(void)\n"
		"{\n"
		"float x  = gl_FragCoord.x*(resolution.x);\n"
		"float y  = gl_FragCoord.y*(resolution.y);\n"
		"float s1 = 20.0*sin(time)+30.0;"
/*		"float s2 = 20.0*cos(time)+30.0;" */
		"float st = sin(y+time*2.0)*0.5+0.5;"
		"float ct = cos(x+time*2.0)*0.5+0.5;"
		"float c1 = sin(s1*cos(x+time) + s1*sin(y+time))*0.5+0.5;\n"
		"gl_FragColor = vec4( c1*0.9*st,c1*0.3*st,c1*0.9*ct,1.0);\n"
		"}\n";

/* zoom cos/sin */
		char fragmentShaderSource_plasma_colorbars2[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"void main(void)\n"
		"{\n"
		"float x  = gl_FragCoord.x*(resolution.x);\n"
		"float y  = gl_FragCoord.y*(resolution.y);\n"
		"float s1 = 20.0*sin(time)+30.0;"
		"float s2 = 20.0*cos(time)+30.0;"
		"float st = sin(y+time*2.0)*0.5+0.5;"
		"float ct = cos(x+time*2.0)*0.5+0.5;"
		"float c1 = sin(s1*cos(x+time) + s2*sin(y+time))*0.5+0.5;\n"
		"gl_FragColor = vec4( c1*0.9*st,c1*0.3*st,c1*0.9*ct,1.0);\n"
		"}\n";

/* very good one, phase shifts unsymetrical shift */
		char fragmentShaderSource_plasma_colorbars3[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"void main(void)\n"
		"{\n"
		"float x  = gl_FragCoord.x*(resolution.x);\n"
		"float y  = gl_FragCoord.y*(resolution.y);\n"
		"float s1 = 20.0*sin(y+time*0.9)+30.0;"
		"float s2 = 20.0*cos(x+time)+30.0;"
		"float st = sin(y+time*2.0)*0.4+0.6;"
		"float ct = cos(x+time*2.0)*0.4+0.6;"
		"float c1 = sin(s1*cos(x+time) + s2*sin(y+time))*0.5+0.5;\n"
		"gl_FragColor = vec4( c1*0.9*st,c1*0.3*st,c1*0.9*ct,1.0);\n"
		"}\n";

		char fragmentShaderSource_plasma_colorbars4_ok[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"void main(void)\n"
		"{\n"
		"float x  = gl_FragCoord.x*(resolution.x);\n"
		"float y  = gl_FragCoord.y*(resolution.y);\n"
		"float s1 = 20.0*sin(y+time*0.9)+25.0;"
		"float s2 = 20.0*cos(x+time)+25.0;"
		"float st = sin(y+time*2.0)*0.4+0.6;"
		"float ct = cos(x+time*2.0)*0.4+0.6;"
		"float c1 = sin(s1*cos(x+time) + s1*sin(y+time))*0.5+0.5;\n"
		"gl_FragColor = vec4( c1*0.9*st,c1*0.3*st,c1*0.9*ct,1.0);\n"
		"}\n";

		char fragmentShaderSource_halfcloud[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"mat2 m = mat2( 0.8, 0.6, -0.6, 0.8);\n"
		"float fbm( vec2 p )\n"
		"{\n"
		"float f=0.0;\n"
		"f += 0.5000*noise1( p ); p*=m*2.02;\n"
		"f += 0.2500*noise1( p ); p*=m*2.03;\n"
		"f += 0.1250*noise1( p ); p*=m*2.01;\n"
		"f += 0.0625*noise1( p ); p*=m*2.04;\n"
		"f /= 0.9375;\n"
		"return f;\n"
		"}\n"
		"void main(void)\n"
		"{\n"
		"vec2 q  = gl_FragCoord.xy*resolution.xy;\n"
		"vec2 p  = -1.0+2.0*q;\n"
		"float background = smoothstep( -0.25, 0.25, p.x);\n"
		"float f = 0.8 - fbm(4.0*p);\n"
		"vec3 col = vec3(f,f,f);\n"
		"gl_FragColor = vec4( col*background ,1.0);\n"
		"}\n";

//#define PI 3.14159
char bars[]=STRINGIFY(
precision mediump float;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

vec2 position;
vec4 color;

float c = cos(time/2.0);
float s = sin(time/2.0);
mat2 R = mat2(c,-s,s,-c);

float barsize = 0.15;
float barsangle = 1.0;

vec4 mixcol(float value, float r, float g, float b)
{
	return vec4(value * r, value * g, value * b, value);
}

void bar(float pos, float r, float g, float b)
{
	 if ((position.y <= pos + barsize) && (position.y >= pos - barsize))
	 {
		 color = mixcol(1.0 - abs(pos - position.y) / barsize, r, g, b);
		 color.a = 1.0;
	 }
}

void main( void )
{

	position = ( gl_FragCoord.xy * resolution.xy );
	position = position * 2.0 - 1.0;
	position = position * R;

	color = vec4(0.0, 0.0, 0.0, 1.0);
	float t = time;

	bar(sin(t), 			1.0, 0.0, 0.0);
	bar(sin(t+barsangle/6.0*2.0), 	1.0, 1.0, 0.0);
	bar(sin(t+barsangle/6.0*4.0),  0.0, 1.0, 0.0);
	bar(sin(t+barsangle/6.0*6.0),  0.0, 1.0, 1.0);
	bar(sin(t+barsangle/6.0*8.0),  0.5, 0.0, 1.0);
	bar(sin(t+barsangle/6.0*10.0),  1.0, 0.0, 1.0);

	gl_FragColor = color;

}
);


char golf_ball[]=STRINGIFY(
	precision highp float;

	uniform float time;
	uniform vec2 resolution;

	float sphere(vec3 p)
	{
		return length(p)-5.0;
	}

	float cone(vec3 p)
	{
		vec3 q = p*4.0;
		q.y *= 0.125;

		float len = length(q.xz);
		vec2 vv = vec2(len, q.y);
		float d = dot(vec2(0.25,0.25),vv);
		return d;
	}

	float displacement(vec3 p)
	{
		return sin(p.x)*sin(p.y)*sin(p.z);
	}

	float scene( vec3 p )
	{
		float acone = cone(-p);
		float asphere = sphere(p*1.5);

		float both_shapes = min(acone, asphere);
		float both_shapes_with_displacement = both_shapes + displacement(p*8.0)*0.05;

			return both_shapes_with_displacement;
	}

	void main( void )
	{

		vec2 p = -1.0 + 2.0*gl_FragCoord.xy * resolution.xy;
		p.x *= resolution.y/resolution.x;

	  vec3 vuv=vec3(0.0,0.5,0.0);
	  vec3 vrp=vec3(0.0,1.0,0.0);
		float rs = time / 4.0;
		float st = sin(rs)*8.0;
		float ct = cos(rs)*8.0;
	  vec3 prp=vec3(st,4.0,ct);

	  vec3 vpn=normalize(vrp-prp);
	  vec3 u=normalize(cross(vuv,vpn));
	  vec3 v=cross(vpn,u);
	  vec3 vcv=(prp+vpn);
	  vec3 scrCoord=vcv+p.x*u+p.y*v;
	  vec3 scp=normalize(scrCoord-prp);

	  const vec3 e=vec3(0.1,0.0,0.0);
	  const float maxd=24.0;

	  float s=0.1;
	  vec3 c;
	  vec3 p1;
	  vec3 n;

	  float f=0.1;
	  for(int i=0;i<30;i++)
	  {
		f+=s;
		p1=prp+scp*f;
		s=scene(p1);
	  }

		c=vec3(0.2,0.6,0.8);
			n=normalize( vec3(s-scene(p1-e.xyy),s-scene(p1-e.yxy),s-scene(p1-e.yyx)));
			float b=dot(n,normalize(prp-p1));
			vec4 tex=vec4((b*c+pow(b,128.0))*(1.0-f*0.01),1.0);
		tex += vec4(0.99,0.66,0.2,1.0)/f*2.0;
		vec4 background=vec4(0.0,0.0,0.0,1.0);
		vec4 Color=(f<maxd)?tex:background;

		gl_FragColor=Color;
	}
);

char plasma_blue[]=STRINGIFY(
precision mediump float;

uniform float time;
uniform vec2 resolution;

vec2 Distort(vec2 p)
{
    float theta  = atan(p.y, p.x);
    float radius = length(p);
    radius = pow(radius, 1.3);
    p.x = radius * cos(theta);
    p.y = radius * sin(theta);
    return 0.5 * (p + 1.0);
}
vec4 pattern(vec2 p)
{
	vec2 m=mod(p.xy+p.x+p.y,2.0)-1.0;
	return vec4(length(m));
}

float hash(const float n)
{
	return fract(sin(n)*43758.5453);
}

float noise(const vec3 x)
{
	vec3 p=floor(x);
	vec3 f=fract(x);

    	f=f*f*(3.0-2.0*f);

    	float n=p.x+p.y*57.0+p.z*43.0;

    	float r1=mix(mix(hash(n+0.0),hash(n+1.0),f.x),mix(hash(n+57.0),hash(n+57.0+1.0),f.x),f.y);
    	float r2=mix(mix(hash(n+43.0),hash(n+43.0+1.0),f.x),mix(hash(n+43.0+57.0),hash(n+43.0+57.0+1.0),f.x),f.y);

	return mix(r1,r2,f.z);
}

void main( void )
{

	vec2 position = ( gl_FragCoord.xy * resolution.xy * 7.0 );

	float off = noise(position.xyx + time);
	vec4 c = pattern(Distort(position+off));

	c.xy = Distort(c.xy);
	gl_FragColor = vec4(c.x - off, sin(c.y) - off, cos(c.z), 1.0);
}
);

char planet_glow[]=STRINGIFY(
precision mediump float;

uniform float time;
uniform vec2 resolution;

float noise2D(vec2 uv) {
	uv = fract(uv)*1e3;
	vec2 f = fract(uv);
	uv = floor(uv);
	float v = uv.x+uv.y*1e3;
	vec4 r = vec4(v, v+1., v+1e3, v+1e3+1.);
	r = fract(1e5*sin(r*1e-2));
	f = f*f*(3.0-2.0*f);
	return (mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y));
}

float fractal(vec2 p) {
	float v = 0.5;
	v += noise2D(p*16.); v*=.5;
	v += noise2D(p*8.); v*=.5;
	v += noise2D(p*4.); v*=.5;
	v += noise2D(p*2.); v*=.5;
	v += noise2D(p*1.); v*=.5;
	return v;
}

vec3 func( vec2  p) {
	p = p*.1+.5;
	vec3 c = vec3(.0, .0, .1);
	vec2 d = vec2(time*.0001, 0.);
	c = mix(c, vec3(.8, .1, .1), pow(fractal(p*.20-d), 3.)*2.);
	c = mix(c, vec3(.9, .6, .6), pow(fractal(p.y*p*.10+d)*1.3, 3.));
	c = mix(c, vec3(1., 1., 1.), pow(fractal(p.y*p*.05+d*2.)*1.2, 1.5));
	return c;
}

void main( void ) {
	vec2 p = 2.0*gl_FragCoord.xy*resolution.xy - 1.0;
	p.x *= resolution.y/resolution.x;
	float d = length(p);
	p *= (acos(d) - 1.57079632)/d;
	gl_FragColor = vec4(func(p)*max(1.-d*d*d, 0.), 1.0);
}

);

char planet_glow2[]=STRINGIFY(
precision mediump float;

uniform float time;
uniform vec2 resolution;

float noise2D(vec2 uv) {
	uv = fract(uv)*1e3;
	vec2 f = fract(uv);
	uv = floor(uv);
	float v = uv.x+uv.y*1e3;
	vec4 r = vec4(v, v+1., v+1e3, v+1e3+1.0);
	r = fract(1e5*sin(r*1e-2));
	f = f*f*(3.0-2.0*f);
	return (mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y));
}

float fractal(vec2 p) {
	float v = 0.5;
	v += noise2D(p*4.0); v*=0.5;
	v += noise2D(p*1.0); v*=0.5;
	return v;
}

vec3 func( vec2  p) {
	p = p*0.1+0.5;
	vec3 c = vec3(0.0, 0.0, 0.1);
	vec2 d = vec2(time*0.0003, 0.0);
	c = mix(c, vec3(0.8, 0.2, 0.1), pow(fractal(p*0.20+d), 3.0)*2.0);
	c = mix(c, vec3(1.0, 1.0, 1.0), pow(fractal(p.y*p*0.05+d)*1.2, 1.5));
	return c;
}


void main( void )
{
	float t = 1.125+sin(time*0.2);
	t*=4.0;
	vec2 p = t*gl_FragCoord.xy*resolution.xy - (t*0.5);
	p.x *= resolution.y/resolution.x;
	float d = length(p);
	p *= (acos(d) - 1.57079632)/d;
	gl_FragColor = vec4(func(p)*max(1.0-d*d*d, 0.0), 1.0);
}

);
char simple_raymarch[]=STRINGIFY(
precision highp float;

uniform vec2 resolution;
uniform float time;

struct Camera
{
	vec3 position;
	vec3 lookAt;
	vec3 rayDir;
	vec3 forward;
	vec3 up;
	vec3 left;
};

float tri(float v)
{
	v = v/3.14159;
	return abs(fract(v)-0.5);
}

float GetDistanceToScene( vec3 p){

  float a = atan(p.x/p.z);

  float d = length(p.xz);
  float ground = p.y - clamp(min(tri(a*4.0),tri(d)),0.03,1.0/16.0);
  float sphere = length(p)-2.0;

  return max(ground,sphere);
}

void main(void)
{
	vec2 vPos = 2.0*gl_FragCoord.xy*resolution.xy - 1.0;
	float t = time*0.125;
	float screenAspectRatio = resolution.y/resolution.x;
	vec3 finalColor = vec3(0.1);

   //Camera setup
	Camera cam;
  	cam.lookAt = vec3(0.0,0.0,0.0);
	cam.position = vec3(sin(t)*3.0, 1.33, cos(t)*3.0);
	cam.position.xz += (cam.position.xz * sin(t)*0.35);

	cam.up = vec3(0.0,1.0,0.0);
  	cam.forward = normalize(cam.lookAt-cam.position);
  	cam.left = cross(cam.forward, cam.up);
 	cam.up = cross(cam.left, cam.forward);

	vec3 screenOrigin = (cam.position+cam.forward);
	vec3 screenHit = screenOrigin + vPos.x*cam.left*screenAspectRatio + vPos.y*cam.up;

	cam.rayDir = normalize(screenHit-cam.position);


  	const float MIN_DISTANCE = 0.01;
	const float MAX_DEPTH=64.0;
	const int MAX_STEPS = 164;

  	float distanceToScene = 1.0;
	float distanceFromCamera = 0.0;

  	vec3 raymarchPosition;

  	for(int i=0;i<MAX_STEPS;i++)
  	{
    		if (abs(distanceToScene)< MIN_DISTANCE || distanceFromCamera>MAX_DEPTH)
    			break;

		distanceFromCamera += distanceToScene;
    	raymarchPosition = cam.position+cam.rayDir*distanceFromCamera;
		distanceToScene = GetDistanceToScene(raymarchPosition);
  	}



	vec3 diffuseColor;
	vec3 normal;
	const vec3 e=vec3(0.01,0.0,0.0);

  	if (distanceFromCamera<MAX_DEPTH)
  	{

		float cy = raymarchPosition.y;
		diffuseColor = vec3(2.0*cy, 0.8, 1.0);
    		normal=normalize(
      			vec3(distanceToScene-GetDistanceToScene(raymarchPosition-e.xyy),
           		distanceToScene-GetDistanceToScene(raymarchPosition-e.yxy),
           		distanceToScene-GetDistanceToScene(raymarchPosition-e.yyx)));

		float lambert = dot(normal, -cam.rayDir);

    		finalColor = lambert*diffuseColor;
  	}

	gl_FragColor = vec4(finalColor, 1.0);
}
);


char bloberry[]=STRINGIFY(
precision mediump float;
uniform vec2 resolution;
uniform float time;

void main( void )
{
	vec2 res = 1.0/resolution.xy;
	float t=2.0;

	lowp vec4 P = vec4(0,0,-2,0);
	lowp vec4 D = gl_FragCoord/vec4(res/2.0,1,1)-1.0;
	lowp vec4 S = vec4(cos(t),-sin(t),cos(time*0.9),0)*0.8;
	lowp vec4 C = vec4(0.5,0.6,0.7,9);
	lowp vec4 f;
	lowp vec4 T;
	for (int r=0;r<70;r++)
	{
		T=P;
		T.w=dot(S,S)/(5.0+sin(time));
		lowp float s=D.z=1.0;
		lowp float k=dot(T,T);
		for(int m=0;m<7;m++)
		{
			if(k < 4.0)
			{
				s*=4.0*k;
				f=2.0*T.x*T;
				f.x=2.0*T.x*T.x-k;
				k=dot(T=f+S,T);
			}
		}
		s=sqrt(k/s)*log(k)/4.0;
		P+=D/length(D)*s;
		if(s < 0.002)
		{
			C*=log(k);
			r = 70;
		}
	}
	gl_FragColor=C+D.y*0.4;
}
);


char starfield[]=STRINGIFY(

precision mediump float;


uniform float time;
uniform vec2 resolution;

void main( void )
{
	vec3 light_color = vec3(0.2,0.1,0.1);
	vec2 q = 1.0 / resolution.xy;
	vec2 position = ( gl_FragCoord.xy -  q.xy*.5 ) / q.x;
	float angle = fract(atan(position.y,position.x)/(2.*3.14159265359));

	float angleFract = fract(angle*256.);
	float angleRnd = floor(angle*256.)+1.;
	float angleRnd1 = fract(angleRnd*fract(angleRnd*.7235)*45.1);
	float angleRnd2 = fract(angleRnd*fract(angleRnd*.82657)*13.724);
	float t = time*20.0+angleRnd1*10.;
	float radDist = sqrt(angleRnd2);

	float adist = radDist/length(position)*.1;
	float dist = abs(fract(t*.1+adist)-.5);
	float color =  (1.0 / (dist))*cos(0.7*(sin(t)))*adist/radDist/30.0;
	vec3 color1 = vec3(0.7, 0.25, 2.25*sin(time));
	vec3 color2 = vec3(0.25, 0.7, 0.5*cos(time));
	vec3 color3 = vec3(0.5*sin(time), 0.25, 0.7);

	vec3 final_color = vec3(0.05*cos(time), 0.025*cos(time*0.5), 0.05*sin(time));

	gl_FragColor = vec4(final_color * abs(sin(time * 0.25) + 1.5)*mod(gl_FragCoord.y, 2.0)*mod(gl_FragCoord.x, 2.0), 1);
	gl_FragColor = vec4(color,color,color,1.0)*vec4(light_color,1.0);
}
);

char fragmentShaderSource_eye3[]=STRINGIFY(
		precision highp float;
		uniform vec2 resolution;
		uniform float time;
		mat2 m = mat2( 0.80,  0.60, -0.60,  0.80 );

		float hash( float n )
		{
		    return fract(sin(n)*43758.5453);
		}

		float noise( in vec2 x )
		{
		    vec2 p = floor(x);
		    vec2 f = fract(x);
		    f = f*f*(3.0-2.0*f);
		    float n = p.x + p.y*57.0;
		    float res = mix(mix( hash(n+  0.0), hash(n+  1.0),f.x), mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y);
		    return res;
		}

		float fbm( vec2 p )
		{
		    float f = 0.0;
		    f += 0.50000*noise( p ); p = m*p*2.02;
		    f += 0.25000*noise( p ); p = m*p*2.03;
		    f += 0.12500*noise( p ); p = m*p*2.01;
		    f += 0.06250*noise( p ); p = m*p*10.0;
		    f += 0.03125*noise( p );
		    return f/0.984375;
		}

		float length2( vec2 p )
		{
		    float ax = abs(p.x);
		    float ay = abs(p.y);
		    return pow( pow(ax,4.0) + pow(ay,4.0), 1.0/4.0 );
		}

		void main(void)
		{
		    vec2 q = gl_FragCoord.xy * resolution.xy;
		    vec2 p = -1.0 + 2.0 * q;
		    p.x *= resolution.y/resolution.x;
		    float r = length( p );
		    float a = atan( p.y, p.x );
		    float dd = 0.2*sin(0.7*time);
		    float ss = 1.0 + clamp(1.0-r,0.0,1.0)*dd;
		    r *= ss;
		    vec3 col = vec3( 0.0, 0.3, 0.4 );
		    float f = fbm( 5.0*p );
		    col = mix( col, vec3(0.2,0.5,0.4), f );
		    col = mix( col, vec3(0.9,0.6,0.2), 1.0-smoothstep(0.2,0.6,r) );
		    a += 0.05*fbm( 20.0*p );
		    f = smoothstep( 0.3, 1.0, fbm( vec2(20.0*a,6.0*r) ) );
		    col = mix( col, vec3(1.0,1.0,1.0), f );
		    f = smoothstep( 0.4, 0.9, fbm( vec2(15.0*a,10.0*r) ) );
		    col *= 1.0-0.5*f;
		    col *= 1.0-0.25*smoothstep( 0.6,0.8,r );
		    f = 1.0-smoothstep( 0.0, 0.6, length2( mat2(0.6,0.8,-0.8,0.6)*(p-vec2(0.3,0.5) )*vec2(1.0,2.0)) );
		    col += vec3(1.0,0.9,0.9)*f*0.985;
		    col *= vec3(0.8+0.2*cos(r*a));
		    f = 1.0-smoothstep( 0.2, 0.25, r );
		    col = mix( col, vec3(0.0), f );
		    f = smoothstep( 0.79, 0.82, r );
		    col = mix( col, vec3(1.0,1.0,1.0), f );
		    gl_FragColor = vec4(col,1.0);
		}
);


//julia doesn work for some reason not handling t properly
char julia2[]=STRINGIFY(

precision mediump float;

uniform vec2 resolution;
uniform float time;

void main( void )
{
	float t=time*2.0;
	vec2 res = 1.0/resolution;
	lowp vec4 P=vec4(0.0, 0.0, -2.0, 0.0);
	lowp vec4 D=gl_FragCoord/vec4(res,1.0,1.0)-1.0;
	lowp vec4 S=vec4(cos(t),-sin(t),cos(time*0.9),0.0)*0.8;
	lowp vec4 C=vec4(0.5, 0.6, 0.7, 9.0);
	lowp vec4 f;
	lowp vec4 T;

	for(int r=0;r<70;r++)
	{
		T=P;
		T.w=dot(S,S)/(5.0+sin(time));
		lowp float s=D.z=1.0;
		lowp float k=dot(T,T);
		for(int m=0;m<7;m++)
		{
			if(k<4.0)
			{
				s*=4.0*k;
				f=2.0*T.x*T;
				f.x=2.0*T.x*T.x-k;
				k=dot(T=f+S,T);
			}
			s=sqrt(k/s)*log(k)/4.0;
			P+=D/length(D)*s;
		}
		if(s<0.002)
		{
			C*=log(k);
			break;
		}
	}
	gl_FragColor=C+D.y*0.4;
}
);

		char fragmentShaderSource_eye2[] =
		"precision highp float;\n"
		"uniform vec2 resolution;\n"
		"uniform float time;\n"
		"mat2 m = mat2( 0.8, 0.6, -0.6, 0.8);\n"
		"float hash( float n )\n"
		"{\n"
		"    return fract(sin(n)*43758.5453);\n"
		"}\n"
		"	float noise( in vec2 x )\n"
		"	{\n"
		"		vec2 p = floor(x);\n"
		"		vec2 f = x - floor(x);\n"
		"		f = f*f*(3.0-2.0*f);\n"
		"		float n = p.x + p.y*57.0\n;"
		"		float res = mix(mix( hash(n+  0.0), hash(n+  1.0),f.x), mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y);\n"
		"		return res;\n"
		"	}\n"
		"	float fbm( vec2 p )\n"
		"	{\n"
		"		float f = 0.0;\n"
		"		f += 0.50000*noise( p ); p = m*p*2.02;\n"
		"		f += 0.25000*noise( p ); p = m*p*2.03;\n"
		"		f += 0.12500*noise( p ); p = m*p*2.01;\n"
		"		f += 0.06250*noise( p ); p = m*p*10.0;\n"
		"		f += 0.03125*noise( p );\n"
		"		return f/0.984375;\n"
		"	}\n"
		"void main(void)\n"
		"{\n"
		"vec2 q  = gl_FragCoord.xy*resolution.xy;\n"
		"vec2 p  = -1.0+2.0*q;\n"
		"p.x *= resolution.y / resolution.x;\n"
		"float r = sqrt( dot(p,p) );\n"
		"float a = atan( p.y, p.x );\n"
		"vec3 col = vec3(1.0);\n"
		"float ss = 0.5 + 0.5*sin(2.0*time);\n"
		"float anim = 1.0+0.1*ss*clamp(1.0-r,0.0,1.0);\n"
		"r *= anim;\n"
		"if( r<0.8 )\n"
		"{\n"
		"	col = vec3( 0.0, 0.3, 0.4 );\n"
		"	float f = fbm(5.0*p);\n"
		"	col = mix( col, vec3(0.2, 0.5, 0.4), f);\n"
		"	f = 1.0-smoothstep(0.2, 0.5, r);\n"
		"   col = mix( col, vec3(0.9, 0.6, 0.2), f);\n"
		"   f = fbm( vec2(6.0*r,20.0*a) );\n"
		"   col = mix( col, vec3(1.0), f);\n"
		"   f = fbm( vec2(10.0*r,15.0*a) );\n"
		"   col *= 1.0 - 0.5*f;\n"
		"   f = smoothstep( 0.6, 0.8, r);\n"
		"   col *= 1.0 - 0.5*f;\n"
		"   f = smoothstep(0.2,0.25, r);\n"
		"	col *= f;\n"
		"   f = 1.0 - smoothstep(0.0, 0.5, length( p - vec2(0.24,0.2)));\n"
		"   col += vec3(1.0, 0.9, 0.8)*f*0.9;\n"
		"   f = smoothstep(0.75, 0.8, r);\n"
		"   col = mix( col, vec3(1.0), f);\n"
		"}\n"
		"gl_FragColor = vec4( col ,1.0);\n"
		"}\n";
/*
		char vertexShaderSource[] =
		"attribute vec4 vPosition;   \n"
		"uniform mat4 MVP;			 \n"
		"void main()                 \n"
		"{                           \n"
		"   gl_Position = MVP * vPosition; \n"
		"}                           \n";
*/

		char blank[]=STRINGIFY(
		precision mediump float;
		varying vec4 v_color;
		uniform vec2 resolution;
		uniform float time;
		void main( void )
		{
		    gl_FragColor = vec4(v_color.x*0.5,v_color.y*1.0,v_color.z*0.5,1.0);
		}
		);


		char vertexShaderSource[]=STRINGIFY(
		attribute vec4 vPosition;
		uniform mat4 MVP;
		varying vec4 v_color;
		void main( void )
		{
			v_color = vPosition;
			gl_Position = MVP * vPosition;
		}
		);

/*
 *
 *


		"float fbm( vec2 p )\n"
		"{\n"
		"float f=0.0;\n"
		"f += 0.5000*noise1( p ); p*=m*2.02;\n"
		"f += 0.2500*noise1( p ); p*=m*2.03;\n"
		"f += 0.1250*noise1( p ); p*=m*2.01;\n"
		"f += 0.0625*noise1( p ); p*=m*2.04;\n"
		"f /= 0.9375;\n"
		"return f;\n"
		"}\n"
		"void main(void)\n"
		"{\n"
		"vec2 q  = gl_FragCoord.xy*resolution.xy;\n"
		"vec2 p  = -1.0+2.0*q;\n"
		"p.x *= resolution.y / resolution.x;\n"
		"float r = sqrt( dot(p,p) );\n"
		"float a = atan( p.y, p.x );\n"
		"vec3 col = vec3(1.0);\n"
		"float ss = 0.5 + 0.5*sin(2.0*time);\n"
		"float anim = 1.0+0.1*ss*clamp(1.0-r,0.0,1.0);\n"
		"r *= anim;\n"
		"if( r<0.8 )\n"
		"{\n"
		"	col = vec3( 0.0, 0.3, 0.4 );\n"
		"	float f = fbm(5.0*p);\n"
		"	col = mix( col, vec3(0.2, 0.5, 0.4), f);\n"
		"	f = 1.0-smoothstep(0.2, 0.5, r);\n"
		"   col = mix( col, vec3(0.9, 0.6, 0.2), f);\n"
		"   f = fbm( vec2(6.0*r,20.0*a) );\n"
		"   col = mix( col, vec3(1.0), f);\n"
		"   f = fbm( vec2(10.0*r,15.0*a) );\n"
		"   col *= 1.0 - 0.5*f;\n"
		"   f = smoothstep( 0.6, 0.8, r);\n"
		"   col *= 1.0 - 0.5*f;\n"
		"   f = smoothstep(0.2,0.25, r);\n"
		"	col *= f;\n"
		"   f = 1.0 - smoothstep(0.0, 0.5, length( p - vec2(0.24,0.2)));\n"
		"   col += vec3(1.0, 0.9, 0.8)*f*0.9;\n"
		"   f = smoothstep(0.75, 0.8, r);\n"
		"   col = mix( col, vec3(1.0), f);\n"
		"}\n"
		"gl_FragColor = vec4( col ,1.0);\n"
		"}\n";

 */
void checkGLError(const char* where) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		lprintfln("%s: glGetError returned %x", where, err);
	}
}



///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint loadShader(const char *shaderSrc, GLenum type) {
	lprintfln("loading shader: %s", shaderSrc);
	checkGLError("begin loading shader");

	GLuint shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(type);
	checkGLError("glCreateShader");

	if (shader == 0) {
		lprintfln("Invalid shader handle!");
		return 0;
	}
	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);
	checkGLError("glShaderSource");

	// Compile the shader
	glCompileShader(shader);
	checkGLError("glCompileShader");

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	checkGLError("glGetShaderiv");

	if (!compiled) {
		lprintfln("Error compiling shader");
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen == 0) // android bug.
			infoLen = 1024;
		if (infoLen > 1) {
			char* infoLog = (char*) malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			//printf("Error compiling shader:\n%s\n", infoLog);
			lprintfln("Error compiling shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

class MyGLMoblet: public GLMoblet {
private:
	GLuint mShader;
	GLuint mTimeLoc;
	GLuint mResolutionLoc;
	GLuint mMatrixMVP;
	GLuint mElementbuffer;
	GLuint mVertexbuffer;
	Graph::BarMgr mBars;		//only temp
	std::vector<unsigned short> mIndices;

	int mWidth, mHeight;

public:
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) {
	}

	/**
	 * This method is called when a key is pressed.
	 */
	void keyPressEvent(int keyCode, int nativeCode)
	{
		switch (keyCode)
		{
			// Close the application if the back key, key 0,
			// or the left "softkey" is pressed (the left
			// softkey is usually located at the top left of
			// the keypad of phones that have a keypad, and
			// is often used as a back navigation key).
			case MAK_BACK:
			case MAK_0:
			case MAK_SOFTLEFT:
				// Exit the application.
				maExit(0);
				break;
		}
	}

	int initGL() {


		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint programObject;
		GLint linked;



		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);
		// enable back face culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);


		// Load the vertex/fragment shaders
		vertexShader = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
		checkGLError("Load vertex shader");

		fragmentShader = loadShader(blank, GL_FRAGMENT_SHADER);
		checkGLError("Load fragment shader");

		// Create the program object
		programObject = glCreateProgram();
		if (programObject == 0) {
			lprintfln("Could not create program!");
			return FALSE;
		}
		checkGLError("Create program");

		glAttachShader(programObject, vertexShader);
		checkGLError("Attach vertex shader");

		glAttachShader(programObject, fragmentShader);
		checkGLError("Attach fragment shader");

		// Bind vPosition to attribute 0
		glBindAttribLocation(programObject, 0, "vPosition");
		checkGLError("Bind vPosition to vertex shader");


		// Link the program
		glLinkProgram(programObject);

		// Check the link status
		glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
		if (!linked) {
			lprintfln("Failed to link shader!");
			GLint infoLen = 0;
			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen == 0) // android bug.
				infoLen = 1024;
			if (infoLen > 1) {
				char* infoLog = (char*) malloc(sizeof(char) * infoLen);
				glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
				//esLogMessage("Error linking program:\n%s\n", infoLog);
				lprintfln("Error linking program:\n%s\n", infoLog);
				free(infoLog);
			}
			glDeleteProgram(programObject);
			return FALSE;
		}
		// Store the program object
		mShader = programObject;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mTimeLoc = glGetUniformLocation(mShader, "time");
		mResolutionLoc = glGetUniformLocation(mShader, "resolution");
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		mMatrixMVP = glGetUniformLocation(mShader, "MVP");

		mBars.addBars(100);
		// make a copy for now because we might need to add multiple indicies to this obj (there is only one index list)
		Graph::Bar &bar = mBars.getBar(0);
		for(int i=0;i<bar.faces().size();i++)
		{
			mIndices.push_back(bar.faces()[i]);
		}
		lprintfln("Indices to:%d",mIndices.size());
		lprintfln("Indices to:%d",bar.vertices().size());
		// Generate a buffer for the vertices
		glGenBuffers(1, &mVertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, bar.vertices().size() * sizeof(glm::vec3), &bar.vertices()[0], GL_STATIC_DRAW);


		// Generate a buffer for the indices
		glGenBuffers(1, &mElementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned short), &mIndices[0], GL_STATIC_DRAW);

		return TRUE;
	}

	int mStartTime;

	void init()
	{
		mWidth = EXTENT_X(maGetScrSize());
		mHeight = EXTENT_Y(maGetScrSize());

		if(!initGL())
			maPanic(1, "Failed to initialize!");

		mStartTime = maGetMilliSecondCount();
	}

	glm::mat4 calculateMatrix(float tick, glm::mat4 &Model, float value)
	{
		// Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 Projection = glm::perspective(45.0f, 3.0f / 4.0f, 0.1f, 100.0f);
		// Camera matrix
		glm::mat4 View       = glm::lookAt(
			glm::vec3(0.0f,5.0f,10.0f), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	//	Model matrix : an identity matrix (model will be at the origin)
//		Model				= glm::mat4(1);
//		Model 				= glm::scale(Model,1.0f,value,1.0f);
//		Model 				= glm::rotate(Model,20.0f*tick,0.0f,1.0f,0.0f);
//		Model 				= glm::translate(Model,1.0f,0.0f,0.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		return Projection * View * Model; // Remember, matrix multiplication is the other way around
	}

	void draw()
	{

		float tick = (maGetMilliSecondCount() - mStartTime) * 0.001f;
		// Set the viewport
		glViewport(0, 0, mWidth, mHeight);

		// Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glCullFace(GL_FRONT);
		// Use the program object   shader and its specific arguments
		glUseProgram(mShader);
		checkGLError("glUseProgram");

		// Generate a buffer for the vertices
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		checkGLError("glEnableVertexAttribArray");

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementbuffer);
		glm::mat4 r = glm::rotate(20.0f*tick,2.0f,1.0f,0.0f);


		int grid = static_cast<int>(static_cast<float>(mBars.size())*0.1f);
		for(int j=0; j<grid; j++)
		{

		for(int i=0; i<grid; i++)
		{
			Graph::Bar &bar = mBars.getBar(i*j);
			float sc = 1.0f;
			bar.setValue(sc*1.1f+sc*1.0f*sin(j*0.1f+i*0.1f+1.3f*tick));
			glm::mat4 t = glm::translate(0.5f-grid*0.5f+i*1.0f,0.0f,0.5f-grid*0.5f+j*1.0f);
			glm::mat4 s = glm::scale(0.5f,bar.getValue(),0.5f);
			glm::mat4 m = r*s*t;
			bar.setMatrix(m);

			glm::mat4 MVP = calculateMatrix(tick, bar.getMatrix(),bar.getValue());
/*
			// Use the program object   shader and its specific arguments
			glUseProgram(mShader);
			checkGLError("glUseProgram");
*/
			glUniform1f(mTimeLoc, tick);
			checkGLError("glUniform1f");

			glUniform2f(mResolutionLoc, 1.0f/(float)mWidth, 1.0f/(float)mHeight);
			checkGLError("glUniform2f");

			// Send our transformation to the currently bound shader,
			// in the "MVP" uniform
			// For each model you render, since the MVP will be different (at least the M part)
			glUniformMatrix4fv(mMatrixMVP, 1, GL_FALSE, &MVP[0][0]);
/*
			// Generate a buffer for the vertices
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, mVertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			checkGLError("glEnableVertexAttribArray");

			// Index buffer
			 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementbuffer);
*/
			 // Draw the triangles !
			 glDrawElements(
				 GL_TRIANGLES,      // mode
				 mIndices.size(),    // count
				 GL_UNSIGNED_SHORT,   // type
				 (void*)0           // element array buffer offset
			 );
			 checkGLError("glDrawElements");

		}
		}

	//	 glDisableVertexAttribArray(0);
	}
};

/**
 * Entry point of the program. The MAMain function
 * needs to be declared as extern "C".
 */
extern "C" int MAMain() {
	Moblet::run(new MyGLMoblet());
	return 0;
}
