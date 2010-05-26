//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

#ifndef H_SPK_COLOR
#define H_SPK_COLOR

#include <iostream> // for operator <<

#include "Core/SPK_DEF.h"

namespace SPK
{
	/**
	* @brief Structure defining a Color in ARGB format
	*/
	class Color
	{
	public :

		///////////////
		// Parameter //
		///////////////
		
		// Stored in RGBA format
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;

		//////////////////
		// Constructors //
		//////////////////

		inline Color(unsigned long int rgba = 0xFFFFFFFF);
		inline Color(int r,int g,int b,int a);

		///////////////
		// Operators //
		///////////////

		inline Color& operator+=(const Color& c);
		inline Color& operator-=(const Color& c);
		inline operator unsigned long int();

		/////////////
		// Setters //
		/////////////

		inline void setR(int r);
		inline void setG(int g);
		inline void setB(int b);
		inline void setA(int a);

		inline void setRGBA(unsigned long int rgba);
		inline void setARGB(unsigned long int argb);
		inline void setBGRA(unsigned long int bgra);

		inline void set(int r,int g,int b,int a);
		
		/////////////
		// Getters //
		/////////////

		inline int getR() const;
		inline int getG() const;
		inline int getB() const;
		inline int getA() const;

		inline unsigned long int getRGBA() const;
		inline unsigned long int getARGB() const;
		inline unsigned long int getBGRA() const;

		///////////////
		// Interface //
		///////////////

		inline void interpolate(const Color& c0,const Color& c1,float ratio);
	};

	////////////////////////
	// External operators //
	////////////////////////

	inline Color operator+(const Color& c0,const Color& c1);
	inline Color operator-(const Color& c0,const Color& c1);
	inline bool operator==(const Color& c0,const Color& c1);
	inline bool operator!=(const Color& c0,const Color& c1);
	inline std::ostream& operator<<(std::ostream& s,const Color& c);

	/////////////////
	// Definitions //
	/////////////////

	inline Color::Color(unsigned long int rgba) :
		r((unsigned char)(rgba >> 24)),
		g((unsigned char)(rgba >> 16)),
		b((unsigned char)(rgba >> 8)),
		a((unsigned char)(rgba))
	{}

	inline Color::Color(int r,int g,int b,int a) :
		r(r),
		g(g),
		b(b),
		a(a)
	{}

	inline Color& Color::operator+=(const Color& c)
	{
		int a = getA() + c.getA();
		if (a > 255) a = 255;
		int r = getR() + c.getR();
		if (r > 255) r = 255;
		int g = getG() + c.getG();
		if (g > 255) g = 255;
		int b = getB() + c.getB();
		if (b > 255) b = 255;
		set(a,r,g,b);
		return *this;
	}

	inline Color& Color::operator-=(const Color& c)
	{
		int a = getA() - c.getA();
		if (a < 0) a = 0;
		int r = getR() - c.getR();
		if (r < 0) r = 0;
		int g = getG() - c.getG();
		if (g < 0) g = 0;
		int b = getB() - c.getB();
		if (b < 0) b = 0;
		set(a,r,g,b);
		return *this;
	}

	inline Color::operator unsigned long int()
	{
		return getRGBA();
	}

	inline void Color::setR(int r) { this->r = r; }
	inline void Color::setG(int g) { this->g = g; }
	inline void Color::setB(int b) { this->b = b; }
	inline void Color::setA(int a) { this->a = a; }

	inline void Color::setRGBA(unsigned long int rgba)
	{
		r = (unsigned char)(rgba >> 24);
		g = (unsigned char)(rgba >> 16);
		b = (unsigned char)(rgba >> 8);
		a = (unsigned char)(rgba);
	}

	inline void Color::setARGB(unsigned long int argb)
	{
		a = (unsigned char)(argb >> 24);
		r = (unsigned char)(argb >> 16);
		g = (unsigned char)(argb >> 8);
		b = (unsigned char)(argb);
	}

	inline void Color::setBGRA(unsigned long int bgra)
	{
		b = (unsigned char)(bgra >> 24);
		g = (unsigned char)(bgra >> 16);
		r = (unsigned char)(bgra >> 8);
		a = (unsigned char)(bgra);
	}

	inline void Color::set(int r,int g,int b,int a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline int Color::getR() const { return r; }
	inline int Color::getG() const { return g; }
	inline int Color::getB() const { return b; }
	inline int Color::getA() const { return a; }

	inline unsigned long int Color::getRGBA() const
	{
		return (r << 24) | (g << 16) | (b << 8) | a;
	}

	inline unsigned long int Color::getARGB() const
	{
		return (a << 24) | (r << 16) | (g << 8) | b;
	}

	inline unsigned long int Color::getBGRA() const
	{
		return (b << 24) | (g << 16) | (r << 8) | a;
	}

	inline void Color::interpolate(const Color& c0,const Color& c1,float ratio)
	{
		int iRatio = static_cast<int>(ratio * 256.0f); // We use fixed point to speed up the interpolation
		int invRatio = 256 - iRatio;
		r = (unsigned char)((c0.r * invRatio + c1.r * iRatio) >> 8);
		g = (unsigned char)((c0.g * invRatio + c1.g * iRatio) >> 8);
		b = (unsigned char)((c0.b * invRatio + c1.b * iRatio) >> 8);
		a = (unsigned char)((c0.a * invRatio + c1.a * iRatio) >> 8);
	}

	inline Color operator+(const Color& c0,const Color& c1)
	{
		Color c(c0);
		return c += c1;
	}

	inline Color operator-(const Color& c0,const Color& c1)
	{
		Color c(c0);
		return c -= c1;
	}

	inline bool operator==(const Color& c0,const Color& c1)
	{
		return c0.getRGBA() == c1.getRGBA();
	}

	inline bool operator!=(const Color& c0,const Color& c1)
	{
		return c0.getRGBA() != c1.getRGBA();
	}

	inline std::ostream& operator<<(std::ostream& s,const Color& c)
	{
		return s << "0x" << std::hex << c.getRGBA() << std::dec;
	}

	// Specialization of the random generation of colors
	template<>
	inline Color SPKContext::generateRandom(const Color& c0,const Color& c1)
	{
		return Color(
			generateRandom(c0.getR(),c1.getR()),
			generateRandom(c0.getG(),c1.getG()),
			generateRandom(c0.getB(),c1.getB()),
			generateRandom(c0.getA(),c1.getA()));
	}
}

#endif
