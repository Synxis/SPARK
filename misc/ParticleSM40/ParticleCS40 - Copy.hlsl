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

#define blocksize 256
groupshared float4 sharedPos[blocksize];

#define PI 3.14159265f

cbuffer cbCS : register( b0 )
{
    uint4   g_param;    // pcbCS->param[0] = MAX_PARTICLES;
                        // pcbCS->param[1] = dimx;              
    float4  g_paramf;   // pcbCS->paramf[0] = 0.1f; elapsedTime
                        // pcbCS->paramf[1] = 1; timeFromStart
};

#define RandomEmitter 1
#define SphericEmitter 2
#define StaticEmitter 3
#define StraightEmitter 4

#define PointZone 51
#define SphereZone 52

#define DefaultInitializer 101
#define RandomInitializer 102
#define RandomInterpolator 103
#define SimpleInterpolator 104

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
};

#define zoPosition 			data1.xyz
#define zoRadius 			data1.w
#define zoTPosition 		data2.xyz
#define zoType 				data2.w

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

#define grParticleData 			data1.x
#define grColorInterpolator 	data1.y
#define grParamSizeInterpolator 	data1.z
#define grParamMassInterpolator 	data1.w
#define grParamAngleInterpolator 	data2.x
#define grParamTexIndexInterpolator 	data2.y
#define grParamRotSpeedInterpolator 	data2.z
#define grMinLifeTime 			data5.x
#define grMaxLifeTime 			data5.y
#define grRadius 				data5.z
#define grImmortal 				data5.w
#define grStill 				data6.x

StructuredBuffer<ParticleData> oldParticle : register(t0);
StructuredBuffer<float4> randoms : register(t1);
StructuredBuffer<Group> groups : register(t2);
StructuredBuffer<Emitter> emitters : register(t3);
StructuredBuffer<Zone> zones : register(t4);
StructuredBuffer<Interpolator> interpolators : register(t5);


RWStructuredBuffer<ParticleData> newParticle : register(u0);

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
			float sqrNorm;
			do
			{
				result = float3(g_paramf.y, g_paramf.y * g_paramf.x, g_paramf.y * g_paramf.y);
				sqrNorm = length(result);
			}
			while( (sqrNorm > 1.0f) || (sqrNorm == 0.0f) );
			result *= speed / sqrt(sqrNorm);
			return result;
		}
		break;
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
		break;
	case StaticEmitter :
		{
			return float3(0.0f, 0.0f, 0.0f);
		}
		break;
	case StraightEmitter :
		{
			return (emitter.emTDirection * speed);
		}
		break;
	}
	return float3(0.0f, 0.0f, 0.0f);
}

float3 generatePosition(int index, bool full, float radius = 0.0f)
{
	Zone zone = zones[index];
	switch( zone.zoType )
	{
	case PointZone :
		return zone.zoTPosition;
		break;
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
		break;
	}
	return float3(0.0f, 0.0f, 0.0f);
}

//--------------------------------------------------------------------------------------------------
// interpolator
//--------------------------------------------------------------------------------------------------
void interpolateParam1(inout float result, float start, float end, float ratio)
{
	result = start * (1.0f - ratio) + end * ratio;
}

void interpolateParam4(inout float4 result, float4 start, float4 end, float ratio)
{
	result = start * (1.0f - ratio) + end * ratio;
}

void interpolate1(inout float data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
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
		interpolateParam1(data, interpolator.data3.x, interpolator.data2.x, particle.paEnergy);
		break;
	}
}

void init1(inout float data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
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

void interpolate4(inout float4 data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
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
		interpolateParam4(data, interpolator.data3, interpolator.data2, particle.paEnergy);
		break;
	}
}

void init4(inout float4 data, Interpolator interpolator, ParticleData particle, int dataSetIndex)
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

