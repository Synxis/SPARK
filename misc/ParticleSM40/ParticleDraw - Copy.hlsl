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

struct VSParticleIn
{
//    float4  color   : COLOR;
    uint    id      : SV_VERTEXID;
};

struct VSParticleDrawOut
{
    float4 pos			: POSITION; // position and size
    float4 color		: COLOR;
};

struct GSParticleDrawOut
{
    float2 tex			: TEXCOORD0;
    float4 color		: COLOR;
    float4 pos			: SV_POSITION;
};

struct PSParticleDrawIn
{
    float2 tex			: TEXCOORD0;
    float4 color		: COLOR;
};

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

struct ExData
{
	float4 exData00;
};

Texture2D		            g_txDiffuse;
StructuredBuffer<ParticleData>   g_bufParticle;

//StructuredBuffer<ExData>   g_bufExData;


SamplerState g_samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

cbuffer cb0
{
    row_major float4x4 g_mWorldViewProj;
    row_major float4x4 g_mInvView;    
};

cbuffer cb1
{
    static float g_fParticleRad = 1.0f;   
};

cbuffer cbImmutable
{
    static float3 g_positions[4] =
    {
        float3( -1, 1, 0 ),
        float3( 1, 1, 0 ),
        float3( -1, -1, 0 ),
        float3( 1, -1, 0 ),
    };
    
    static float2 g_texcoords[4] = 
    { 
        float2(0,0), 
        float2(1,0),
        float2(0,1),
        float2(1,1),
    };
};

float4 DWtoF4( uint dw )
{
    //float f = 1.0f / 255.0f;
    float4 result;
    
    result.a = float((dw & 0xff000000) >> 24);
    result.r = float((dw & 0x00ff0000) >> 16);
    result.g = float((dw & 0x0000ff00) >>  8);
    result.b = float((dw & 0x000000ff));
    
    result *= 0.0039215686274509803921568627451f;
    
    return result;
}

//
// Vertex shader for drawing the point-sprite particles
//
VSParticleDrawOut VSParticleDraw(VSParticleIn input)
{
    VSParticleDrawOut output;
    
    output.pos.xyz = g_bufParticle[input.id].paPosition;
	output.pos.w = g_bufParticle[input.id].paParamSize;
    
    output.color = g_bufParticle[input.id].paColor;
    
    return output;
}

//
// GS for rendering point sprite particles.  Takes a point and turns it into 2 tris.
//
[maxvertexcount(4)]
void GSParticleDraw(point VSParticleDrawOut input[1], inout TriangleStream<GSParticleDrawOut> SpriteStream)
{
    GSParticleDrawOut output;
    
    //
    // Emit two new triangles
    //
    for(int i=0; i<4; i++)
    {
        float3 position = g_positions[i] * input[0].pos.w;//g_fParticleRad;
        position = mul( position, (float3x3)g_mInvView ) + input[0].pos.xyz;
        output.pos = mul( float4(position,1.0), g_mWorldViewProj ); 

        output.color = input[0].color;        
        output.tex = g_texcoords[i];
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();
}

//
// PS for drawing particles
//
float4 PSParticleDraw(PSParticleDrawIn input) : SV_Target
{   
    return g_txDiffuse.Sample( g_samLinear, input.tex ) * input.color;
}
