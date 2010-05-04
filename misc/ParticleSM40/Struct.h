//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - foulon matthieu - stardeath@wanadoo.fr				//
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

#pragma once

float random()
{
	return float(rand()) / float(RAND_MAX);
}

float random(float min, float max)
{
	return (max-min)*random()+min;
}

#define float4 D3DXVECTOR4

typedef struct int4
{
	__int32 x, y, z, w;
} int4, *pint4;

//--------------------------------------------------------------------------------------------------
// déclaration des structures spéciales gpu
//--------------------------------------------------------------------------------------------------
struct ParticleData
{
	D3DXVECTOR4 data1; // Position : xyz; Age : w
	D3DXVECTOR4 data2; // Velocity : xyz; Energy : w
	D3DXVECTOR4 data3; // OldPosition : xyz; LifeTime : w
	D3DXVECTOR4 data4; // Color
	D3DXVECTOR4 data5; // PARAM_SIZE : x; PARAM_MASS : y; PARAM_ANGLE : z; PARAM_TEXTURE_INDEX : w
	D3DXVECTOR4 data6; // PARAM_ROTATION_SPEED : x; SqrDist : y; Group : z

	D3DXVECTOR3& Position()		{return *((D3DXVECTOR3*)&data1);}
	float& Age()				{return data1.w;}
	D3DXVECTOR3& Velocity()		{return *((D3DXVECTOR3*)&data2);}
	float& Energy()				{return data2.w;}
	D3DXVECTOR3& OldPosition()	{return *((D3DXVECTOR3*)&data3);}
	float& LifeTime()			{return data3.w;}
	D3DXVECTOR4& Color()		{return data4;}
	float& ParamSize()			{return data5.x;}
	float& ParamMass()			{return data5.y;}
	float& ParamAngle()			{return data5.z;}
	float& ParamTexIndex()		{return data5.w;}
	float& ParamRotSpeed()		{return data6.x;}
	float& SqrDist()			{return data6.y;}
	float& Group()				{return data6.z;}
	float& Random()				{return data6.w;}
};

struct Interpolator
{
	// rien de prévu pour le random interpolator
	float4 data1; // type
	float4 data2;
	float4 data3;

	float& Type()					{return data1.x;}
	float& ParamStart1()			{return data2.x;}
	float& ParamEnd1()				{return data3.x;}
	D3DXVECTOR2& ParamStart2()		{return *((D3DXVECTOR2*)&data2);}
	D3DXVECTOR2& ParamEnd2()		{return *((D3DXVECTOR2*)&data3);}
	D3DXVECTOR3& ParamStart3()		{return *((D3DXVECTOR3*)&data2);}
	D3DXVECTOR3& ParamEnd3()		{return *((D3DXVECTOR3*)&data3);}
	D3DXVECTOR4& ParamStart4()		{return *((D3DXVECTOR4*)&data2);}
	D3DXVECTOR4& ParamEnd4()		{return *((D3DXVECTOR4*)&data3);}
};


struct Group
{
	float4 data1; // particleData, colorInterpolator, paramInterpolator 1 et 2
	float4 data2; // paramInterpolator 3, 4 et 5
	float4 data3; // emitters
	float4 data4; // modifiers
	float4 data5; // minLifeTime, maxLifeTime, radius, immortal
	float4 data6; // still, capacity

	float& ColorInterpolator()			{return data1.y;}
	float& ParamSizeInterpolator() 		{return data1.z;}
	float& ParamMassInterpolator() 		{return data1.w;}
	float& ParamAngleInterpolator() 	{return data2.x;}
	float& ParamTexIndexInterpolator() 	{return data2.y;}
	float& ParamRotSpeedInterpolator() 	{return data2.z;}
	float& MinLifeTime()				{return data5.x;}
	float& MaxLifeTime()				{return data5.y;}
	float& Radius()						{return data5.z;}
	float& Immortal()					{return data5.w;}
	float& Still()						{return data6.x;}
	float& Capacity()					{return data6.y;}
	float4& Modifiers()					{return data4;}
};