/*
void initParticle(inout ParticleData particle, int particleIndex, int groupIndex, int emitterIndex, int nbManualBorn)
{
	Group group = groups[groupIndex];

	particle.paAge = 0.0f;
	particle.paEnergy = 1.0;
	particle.paLifeTime = (group.grMaxLifeTime-group.grMinLifeTime) * random(g_paramf.y, group.grMinLifeTime, group.grMaxLifeTime);
	particle.paColor = float4(1.0f, 0.5f, 0.5f, 1.0f);
	particle.paParamSize = 1.0f;
	particle.paParamMass = 1.0f;
	particle.paParamAngle = 0.0f;
	particle.paParamTexIndex = 0.0f;
	particle.paParamRotSpeed = 0.0f;

	Emitter emitter = emitters[emitterIndex];
	
	particle.paPosition = generatePosition(emitter.emZone, false);
	particle.paVelocity = generateVelocity(emitterIndex, random(g_paramf.y, emitter.emForceMin, emitter.emForceMax));
}

[numthreads(blocksize, 1, 1)]
void CSMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
    // Each thread of the CS updates one of the particles
    ParticleData particle = oldParticle[DTid.x];

	Group group = groups[particle.paGroup];


    // Prepares the additionnal data
	//prepareAdditionnalData();

	int nbAutoBorn = 0;
	int nbManualBorn = 0;

	// Checks the number of born particles
	bool hasAliveEmitters = false;

	int emitterIndex = 0;
	int nbBorn = nbAutoBorn + nbManualBorn;

	// Updates the age of the particles function of the delta time
	particle.paAge += g_paramf.x;


	// Computes the energy of the particles (if they are not immortal)
	if( group.grImmortal == 0 )
		particle.paEnergy = 1.0f - particle.paAge / particle.paLifeTime;

	// Updates the position of particles function of their velocity
	if( group.grStill == 0 )
	{
		particle.paOldPosition = particle.paPosition;
		particle.paPosition += particle.paVelocity * g_paramf.x;
	}

	// Interpolates the parameters
	if( group.grColorInterpolator != -1 )
		particle.paColor = interpolate4(group.grColorInterpolator, particle.paGroup);

	if( group.grParamSizeInterpolator != -1 ) particle.paParamSize = 1.0f;

	if( group.grParamMassInterpolator != -1 ) particle.paParamMass = 1.0f;

	if( group.grParamAngleInterpolator != -1 ) particle.paParamAngle = 0.0f;

	if( group.grParamTexIndexInterpolator != -1 ) particle.paParamTexIndex = 0.0f;

	if( group.grParamRotSpeedInterpolator != -1 ) particle.paParamRotSpeed = 0.0f;

	// Modifies the particles with specific active modifiers behavior
	//for (std::vector<ModifierDef>::const_iterator it = activeModifiers.begin(); it != activeModifiers.end(); ++it)
	//	it->obj->modify(*this,it->dataSet,deltaTime);

	// Updates the renderer data
	//if (renderer.obj != NULL)
	//	renderer.obj->update(*this,renderer.dataSet);

	// Checks dead particles and reinits or swaps
	if( particle.paEnergy <= 0.0f )
	{
		// Death action
		// pas implanté
		{
			initParticle(particle, DTid.x, particle.paGroup, emitterIndex, nbManualBorn);
		}
/*
		if( nbBorn > 0 ) // If particles are to be born, reinits a dead one with a new one
		{
			initParticle(i,emitterIndex,nbManualBorn);
			--nbBorn;
		}
		else // Else inactivate the dead one
		{
			swapParticles(i,particleData.nbParticles - 1);
			--particleData.nbParticles;
		}
*
	}

	// Emits new particles if some left
	//for (size_t i = std::min(nbBorn,particleData.maxParticles - particleData.nbParticles); i > 0; --i)
	//	initParticle(particleData.nbParticles++,emitterIndex,nbManualBorn);

	// Computes the distance of particles from the camera
	/*
	if (distanceComputationEnabled)
	{
		for (size_t i = 0; i < particleData.nbParticles; ++i)
			particleData.sqrDists[i] = getSqrDist(particleData.positions[i],System::getCameraPosition());
	}
	*




    // write particle updated
    newParticle[DTid.x] = particle;
}
*/

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
		init4(particle.paColor, interpolators[group.grColorInterpolator], particle, 0);
	else
		particle.paColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if( group.grParamSizeInterpolator != -1.0f ) init1(particle.paParamSize, interpolators[group.grParamSizeInterpolator], particle, 0);
	if( group.grParamMassInterpolator != -1.0f ) init1(particle.paParamMass, interpolators[group.grParamMassInterpolator], particle, 0);
	if( group.grParamAngleInterpolator != -1.0f ) init1(particle.paParamAngle, interpolators[group.grParamAngleInterpolator], particle, 0);
	if( group.grParamTexIndexInterpolator != -1.0f ) init1(particle.paParamTexIndex, interpolators[group.grParamTexIndexInterpolator], particle, 0);
	if( group.grParamRotSpeedInterpolator != -1.0f ) init1(particle.paParamRotSpeed, interpolators[group.grParamRotSpeedInterpolator], particle, 0);

/*
	particle.paParamSize = random(particleIndex*g_paramf.y*g_paramf.y, 1.0f, 5.0f);//1.0f;
	particle.paParamMass = 1.0f;
	particle.paParamAngle = 0.0f;
	particle.paParamTexIndex = 0.0f;
	particle.paParamRotSpeed = 0.0f;
*/
	
	particle.paPosition = float3(0.0f, 0.0f, 0.0f);
	particle.paOldPosition = particle.paPosition;

	particle.paVelocity = 20.0f * random(particleIndex*g_paramf.y) * normalize(float4(2.0f*random(particleIndex)-1.0f, 2.0f*random(particleIndex+10)-1.0f, 2.0f*random(particleIndex-10)-1.0f, 0.0f));
}

[numthreads(blocksize, 1, 1)]
void CSSimpleMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
    // Each thread of the CS updates one of the particles
    ParticleData particle = oldParticle[DTid.x];

	particle.paAge += g_paramf.x;

	Group group = groups[particle.paGroup];

	if( group.grImmortal == 0.0f ) particle.paEnergy = 1.0f - particle.paAge / particle.paLifeTime;
	
	if( group.grStill == 0.0f )
	{
		particle.paOldPosition = particle.paPosition;
		particle.paPosition += particle.paVelocity * g_paramf.x;
	}

	// Interpolates the parameters
	if( group.grColorInterpolator != -1.0f ) interpolate4(particle.paColor, interpolators[group.grColorInterpolator], particle, 0);
	if( group.grParamSizeInterpolator != -1.0f ) interpolate1(particle.paParamSize, interpolators[group.grParamSizeInterpolator], particle, 0);
	if( group.grParamMassInterpolator != -1.0f ) interpolate1(particle.paParamMass, interpolators[group.grParamMassInterpolator], particle, 0);
	if( group.grParamAngleInterpolator != -1.0f ) interpolate1(particle.paParamAngle, interpolators[group.grParamAngleInterpolator], particle, 0);
	if( group.grParamTexIndexInterpolator != -1.0f ) interpolate1(particle.paParamTexIndex, interpolators[group.grParamTexIndexInterpolator], particle, 0);
	if( group.grParamRotSpeedInterpolator != -1.0f ) interpolate1(particle.paParamRotSpeed, interpolators[group.grParamRotSpeedInterpolator], particle, 0);

	if( particle.paEnergy <= 0.0f )
	{
		initSimpleParticle(particle, DTid.x);
	}

    // write particle updated
    newParticle[DTid.x] = particle;
}
