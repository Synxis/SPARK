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

#define blocksize 128

//groupshared uint sharedRand[blocksize];

#define PI 3.14159265f

cbuffer cbCS : register( b0 )
{
    uint4   g_param;    // pcbCS->param[0] = MAX_PARTICLES;
                        // pcbCS->param[1] = dimx;              
    float4  g_paramf;   // pcbCS->paramf[0] = 0.1f; elapsedTime
                        // pcbCS->paramf[1] = 1; timeFromStart
						// pcbCS->paramf[2] = 0..1;
};

#define elapsedTime g_paramf.x

#define RandomEmitter 1
#define SphericEmitter 2
#define StaticEmitter 3
#define StraightEmitter 4

#define PointZone 51
#define SphereZone 52
#define PlaneZone 53

#define DefaultInitializer 101
#define RandomInitializer 102
#define RandomInterpolator 103
#define SimpleInterpolator 104

#define FrictionModifier 151
#define GravityModifier 152
#define ObstacleModifier 153
#define RotatorModifier 154

struct ParticleData
{
    float4 data1; // Position : xyz; Age : w
    float4 data2; // Velocity : xyz; Energy : w
	float4 data3; // OldPosition : xyz; LifeTime : w
	float4 data4; // Color
	float4 data5; // PARAM_SIZE : x; PARAM_MASS : y; PARAM_ANGLE : z; PARAM_TEXTURE_INDEX : w
	float4 data6; // PARAM_ROTATION_SPEED : x; SqrDist : y; Group : z
};

#define paPosition 			data1.xyz
#define paAge 				data1.w
#define paVelocity 			data2.xyz
#define paEnergy 			data2.w
#define paOldPosition 		data3.xyz
#define paLifeTime 			data3.w
#define paColor 			data4
#define paParamSize 		data5.x
#define paParamMass 		data5.y
#define paParamAngle 		data5.z
#define paParamTexIndex 	data5.w
#define paParamRotSpeed 	data6.x
#define paSqrDist 			data6.y
#define paGroup 			data6.z
#define paRandom 			data6.w

struct Emitter
{
	float4 data1; // zone, active, tank, flow, 
	float4 data2; // forceMin, forceMax, full, fraction
	float4 data3; // direction, angleMin
	float4 data4; // tDirection, angleMax
	float4 data5; // matrix 1, 2, 3, cosAngleMin
	float4 data6; // matrix 4, 5, 6, cosAngleMax
	float4 data7; // matrix 7, 8, 9, type
};

#define emZone 				data1.x
#define emActive 			data1.y
#define emTank 				data1.z
#define emFlow 				data1.w
#define emForceMin 			data2.x
#define emForceMax 			data2.y
#define emFull 				data2.z
#define emFraction 			data2.w
#define emDirection 		data3.xyz
#define emAngleMin 			data3.w
#define emTDirection 		data4.xyz
#define emAngleMax 			data4.w
#define emCosAngleMin 		data5.w
#define emCosAngleMax 		data6.w
#define emMatrix 			float3x3(data5.x, data5.y, data5.z, data6.x, data6.y, data6.z, data7.x, data7.y, data7.z)
#define emMatrixRow1(e)		float3(e.data5.x, e.data5.y, e.data5.z)
#define emMatrixRow2(e) 	float3(e.data6.x, e.data6.y, e.data6.z)
#define emMatrixRow3(e) 	float3(e.data7.x, e.data7.y, e.data7.z)
#define emType 				data7.w

struct Zone
{
	float4 data1; // position, radius
	float4 data2; // tPosition, type
	float4 data3; // normal
	float4 data4; // tNormal
};

#define zoPosition 			data1.xyz
#define zoRadius 			data1.w
#define zoTPosition 		data2.xyz
#define zoType 				data2.w
#define zoNormal 			data3.xyz
#define zoTNormal 			data4.xyz

struct Interpolator
{
	// rien de prévu pour le random interpolator
	float4 data1; // type
	float4 data2;
	float4 data3;
};