struct CreationData 
{
	D3DXVECTOR4 data1; // Position : xyz; Number : w
	D3DXVECTOR4 data2; // Velocity : xyz; Full : w

	D3DXVECTOR3& Position()	{return *((D3DXVECTOR3*)&data1);}
	float& Number()			{return data1.w;}
	D3DXVECTOR3& Velocity()	{return *((D3DXVECTOR3*)&data2);}
	float& Full()			{return data2.w;}
};

enum EmitterType
{
	RandomEmitter = 1,
	SphericEmitter = 2,
	StaticEmitter = 3,
	StraightEmitter = 4,
};

enum ZoneType
{
	PointZone = 51,
	SphereZone = 52,
	PlaneZone = 53,
};

enum InterpolatorType
{
	DefaultInitializer = 101,
	RandomInitializer = 102,
	RandomInterpolator = 103,
	SimpleInterpolator = 104,
};

enum ModifierType
{
	FrictionModifier = 151,
	GravityModifier = 152,
	ObstacleModifier = 153,
	RotatorModifier = 154,
};

struct Zone
{
	D3DXVECTOR4 data1; // position, radius
	D3DXVECTOR4 data2; // tPosition, type
	D3DXVECTOR4 data3; // normal
	D3DXVECTOR4 data4; // tNormal

	D3DXVECTOR3& Position()		{return *((D3DXVECTOR3*)&data1);}
	float& Radius()				{return data1.w;}
	D3DXVECTOR3& TPosition()	{return *((D3DXVECTOR3*)&data2);}
	float& Type()				{return data2.w;}
	D3DXVECTOR3& Normal()		{return *((D3DXVECTOR3*)&data3);}
	D3DXVECTOR3& TNormal()		{return *((D3DXVECTOR3*)&data4);}

	void generatePosition(D3DXVECTOR3& v, bool full, float radius = 0.0f);
};



struct Emitter
{
	float4 data1; // zone, active, tank, flow, 
	float4 data2; // forceMin, forceMax, full, fraction
	float4 data3; // direction, angleMin
	float4 data4; // tDirection, angleMax
	float4 data5; // matrix 1, 2, 3, cosAngleMin
	float4 data6; // matrix 4, 5, 6, cosAngleMax
	float4 data7; // matrix 7, 8, 9, type

	float& Zone()				{return data1.x;}
	float& Active()				{return data1.y;}
	float& Tank()				{return data1.z;}
	float& Flow()				{return data1.w;}

	float& ForceMin()			{return data2.x;}
	float& ForceMax()			{return data2.y;}
	float& Full()				{return data2.z;}
	float& Fraction()			{return data2.w;}

	D3DXVECTOR3& Direction()	{return *((D3DXVECTOR3*)&data3);}
	float& AngleMin()			{return data3.w;}

	D3DXVECTOR3& TDirection()	{return *((D3DXVECTOR3*)&data4);}
	float& AngleMax()			{return data4.w;}

	float& Matrix0()			{return data5.x;}
	float& Matrix1()			{return data5.y;}
	float& Matrix2()			{return data5.z;}
	float& CosAngleMin()		{return data5.w;}

	float& Matrix3()			{return data6.x;}
	float& Matrix4()			{return data6.y;}
	float& Matrix5()			{return data6.z;}
	float& CosAngleMax()		{return data6.w;}

	float& Matrix6()			{return data7.x;}
	float& Matrix7()			{return data7.y;}
	float& Matrix8()			{return data7.z;}
	float& Type()				{return data7.w;}

	void generateVelocity(ParticleData& particle, float speed);
	void emit(ParticleData& particle);
};

struct Modifier
{
	float4 data1; // type, active, local, zone
	float4 data2; // value, bouncingRatio
	float4 data3; // tValue, friction

	float& Type()				{return data1.x;}
	float& Active()				{return data1.y;}
	float& Local()				{return data1.z;}
	float& Zone()				{return data1.w;}