#define inType 				data1.x

struct Group
{
	float4 data1; // particleData, colorInterpolator, paramInterpolator 1 et 2
	float4 data2; // paramInterpolator 3, 4 et 5
	float4 data3; // emitters
	float4 data4; // modifiers
	float4 data5; // minLifeTime, maxLifeTime, radius, immortal
	float4 data6; // still
};

#define grParticleData 					data1.x
#define grColorInterpolator 			data1.y
#define grParamSizeInterpolator 		data1.z
#define grParamMassInterpolator 		data1.w
#define grParamAngleInterpolator 		data2.x
#define grParamTexIndexInterpolator 	data2.y
#define grParamRotSpeedInterpolator 	data2.z
#define grMinLifeTime 					data5.x
#define grMaxLifeTime 					data5.y
#define grRadius 						data5.z
#define grImmortal 						data5.w
#define grStill 						data6.x
#define grModifiers 					data4

struct Modifier
{
	float4 data1; // type, active, local, zone
	float4 data2; // value, bouncingRatio
	float4 data3; // tValue, friction
};

#define moType					data1.x
#define moActive				data1.y
#define moLocal					data1.z
#define moZone					data1.w
#define moValue					data2.xyz
#define moBouncingRatio			data2.w
#define moTValue				data3.xyz
#define moFriction				data3.w



StructuredBuffer<ParticleData> oldParticle : register(t0);
StructuredBuffer<float4> randoms : register(t1);
StructuredBuffer<Group> groups : register(t2);
StructuredBuffer<Emitter> emitters : register(t3);
StructuredBuffer<Zone> zones : register(t4);
StructuredBuffer<Interpolator> interpolators : register(t5);
StructuredBuffer<Modifier> modifiers : register(t6);

RWStructuredBuffer<ParticleData> newParticle : register(u0);

/*
float randomS(uint GI)
{
	// optimized standard minimal
	int tmp0 = 16807L * (sharedRand[GI] & 0xFFFFL);
	int tmp1 = 16807L * (sharedRand[GI] >> 16);
	int tmp2 = (tmp0 >> 16) + tmp1;
	tmp0 = ((tmp0 & 0xFFFF)|((tmp2 & 0x7FFF) << 16)) + (tmp2 >> 15);

	// correction of the error
	if ((tmp0 & 0x80000000L) != 0)
		tmp0 = (tmp0 + 1) & 0x7FFFFFFFL;

	sharedRand[GI] = tmp0;

	// find a random number in the interval
	return (float(sharedRand[GI]) - 1.0f) / 2147483646.0f;
}
*/

float randomP(int dec)
{
	// optimized standard minimal
	uint randomSeed = asuint(newParticle[0].paRandom) + dec;
	int tmp0 = 16807L * (randomSeed & 0xFFFFL);
	int tmp1 = 16807L * (randomSeed >> 16);
	int tmp2 = (tmp0 >> 16) + tmp1;
	tmp0 = ((tmp0 & 0xFFFF)|((tmp2 & 0x7FFF) << 16)) + (tmp2 >> 15);

	// correction of the error
	if ((tmp0 & 0x80000000L) != 0)
		tmp0 = (tmp0 + 1) & 0x7FFFFFFFL;

	newParticle[0].paRandom = asfloat(tmp0);

	// find a random number in the interval
	return (float(tmp0) - 1.0f) / 2147483646.0f;
}

/*
float randomS(uint GI, float minVal, float maxVal)
{
/*
	// optimized standard minimal
	int tmp0 = 16807L * (sharedRand[GI] & 0xFFFFL);
	int tmp1 = 16807L * (sharedRand[GI] >> 16);
	int tmp2 = (tmp0 >> 16) + tmp1;
	tmp0 = ((tmp0 & 0xFFFF)|((tmp2 & 0x7FFF) << 16)) + (tmp2 >> 15);

	// correction of the error
	if ((tmp0 & 0x80000000L) != 0)
		tmp0 = (tmp0 + 1) & 0x7FFFFFFFL;

	sharedRand[GI] = tmp0;

	// find a random number in the interval
	return float(minVal + ((sharedRand[GI] - 1) / 2147483646.0f) * (maxVal - minVal));
*
	return float(minVal + randomS(GI) * (maxVal - minVal));
}
*/

float randomP(int dec, float minVal, float maxVal)
{
	return float(minVal + randomP(dec) * (maxVal - minVal));
}

/*
float3 randomVecS(uint GI)
{
	float3 result;
	int i = 0;
	[unroll]do
	{
		result = float3(randomS(GI, -1.0f, 1.0f), randomS(GI, -1.0f, 1.0f), randomS(GI, -1.0f, 1.0f));
	}
	while( length(result) > 1.0f && i++ < 2 );
	return normalize(result);
}
*/

float3 randomVecP(int dec)
{
	float3 result;
	int i = 0;
	[unroll]do
	{
		result = float3(randomP(dec, -1.0f, 1.0f), randomP(dec, -1.0f, 1.0f), randomP(dec, -1.0f, 1.0f));
	}
	while( length(result) > 1.0f && i++ < 2 );
	return normalize(result);
}

float random(uint seed)
{
	float4 id = randoms[seed&0x1ff];
	return id[g_param.w];
}

float random(uint seed, float minVal, float maxVal)
{
	float4 id = randoms[seed&0x1ff];
	return ( ( maxVal-minVal )*id[g_param.w] + minVal );
}

float3 randomVec(uint seed)
{
	return float3(2.0f*random(seed)-1.0f, 2.0f*random(seed+1)-1.0f, 2.0f*random(seed+2)-1.0f);
}

// Emitter
float3 generateVelocity(int index, float speed)
{
	Emitter emitter = emitters[index];
	float3 result;
	switch( emitter.emType )
	{
	case RandomEmitter :
		{
			result = randomVecP(index);
			result *= speed;
			return result;
		}
	case SphericEmitter :
		{
			float a = random(g_paramf.y, emitter.emCosAngleMax, emitter.emCosAngleMin);
			float theta = acos(a);
			float phi = random((g_paramf.y*g_paramf.x), 0.0f, (2.0f * PI));
			float sinTheta = sin(theta);
			float3 xyz = float3(sinTheta * cos(phi), sinTheta * sin(phi), cos(theta));
			result.x = speed * dot(xyz, emMatrixRow1(emitter));
			result.y = speed * dot(xyz, emMatrixRow2(emitter));
			result.z = speed * dot(xyz, emMatrixRow3(emitter));
			return result;
		}
	case StaticEmitter :
		{
			return float3(0.0f, 0.0f, 0.0f);
		}
	case StraightEmitter :
		{
			return (emitter.emTDirection * speed);
		}
	}
	return float3(0.0f, 0.0f, 0.0f);
}

//#define zone zones[index]
float3 generatePosition(int index, bool full, float radius = 0.0f)
{
	Zone zone = zones[index];
	switch( zone.zoType )
	{
	case PointZone :
		return zone.zoTPosition;
	case SphereZone :
		{
			float relRadius = zone.zoRadius - radius;
			float3 result;

			do
			{
				result = float3(g_paramf.y, g_paramf.y * g_paramf.x, g_paramf.y * g_paramf.y);
				result *= relRadius;
			}
			while( dot(result, result) > (relRadius*relRadius) );

			if( !full ) result *= zone.zoRadius / length(result);

			result += zone.zoTPosition;

			return result;
		}
	case PlaneZone :
		return zone.zoTPosition;
	}
	return float3(0.0f, 0.0f, 0.0f);
}

float3 computeNormal(int index, const float3 v)
{
	Zone zone = zones[index];

	switch( zone.zoType )
	{
	case PointZone :
	case SphereZone :
		// même chose pour Point et Sphere
		float3 normal = (v - zone.zoTPosition);
		return normalize(normal);
	case PlaneZone :
		return zone.zoTNormal;
	}
	return float3(0.0f, 0.0f, 0.0f);
}