	D3DXVECTOR3& Value()		{return *((D3DXVECTOR3*)&data2);}
	float& BouncingRatio()		{return data2.w;}

	D3DXVECTOR3& TValue()		{return *((D3DXVECTOR3*)&data3);}
	float& Friction()			{return data3.w;}
};
//--------------------------------------------------------------------------------------------------
//  fin de la déclaration des structures spéciales gpu
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// déclaration des fonctions
//--------------------------------------------------------------------------------------------------
Emitter		createRandomEmitter(int zone, bool full, int tank, float flow, float forceMin, float forceMax);
Zone		createPointZone(D3DXVECTOR3 position);
Zone		createSphereZone(D3DXVECTOR3 position,float radius);
Zone		createPlaneZone(D3DXVECTOR3 position, D3DXVECTOR3 normal);
Group		createGroup(int capacity);

Interpolator createDefaultInitializer();
Interpolator createSimpleInterpolator();

Modifier createGravityModifier(D3DXVECTOR3 value);
Modifier createObstacleModifier(float zone, float bouncingRatio = 1.0f, float friction = 1.0f);
//--------------------------------------------------------------------------------------------------
// fin de la déclaration des fonctions
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// conteneurs
//--------------------------------------------------------------------------------------------------
vector<ParticleData>				g_vecParticles;
vector<Group>						g_vecGroups;
vector<Emitter>						g_vecEmitters;
vector<Zone>						g_vecZones;
vector<Interpolator>				g_vecInterpolators;
vector<CreationData>				g_vecCreationData;
vector<Modifier>					g_vecModifiers;
//--------------------------------------------------------------------------------------------------
// fin des conteneurs
//--------------------------------------------------------------------------------------------------

Interpolator createDefaultInitializer(D3DXVECTOR4 defaultValue)
{
	Interpolator result;
	ZeroMemory(&result, sizeof(result));

	result.Type() = DefaultInitializer;
	result.ParamStart4() = defaultValue;

	return result;
}

Interpolator createSimpleInterpolator(D3DXVECTOR4 startValue, D3DXVECTOR4 endValue)
{
	Interpolator result;
	ZeroMemory(&result, sizeof(result));

	result.Type() = SimpleInterpolator;
	result.ParamStart4() = startValue;
	result.ParamEnd4() = endValue;

	return result;
}

Emitter createRandomEmitter(int zone, bool full, int tank, float flow, float forceMin, float forceMax)
{
	Emitter result;
	ZeroMemory(&result, sizeof(result));

	result.Type() = RandomEmitter;
	result.Zone() = float(zone);
	result.Full() = (full) ? (1.0f) : (0.0f);
	result.Tank() = float(tank);
	result.Flow() = flow;
	result.ForceMin() = forceMin;
	result.ForceMax() = forceMax;

	return result;
}

void Emitter::emit(ParticleData& particle)
{
	g_vecZones[(unsigned int)Zone()].generatePosition( particle.Position(), ( (Full() == 0.0f)?(false):(true) ) );
	generateVelocity(particle, random(ForceMin(), ForceMax()) / particle.ParamMass());
}

void Emitter::generateVelocity(ParticleData& particle, float speed)
{
	switch( (unsigned int)Type() )
	{
	case RandomEmitter:
		{
			float sqrNorm;

			do 
			{
				particle.Velocity() = D3DXVECTOR3(2.0f * float(rand()) / float(RAND_MAX) - 1.0f, 2.0f * float(rand()) / float(RAND_MAX) - 1.0f, 2.0f * float(rand()) / float(RAND_MAX) - 1.0f);
				sqrNorm = D3DXVec3LengthSq(&particle.Velocity());
			}
			while((sqrNorm > 1.0f) || (sqrNorm == 0.0f));

			particle.Velocity() *= speed / sqrt(sqrNorm);
		}
		break;
	case SphericEmitter:
		{
			float a = random(CosAngleMax(), CosAngleMin());
			float theta = acos(a);
			float phi = random(0.0f, 2.0f * 3.141592f);

			float sinTheta = sin(theta);
			float x = sinTheta * cos(phi);
			float y = sinTheta * sin(phi);
			float z = cos(theta);

			particle.Velocity().x = speed * (Matrix0() * x + Matrix1() * y + Matrix2() * z);
			particle.Velocity().y = speed * (Matrix3() * x + Matrix4() * y + Matrix5() * z);
			particle.Velocity().z = speed * (Matrix6() * x + Matrix7() * y + Matrix8() * z);
		}
		break;
	case StaticEmitter:
		{
			particle.Velocity() = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // no initial velocity
		}
		break;
	case StraightEmitter:
		{
			particle.Velocity() = TDirection();
			particle.Velocity() *= speed;
		}
		break;
	}
}