//#define zone zones[index]

bool intersects(int index, float3 v0, float3 v1, float radius)
{
	Zone zone = zones[index];

	switch( zone.zoType )
	{
	case PointZone :
		return false;
	case SphereZone :
		{
			float r2 = zone.zoRadius * zone.zoRadius + radius * radius;
			float s2 = 2.0f * zone.zoRadius * radius;

			float dist0 = dot(zone.zoTPosition - v0, zone.zoTPosition - v0);
			float dist1 = dot(zone.zoTPosition - v1, zone.zoTPosition - v1);

			if (dist0 > r2 + s2) // the start sphere is completely out of the sphere
			{
				if (dist1 > r2 + s2) // the end sphere is completely out of the sphere
					return false;

				return true;
			}

			if (dist0 < r2 - s2) // the start sphere is completely in the sphere
			{
				if (dist1 < r2 - s2) // the end sphere is completely in the sphere
					return false;

				return true;
			}

			// the start sphere is already intersecting the sphere, the intersection is ignored
			return false;
		}
	case PlaneZone :
		{
			float dist0 = dot(zone.zoTNormal, v0 - zone.zoTPosition);
			float dist1 = dot(zone.zoTNormal, v1 - zone.zoTPosition);

			if ((dist0 <= 0.0f) == (dist1 <= 0.0f)) // both points are on the same side
				return false;

			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------
// interpolator
//--------------------------------------------------------------------------------------------------
void interpolateParam(inout float result, float start, float end, float ratio)
{
	result = start * (1.0f - ratio) + end * ratio;
}

void interpolateParam(inout float4 result, float4 start, float4 end, float ratio)
{
	result = start * (1.0f - ratio) + end * ratio;
}

void interpolate(inout float data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
{
	//Interpolator interpolator = interpolators[interpolatorIndex];
	switch( interpolator.inType )
	{
	case DefaultInitializer :
		// rien
		break;
	case RandomInitializer :
		// rien
		break;
	case RandomInterpolator :
		// rien ... pour l'instant
		break;
	case SimpleInterpolator :
		interpolateParam(data, interpolator.data3.x, interpolator.data2.x, particle.paEnergy);
		break;
	}
}

void init(inout float data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
{
	switch( interpolator.inType )
	{
	case DefaultInitializer :
		data = interpolator.data2.x;
		break;
	case RandomInitializer :
		// rien
		break;
	case RandomInterpolator :
		// rien ... pour l'instant
		break;
	case SimpleInterpolator :
		data = interpolator.data2.x;
		break;
	}
}

void interpolate(inout float4 data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
{
	switch( interpolator.inType )
	{
	case DefaultInitializer :
		// rien
		break;
	case RandomInitializer :
		// rien
		break;
	case RandomInterpolator :
		// rien ... pour l'instant
		break;
	case SimpleInterpolator :
		interpolateParam(data, interpolator.data3, interpolator.data2, particle.paEnergy);
		break;
	}
}

void init(inout float4 data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
{
	switch( interpolator.inType )
	{
	case DefaultInitializer :
		data = interpolator.data2;
		break;
	case RandomInitializer :
		// rien
		break;
	case RandomInterpolator :
		// rien ... pour l'instant
		break;
	case SimpleInterpolator :
		data = interpolator.data2;
		break;
	}
}
//--------------------------------------------------------------------------------------------------
// end interpolator
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// modifier
//--------------------------------------------------------------------------------------------------
void modify(inout ParticleData particle, Modifier modifier)
{
	switch( modifier.moType )
	{
	case FrictionModifier :
		{
			const float discreteFriction = modifier.moValue.x * elapsedTime;
			if( particle.paParamMass > 0.0f )
			{
				particle.paVelocity *= 1.0f - min(1.0f, discreteFriction/particle.paParamMass);
			}
			else
			{
				const float ratio = 1.0f - min(1.0f, discreteFriction);
				particle.paVelocity *= ratio;
			}
		}
		break;
	case GravityModifier :
		{
			particle.paVelocity += modifier.moValue * elapsedTime;
		}
		break;
	case ObstacleModifier :
		{
			if( modifier.moZone != -1.0f )
			{
				//bool hr = intersects(modifier.moZone, particle.paOldPosition, particle.paPosition, particle.paParamSize * groups[particle.paGroup].grRadius);
				if( intersects(modifier.moZone, particle.paOldPosition, particle.paPosition, particle.paParamSize * groups[particle.paGroup].grRadius) )
				{
					particle.paPosition = particle.paOldPosition;

					float3 normal = computeNormal(modifier.moZone, particle.paPosition);

					float dist = dot(particle.paVelocity, normal);

					normal *= dist;
					particle.paVelocity -= normal;			// tangent component
					particle.paVelocity *= modifier.moFriction;
					normal *= modifier.moBouncingRatio;	// normal component
					particle.paVelocity -= normal;

					//particle.paColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
				}
			}
		}
		break;
	case RotatorModifier :
		{
			//if( particle.paParamRotSpeed > 0.0f )
			particle.paParamAngle += particle.paParamRotSpeed * elapsedTime;
		}
		break;
	}
}
//--------------------------------------------------------------------------------------------------
// end modifier
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// simple
//--------------------------------------------------------------------------------------------------
void initSimpleParticle(inout ParticleData particle, int particleIndex)
{
	Group group = groups[particle.paGroup];

	particle.paAge = 0.0f;
	particle.paEnergy = 1.0;
	particle.paLifeTime = random(particleIndex*g_paramf.y, group.grMinLifeTime, group.grMaxLifeTime);

	if( group.grColorInterpolator != -1.0f )
		init(particle.paColor, interpolators[group.grColorInterpolator], particle, 0);
	else
		particle.paColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if( group.grParamSizeInterpolator != -1.0f ) init(particle.paParamSize, interpolators[group.grParamSizeInterpolator], particle, 0);
	if( group.grParamMassInterpolator != -1.0f ) init(particle.paParamMass, interpolators[group.grParamMassInterpolator], particle, 0);
	if( group.grParamAngleInterpolator != -1.0f ) init(particle.paParamAngle, interpolators[group.grParamAngleInterpolator], particle, 0);
	if( group.grParamTexIndexInterpolator != -1.0f ) init(particle.paParamTexIndex, interpolators[group.grParamTexIndexInterpolator], particle, 0);
	if( group.grParamRotSpeedInterpolator != -1.0f ) init(particle.paParamRotSpeed, interpolators[group.grParamRotSpeedInterpolator], particle, 0);

/*
	particle.paParamSize = random(particleIndex*g_paramf.y*g_paramf.y, 1.0f, 5.0f);//1.0f;
	particle.paParamMass = 1.0f;
	particle.paParamAngle = 0.0f;
	particle.paParamTexIndex = 0.0f;
	particle.paParamRotSpeed = 0.0f;
*/
	
	particle.paPosition = float3(0.0f, 0.0f, 0.0f);
	particle.paOldPosition = particle.paPosition;

	// place où devrait se situer l'utilisation de l'émetteur
	particle.paVelocity = 20.0f * random(particleIndex*g_paramf.y) * normalize(float3(2.0f*random(particleIndex)-1.0f, 2.0f*random(particleIndex+10)-1.0f, 2.0f*random(particleIndex-10)-1.0f));
}

#define group groups[particle.paGroup]

void initSimpleParticleS(inout ParticleData particle, int particleIndex, uint GI)
{
	//Group group = groups[particle.paGroup];

	particle.paAge = 0.0f;
	particle.paEnergy = 1.0;
	//particle.paLifeTime = random(particleIndex*g_paramf.y, group.grMinLifeTime, group.grMaxLifeTime);
	particle.paLifeTime = randomP(particleIndex, group.grMinLifeTime, group.grMaxLifeTime);

	if( group.grColorInterpolator != -1.0f )
		init(particle.paColor, interpolators[group.grColorInterpolator], particle, 0);
	else
		particle.paColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if( group.grParamSizeInterpolator != -1.0f ) 		init(particle.paParamSize, interpolators[group.grParamSizeInterpolator], particle, 0);
	if( group.grParamMassInterpolator != -1.0f ) 		init(particle.paParamMass, interpolators[group.grParamMassInterpolator], particle, 0);
	if( group.grParamAngleInterpolator != -1.0f ) 		init(particle.paParamAngle, interpolators[group.grParamAngleInterpolator], particle, 0);
	if( group.grParamTexIndexInterpolator != -1.0f ) 	init(particle.paParamTexIndex, interpolators[group.grParamTexIndexInterpolator], particle, 0);
	if( group.grParamRotSpeedInterpolator != -1.0f ) 	init(particle.paParamRotSpeed, interpolators[group.grParamRotSpeedInterpolator], particle, 0);
	
	particle.paPosition = float3(0.0f, 0.0f, 0.0f);
	particle.paOldPosition = particle.paPosition;

	//particle.paVelocity = 20.0f * random(particleIndex*g_paramf.y) * normalize(float3(randomS(GI, -1.0f, 1.0f), randomS(GI, -1.0f, 1.0f), randomS(GI, -1.0f, 1.0f)));
	particle.paVelocity = 20.0f * randomP(particleIndex) * randomVecP(particleIndex);
	//particle.paVelocity = 20.0f * random(particleIndex*g_paramf.y) * normalize(float3(2.0f*randomS(GI)-1.0f, 2.0f*randomS(GI)-1.0f, 2.0f*randomS(GI)-1.0f));
}

//#define particle newParticle[DTid.x]

[numthreads(blocksize, 1, 1)]
void CSSimpleMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
	// init random
	//sharedRand[GI] = float(GI+1+g_paramf.x) / float(blocksize) * 0xFFFFFFFF;
	//sharedRand[GI] = 0xFFFFFF * g_paramf.z * (GI + 1);

    // Each thread of the CS updates one of the particles
    ParticleData particle = oldParticle[DTid.x];

	particle.paAge += g_paramf.x;

	//Group group = groups[particle.paGroup];

	if( group.grImmortal == 0.0f ) particle.paEnergy = 1.0f - particle.paAge / particle.paLifeTime;
	
	if( group.grStill == 0.0f )
	{
		particle.paOldPosition = particle.paPosition;
		particle.paPosition += particle.paVelocity * g_paramf.x;
	}

	// Interpolates the parameters
	if( group.grColorInterpolator != -1.0f ) 			interpolate(particle.paColor, interpolators[group.grColorInterpolator], particle, 0);
	if( group.grParamSizeInterpolator != -1.0f ) 		interpolate(particle.paParamSize, interpolators[group.grParamSizeInterpolator], particle, 0);
	if( group.grParamMassInterpolator != -1.0f ) 		interpolate(particle.paParamMass, interpolators[group.grParamMassInterpolator], particle, 0);
	if( group.grParamAngleInterpolator != -1.0f ) 		interpolate(particle.paParamAngle, interpolators[groups[particle.paGroup].grParamAngleInterpolator], particle, 0);
	if( group.grParamTexIndexInterpolator != -1.0f ) 	interpolate(particle.paParamTexIndex, interpolators[group.grParamTexIndexInterpolator], particle, 0);
	if( group.grParamRotSpeedInterpolator != -1.0f ) 	interpolate(particle.paParamRotSpeed, interpolators[group.grParamRotSpeedInterpolator], particle, 0);

	// Modifies the particles with specific active modifiers behavior
	[unroll]for(int i = 0; i < 4; i++)
	{
		if( group.grModifiers[i] != -1.0f ) modify(particle, modifiers[group.grModifiers[i]]);
		//else break;
	}

	if( particle.paEnergy <= 0.0f )
	{
		//initSimpleParticle(particle, DTid.x);
		initSimpleParticleS(particle, DTid.x, GI);
	}

    // write particle updated
    newParticle[DTid.x] = particle;
}