Zone createPointZone(D3DXVECTOR3 position)
{
	Zone result;
	ZeroMemory(&result, sizeof(result));

	result.Position() = position;
	result.TPosition() = position;
	result.Type() = PointZone;

	return result;
}

Zone createSphereZone(D3DXVECTOR3 position,float radius)
{
	Zone result;
	ZeroMemory(&result, sizeof(result));

	result.Position() = position;
	result.TPosition() = position;
	result.Radius() = radius;
	result.Type() = SphereZone;

	return result;
}

Zone createPlaneZone(D3DXVECTOR3 position, D3DXVECTOR3 normal)
{
	Zone result;
	ZeroMemory(&result, sizeof(result));

	result.Position() = position;
	result.TPosition() = position;
	D3DXVECTOR3 NormalizedNormal(normal);
	D3DXVec3Normalize(&NormalizedNormal, &NormalizedNormal);
	result.Normal() = NormalizedNormal;
	result.TNormal() = NormalizedNormal;
	result.Type() = PlaneZone;

	return result;
}

void Zone::generatePosition(D3DXVECTOR3& v, bool full, float radius)
{
	switch( (unsigned int)Type() )
	{
	case PointZone:
		{
			v = TPosition();
		}
		break;
	case SphereZone:
		{
			const float relRadius = Radius() - radius;

			do v = D3DXVECTOR3(random(-relRadius, relRadius), random(-relRadius, relRadius), random(-relRadius, relRadius));
			while( D3DXVec3LengthSq(&v) > relRadius * relRadius );

			if (!full)
				v *= Radius() / D3DXVec3Length(&v);

			v += TPosition();
		}
		break;
	}
}

Group createGroup(int capacity)
{
	Group group;
	ZeroMemory(&group, sizeof(group));

	group.Capacity() = float(capacity);
	group.MinLifeTime() = 1.0f;
	group.MaxLifeTime() = 1.0f;
	group.Immortal() = 0.0f;
	group.Still() = 0.0f;
	group.Radius() = 0.0f;

	// -1 étant l'équivalent du pointeur NULL
	group.ColorInterpolator() = -1.0f;
	group.ParamSizeInterpolator() = -1.0f;
	group.ParamMassInterpolator() = -1.0f;
	group.ParamAngleInterpolator() = -1.0f;
	group.ParamTexIndexInterpolator() = -1.0f;
	group.ParamRotSpeedInterpolator() = -1.0f;

	group.Modifiers() = D3DXVECTOR4(-1.0f, -1.0f, -1.0f, -1.0f);

	//reallocate(capacity);

	return group;
}

Modifier createGravityModifier(D3DXVECTOR3 value)
{
	Modifier modifier;
	ZeroMemory(&modifier, sizeof(modifier));

	modifier.Type() = GravityModifier;
	modifier.Value() = value;
	modifier.TValue() = value;

	return modifier;
}

Modifier createObstacleModifier(int zone, float bouncingRatio, float friction)
{
	Modifier modifier;
	ZeroMemory(&modifier, sizeof(modifier));

	modifier.Type() = ObstacleModifier;
	modifier.Zone() = float(zone);
	modifier.BouncingRatio() = bouncingRatio;
	modifier.Friction() = friction;

	return modifier;
}