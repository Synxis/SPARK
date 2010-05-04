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

#include "DXUT.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "DXUTcamera.h"
#include "DXUTsettingsdlg.h"
#include <d3dx11.h>
#include <commdlg.h>
#include "resource.h"
#include "WaitDlg.h"

#include <vector>
using namespace std;

#include "Struct.h"

//#define EXDATA

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager          g_DialogResourceManager;    // manager for shared resources of dialogs
CModelViewerCamera                  g_Camera;                   // A model viewing camera
CD3DSettingsDlg                     g_D3DSettingsDlg;           // Device settings dialog
CDXUTDialog                         g_HUD;                      // dialog for standard controls
CDXUTDialog                         g_SampleUI;                 // dialog for sample specific controls
CDXUTTextHelper*                    g_pTxtHelper = NULL;

ID3D11VertexShader*                 g_pRenderParticlesVS = NULL;
ID3D11GeometryShader*               g_pRenderParticlesGS = NULL;
ID3D11PixelShader*                  g_pRenderParticlesPS = NULL;
ID3D11SamplerState*                 g_pSampleStateLinear = NULL;
ID3D11BlendState*                   g_pBlendingStateParticle = NULL;
ID3D11DepthStencilState*            g_pDepthStencilState = NULL;

ID3D11ComputeShader*                g_pCalcCS = NULL;
ID3D11Buffer*                       g_pcbCS = NULL;

ID3D11Buffer*                       g_pParticle0 = NULL;
ID3D11Buffer*                       g_pParticle1 = NULL;
ID3D11ShaderResourceView*           g_pParticleRV0 = NULL;
ID3D11ShaderResourceView*           g_pParticleRV1 = NULL;
ID3D11UnorderedAccessView*          g_pParticleUAV0 = NULL;
ID3D11UnorderedAccessView*          g_pParticleUAV1 = NULL;
ID3D11Buffer*                       g_pParticleBuffer = NULL;
ID3D11InputLayout*                  g_pParticleVertexLayout = NULL;

ID3D11Buffer*						g_pRandom = NULL;
ID3D11ShaderResourceView*           g_pRandomRV = NULL;

ID3D11Buffer*						g_pGroups = NULL;
ID3D11ShaderResourceView*           g_pGroupsRV = NULL;
ID3D11Buffer*						g_pEmitters = NULL;
ID3D11ShaderResourceView*           g_pEmittersRV = NULL;
ID3D11Buffer*						g_pZones = NULL;
ID3D11ShaderResourceView*           g_pZonesRV = NULL;
ID3D11Buffer*						g_pInterpolators = NULL;
ID3D11ShaderResourceView*           g_pInterpolatorsRV = NULL;
ID3D11Buffer*						g_pModifiers = NULL;
ID3D11ShaderResourceView*           g_pModifiersRV = NULL;

ID3D11Device*						g_pd3dDevice = NULL;
ID3D11DeviceContext*				g_pDeviceContext = NULL;


#ifdef EXDATA
//--------------------------------------------------------------------------------------------------
// extended data
//--------------------------------------------------------------------------------------------------
ID3D11Buffer*						g_pParticleExData0 = NULL;
ID3D11ShaderResourceView*           g_pParticleExDataRV0 = NULL;
ID3D11UnorderedAccessView*          g_pParticleExDataUAV0 = NULL;
ID3D11Buffer*						g_pParticleExData1 = NULL;
ID3D11ShaderResourceView*           g_pParticleExDataRV1 = NULL;
ID3D11UnorderedAccessView*          g_pParticleExDataUAV1 = NULL;
//--------------------------------------------------------------------------------------------------
// end extended data
//--------------------------------------------------------------------------------------------------
#endif

ID3D11Buffer*                       g_pcbGS = NULL;

ID3D11ShaderResourceView*           g_pParticleTexRV = NULL;

float                               g_fSpread = 400.0f;

struct PARTICLE_VERTEX
{
    D3DXVECTOR4 Color;
};

#define MAX_PARTICLES	10000		// nombre de particules
#define BLOCKSIZE		128.0f		// taille des blocs sur le gpu

struct CB_GS
{
    D3DXMATRIX m_WorldViewProj;
    D3DXMATRIX m_InvView;
};

struct CB_CS
{
    UINT param[4];
    float paramf[4];
};


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_RESETPARTICLES      5

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                         void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                      DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext );

void InitApp();
void RenderText();

void InitEngine();
HRESULT CreateBuffers( ID3D11Device* pd3dDevice );

//--------------------------------------------------------------------------------------
// Find and compile the specified shader
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    // find the file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_AVOID_FLOW_CONTROL;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( str, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        SAFE_RELEASE( pErrorBlob );
        return hr;
    }
    SAFE_RELEASE( pErrorBlob );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	HRESULT hr = S_OK;

	srand( GetTickCount() );

    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackKeyboard( OnKeyboard );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

    InitApp();

    //DXUTInit( true, true, L"-forceref" ); // Force create a ref device so that feature level D3D_FEATURE_LEVEL_11_0 is guaranteed
    DXUTInit( true, true );                 // Use this line instead to try to create a hardware device

    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"ParticleCS40" );
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 1024, 768 );

	InitEngine();
	V_RETURN( CreateBuffers(g_pd3dDevice) );

    DXUTMainLoop();                      // Enter into the DXUT render loop

    return DXUTGetExitCode();
}

void InitEngine()
{
	Group group = createGroup(MAX_PARTICLES);
	group.MinLifeTime() = 2.0f;
	group.MaxLifeTime() = 10.0f;
	group.ColorInterpolator() = 1.0f;
	group.Modifiers().x = 0.0f;
	group.Modifiers().y = 2.0f;
	group.Modifiers().z = 1.0f;
	group.ParamAngleInterpolator() = 2.0f;
	group.ParamSizeInterpolator() = 3.0f;

	g_vecGroups.push_back(group);

	g_vecModifiers.push_back(createGravityModifier(D3DXVECTOR3(0.0f, -10.f, 0.0f)));
	g_vecModifiers.push_back(createObstacleModifier(2, 0.8f, 1.0f));
	g_vecModifiers.push_back(createObstacleModifier(3, 2.0f, 1.0f));

	g_vecZones.push_back(createPointZone(D3DXVECTOR3(0.0f, 0.0f, 0.0f)));
	g_vecZones.push_back(createSphereZone(D3DXVECTOR3(3.0f, -20.0f, 3.0f), 15.0f));
	g_vecZones.push_back(createPlaneZone(D3DXVECTOR3(0.0f, -100.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f)));
	g_vecZones.push_back(createPlaneZone(D3DXVECTOR3(20.0f, -100.0f, 0.0f), D3DXVECTOR3(-1.0f, 1.0f, 0.0f)));

	g_vecEmitters.push_back(createRandomEmitter(-1, false, 500, 100.0f, 10.0f, 20.0f));

	g_vecInterpolators.push_back(createDefaultInitializer(D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f)));
	g_vecInterpolators.push_back(createSimpleInterpolator(D3DXVECTOR4(1.0f, 0.1f, 0.1f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.0f)));
	g_vecInterpolators.push_back(createSimpleInterpolator(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR4(2.0f*D3DX_PI, 0.0f, 0.0f, 0.0f)));
	g_vecInterpolators.push_back(createSimpleInterpolator(D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR4(4.0f, 0.0f, 0.0f, 0.0f)));

	ParticleData particle;
	particle.Position() = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	particle.Age() = 0.0f;
	particle.Velocity() = D3DXVECTOR3(float(rand()) / float(RAND_MAX) - 0.5f, float(rand()) / float(RAND_MAX) - 0.5f, float(rand()) / float(RAND_MAX) - 0.5f);
	particle.Velocity() *= 40.0f;
	particle.Energy() = 1.0f;
	particle.OldPosition() = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	particle.LifeTime() = 5.0f;
	particle.Color() = D3DXVECTOR4(1.0f, 0.5f, 0.2f, 0.5);
	particle.ParamSize() = 1.0f;
	particle.ParamMass() = 1.0f;
	particle.ParamAngle() = 0.0f;
	particle.ParamTexIndex() = 0.0f;
	particle.ParamRotSpeed() = 0.1f;
	particle.SqrDist() = 0.0f;
	particle.Group() = 0.0f;
	unsigned int t = GetTickCount();
	particle.Random() = *((float*)&t);

	for( int i = 0; i < MAX_PARTICLES; ++i )
	{
		g_vecParticles.push_back(particle);
		particle.Velocity() = D3DXVECTOR3(float(rand()) / float(RAND_MAX) - 0.5f, float(rand()) / float(RAND_MAX) - 0.5f, float(rand()) / float(RAND_MAX) - 0.5f);
		D3DXVec3Normalize(&particle.Velocity(), &particle.Velocity());
		particle.Velocity() *= 40.0f;
	}
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 23 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += 26, 170, 23, VK_F3 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += 26, 170, 23, VK_F2 );
    g_HUD.AddButton( IDC_RESETPARTICLES, L"Reset particles", 0, iY += 26, 170, 22, VK_F2 );
    g_SampleUI.SetCallback( OnGUIEvent ); 
}

//--------------------------------------------------------------------------------------
HRESULT CreateBuffers( ID3D11Device* pd3dDevice )
{
    HRESULT hr = S_OK;

	// ParticleData
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = MAX_PARTICLES * sizeof(ParticleData);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(ParticleData);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &g_vecParticles[0];
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pParticle0 ) );
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pParticle1 ) );

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory( &DescRV, sizeof( DescRV ) );
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pParticle0, &DescRV, &g_pParticleRV0 ) );
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pParticle1, &DescRV, &g_pParticleRV1 ) );

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
		ZeroMemory( &DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC) );
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateUnorderedAccessView( g_pParticle0, &DescUAV, &g_pParticleUAV0 ) );
		V_RETURN( pd3dDevice->CreateUnorderedAccessView( g_pParticle1, &DescUAV, &g_pParticleUAV1 ) );
	}

	// Group
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = UINT(g_vecGroups.size() * sizeof(Group));
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Group);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &g_vecGroups[0];
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pGroups ) );

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory( &DescRV, sizeof( DescRV ) );
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pGroups, &DescRV, &g_pGroupsRV ) );
	}

	// Interpolator
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = UINT(g_vecInterpolators.size() * sizeof(Interpolator));
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Interpolator);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &g_vecInterpolators[0];
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pInterpolators ) );

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory( &DescRV, sizeof( DescRV ) );
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pInterpolators, &DescRV, &g_pInterpolatorsRV ) );
	}

	// Modifier
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = UINT(g_vecModifiers.size() * sizeof(Modifier));
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Modifier);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &g_vecModifiers[0];
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pModifiers ) );

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory( &DescRV, sizeof( DescRV ) );
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pModifiers, &DescRV, &g_pModifiersRV ) );
	}

	// Zone
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = UINT(g_vecZones.size() * sizeof(Zone));
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Zone);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &g_vecZones[0];
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pZones ) );

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory( &DescRV, sizeof( DescRV ) );
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pZones, &DescRV, &g_pZonesRV ) );
	}

    return hr;
}

#ifdef EXDATA
//--------------------------------------------------------------------------------------------------
// extended data
//--------------------------------------------------------------------------------------------------
HRESULT CreateParticleExDataBuffers( ID3D11Device* pd3dDevice )
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = MAX_PARTICLES * sizeof(D3DXVECTOR4);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(D3DXVECTOR4);
	desc.Usage = D3D11_USAGE_DEFAULT;

	// Initialize the data in the buffers
	D3DXVECTOR4* pData1 = new D3DXVECTOR4[ MAX_PARTICLES ];
	if( !pData1 )
		return E_OUTOFMEMORY;    

	srand( GetTickCount() );   


	ZeroMemory(pData1, MAX_PARTICLES*sizeof(D3DXVECTOR4));


	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pData1;
	V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pParticleExData0 ) );
	V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pParticleExData1 ) );
	SAFE_DELETE_ARRAY( pData1 );

	D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
	ZeroMemory( &DescRV, sizeof( DescRV ) );
	DescRV.Format = DXGI_FORMAT_UNKNOWN;
	DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	DescRV.Buffer.FirstElement = 0;
	DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
	V_RETURN( pd3dDevice->CreateShaderResourceView( g_pParticleExData0, &DescRV, &g_pParticleExDataRV0 ) );
	V_RETURN( pd3dDevice->CreateShaderResourceView( g_pParticleExData1, &DescRV, &g_pParticleExDataRV1 ) );

	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory( &DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC) );
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
	V_RETURN( pd3dDevice->CreateUnorderedAccessView( g_pParticleExData0, &DescUAV, &g_pParticleExDataUAV0 ) );
	V_RETURN( pd3dDevice->CreateUnorderedAccessView( g_pParticleExData1, &DescUAV, &g_pParticleExDataUAV1 ) );

	return hr;
}
//--------------------------------------------------------------------------------------------------
// end extended data
//--------------------------------------------------------------------------------------------------
#endif

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    assert( pDeviceSettings->ver == DXUT_D3D11_DEVICE );

    // Disable vsync
    pDeviceSettings->d3d11.SyncInterval = 0;
    g_D3DSettingsDlg.GetDialogControl()->GetComboBox( DXUTSETTINGSDLG_PRESENT_INTERVAL )->SetEnabled( false );    

    // For the first device created if it is a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
            pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
        {
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
        }
    }

    return true;
}

template <class T> void SWAP( T* &x, T* &y )
{
    T* temp = x;
    x = y;
    y = temp;
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.
// 
// fElapsedTime en seconde
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

	/*
	char buf[256];
	sprintf_s(buf, 256, "%e %f\n", fTime, fElapsedTime);
	OutputDebugStringA(buf);
	//*/

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();

    int dimx = int(ceil(MAX_PARTICLES/BLOCKSIZE));
    
	{
        pd3dImmediateContext->CSSetShader( g_pCalcCS, NULL, 0 );

        // For CS input            
        ID3D11ShaderResourceView* aRViews[ 7 ] = { g_pParticleRV0, NULL, g_pGroupsRV, NULL, g_pZonesRV, g_pInterpolatorsRV, g_pModifiersRV };
        pd3dImmediateContext->CSSetShaderResources( 0, 7, aRViews );

        // For CS output : !!! obligation d'utiliser un autre buffer pour la sortie !!!
        ID3D11UnorderedAccessView* aUAViews[ 1 ] = { g_pParticleUAV1 };
        pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, aUAViews, (UINT*)(&aUAViews) );

		#ifdef EXDATA
		//--------------------------------------------------------------------------------------------------
		// extended data
		//--------------------------------------------------------------------------------------------------
		// For CS input            
		ID3D11ShaderResourceView* aRViewsExData[ 1 ] = { g_pParticleExDataRV0 };
		pd3dImmediateContext->CSSetShaderResources( 1, 1, aRViewsExData );

		// For CS output
		ID3D11UnorderedAccessView* aUAViewsExData[ 1 ] = { g_pParticleExDataUAV1 };
		pd3dImmediateContext->CSSetUnorderedAccessViews( 1, 1, aUAViewsExData, (UINT*)(&aUAViewsExData) );
		//--------------------------------------------------------------------------------------------------
		// end extended data
		//--------------------------------------------------------------------------------------------------
		#endif

        // For CS constant buffer
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        V( pd3dImmediateContext->Map( g_pcbCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
        CB_CS* pcbCS = ( CB_CS* )MappedResource.pData;
		pcbCS->param[0] = MAX_PARTICLES;
		pcbCS->param[1] = dimx;
		pcbCS->param[2] = rand();
		pcbCS->param[3] = rand() % 4;
		pcbCS->paramf[0] = fElapsedTime;
		pcbCS->paramf[1] = static_cast<float>(fTime);
		pcbCS->paramf[2] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        pd3dImmediateContext->Unmap( g_pcbCS, 0 );
        ID3D11Buffer* ppCB[1] = { g_pcbCS };
        pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppCB );

        // Run the CS
        pd3dImmediateContext->Dispatch( dimx, 1, 1 );

        // Unbind resources for CS
        ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
        pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, ppUAViewNULL, (UINT*)(&aUAViews) );
        ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };
        pd3dImmediateContext->CSSetShaderResources( 0, 1, ppSRVNULL );

        //pd3dImmediateContext->CSSetShader( NULL, NULL, 0 );

		//*
        SWAP( g_pParticle0, g_pParticle1 );
        SWAP( g_pParticleRV0, g_pParticleRV1 );
        SWAP( g_pParticleUAV0, g_pParticleUAV1 );
		//*/

		#ifdef EXDATA
		//--------------------------------------------------------------------------------------------------
		// extended data
		//--------------------------------------------------------------------------------------------------
		// Unbind resources for CS
		ID3D11UnorderedAccessView* ppUAViewExDataNULL[1] = { NULL };
		pd3dImmediateContext->CSSetUnorderedAccessViews( 1, 1, ppUAViewExDataNULL, (UINT*)(&aUAViewsExData) );
		ID3D11ShaderResourceView* ppSRVExDataNULL[1] = { NULL };
		pd3dImmediateContext->CSSetShaderResources( 1, 1, ppSRVExDataNULL );

		//pd3dImmediateContext->CSSetShader( NULL, NULL, 0 );

		SWAP( g_pParticleExData0, g_pParticleExData1 );
		SWAP( g_pParticleExDataRV0, g_pParticleExDataRV1 );
		SWAP( g_pParticleExDataUAV0, g_pParticleExDataUAV1 );
		//--------------------------------------------------------------------------------------------------
		// end extended data
		//--------------------------------------------------------------------------------------------------
		#endif
    }

    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );
}

//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                         void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
    case IDC_TOGGLEFULLSCREEN:
        DXUTToggleFullScreen(); break;
    case IDC_TOGGLEREF:
        DXUTToggleREF(); break;
    case IDC_CHANGEDEVICE:
        g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;

    case IDC_RESETPARTICLES:
        {
            SAFE_RELEASE(g_pParticle0);
            SAFE_RELEASE(g_pParticle1);
            SAFE_RELEASE(g_pParticleRV0);
            SAFE_RELEASE(g_pParticleRV1);
            SAFE_RELEASE(g_pParticleUAV0);
            SAFE_RELEASE(g_pParticleUAV1);
            CreateBuffers(DXUTGetD3D11Device());

			#ifdef EXDATA
			//--------------------------------------------------------------------------------------------------
			// extended data
			//--------------------------------------------------------------------------------------------------
			SAFE_RELEASE( g_pParticleExData0 );
			SAFE_RELEASE( g_pParticleExDataRV0 );
			SAFE_RELEASE( g_pParticleExDataUAV0 );
			SAFE_RELEASE( g_pParticleExData1 );
			SAFE_RELEASE( g_pParticleExDataRV1 );
			SAFE_RELEASE( g_pParticleExDataUAV1 );
			CreateParticleExDataBuffers(DXUTGetD3D11Device());
			//--------------------------------------------------------------------------------------------------
			// end extended data
			//--------------------------------------------------------------------------------------------------
			#endif

            break;
        }
    }
}

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // reject any device which doesn't support CS4x
    if ( DeviceInfo->ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x == FALSE )
        return false;
    
    return true;
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnD3D11DestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

	g_pd3dDevice = pd3dDevice;

    static bool bFirstOnCreateDevice = true;

    // Warn the user that in order to support CS4x, a non-hardware device has been created, continue or quit?
    if ( DXUTGetDeviceSettings().d3d11.DriverType != D3D_DRIVER_TYPE_HARDWARE && bFirstOnCreateDevice )
    {
        if ( MessageBox( 0, L"CS4x capability is missing. "\
                            L"In order to continue, a non-hardware device has been created, "\
                            L"it will be very slow, continue?", L"Warning", MB_ICONEXCLAMATION | MB_YESNO ) != IDYES )
		return E_FAIL;
    }

    CWaitDlg CompilingShadersDlg;
    CompilingShadersDlg.ShowDialog( L"Compiling Shaders..." );

    bFirstOnCreateDevice = false;

    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS ho;
    V_RETURN( pd3dDevice->CheckFeatureSupport( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &ho, sizeof(ho) ) );

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

    ID3DBlob* pBlobRenderParticlesVS = NULL;
    ID3DBlob* pBlobRenderParticlesGS = NULL;
    ID3DBlob* pBlobRenderParticlesPS = NULL;
    ID3DBlob* pBlobCalcCS = NULL;

    // Create the shaders
    V_RETURN( CompileShaderFromFile( L"ParticleDraw.hlsl", "VSParticleDraw", "vs_4_0", &pBlobRenderParticlesVS ) );
    V_RETURN( CompileShaderFromFile( L"ParticleDraw.hlsl", "GSParticleDraw", "gs_4_0", &pBlobRenderParticlesGS ) );
    V_RETURN( CompileShaderFromFile( L"ParticleDraw.hlsl", "PSParticleDraw", "ps_4_0", &pBlobRenderParticlesPS ) );
    V_RETURN( CompileShaderFromFile( L"ParticleCS40.hlsl", "CSSimpleMain", "cs_4_0", &pBlobCalcCS ) );

    V_RETURN( pd3dDevice->CreateVertexShader( pBlobRenderParticlesVS->GetBufferPointer(), pBlobRenderParticlesVS->GetBufferSize(), NULL, &g_pRenderParticlesVS ) );
    V_RETURN( pd3dDevice->CreateGeometryShader( pBlobRenderParticlesGS->GetBufferPointer(), pBlobRenderParticlesGS->GetBufferSize(), NULL, &g_pRenderParticlesGS ) );
    V_RETURN( pd3dDevice->CreatePixelShader( pBlobRenderParticlesPS->GetBufferPointer(), pBlobRenderParticlesPS->GetBufferSize(), NULL, &g_pRenderParticlesPS ) );
    V_RETURN( pd3dDevice->CreateComputeShader( pBlobCalcCS->GetBufferPointer(), pBlobCalcCS->GetBufferSize(), NULL, &g_pCalcCS ) );

    // Create our vertex input layout
    /*
	const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	*/
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{  },
	};
//    V_RETURN( pd3dDevice->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[0] ), pBlobRenderParticlesVS->GetBufferPointer(), pBlobRenderParticlesVS->GetBufferSize(), &g_pParticleVertexLayout ) );

	// créé un warning parce que la taille est 0 mais c'est normal vu qu'on envoie aucun sommet en entrée du pipeline
	V_RETURN( pd3dDevice->CreateInputLayout( layout, 0, pBlobRenderParticlesVS->GetBufferPointer(), pBlobRenderParticlesVS->GetBufferSize(), &g_pParticleVertexLayout ) );



    SAFE_RELEASE( pBlobRenderParticlesVS );
    SAFE_RELEASE( pBlobRenderParticlesGS );
    SAFE_RELEASE( pBlobRenderParticlesPS );
    SAFE_RELEASE( pBlobCalcCS );

//    V_RETURN( CreateParticleBuffer( pd3dDevice ) );
//    V_RETURN( CreateParticleBuffers(pd3dDevice) );



	// générer les aléatoires pour la cg
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = 512 * sizeof(D3DXVECTOR4);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(D3DXVECTOR4);
		desc.Usage = D3D11_USAGE_DEFAULT;

		// Initialize the data in the buffers
		D3DXVECTOR4 pRandom[512];
		for( int i = 0; i < 512; ++i )
		{
			pRandom[i].x = float(rand()) / float(RAND_MAX);
			pRandom[i].y = float(rand()) / float(RAND_MAX);
			pRandom[i].z = float(rand()) / float(RAND_MAX);
			pRandom[i].w = float(rand()) / float(RAND_MAX);
		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pRandom;
		V_RETURN( pd3dDevice->CreateBuffer( &desc, &InitData, &g_pRandom ) );

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory( &DescRV, sizeof( DescRV ) );
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		V_RETURN( pd3dDevice->CreateShaderResourceView( g_pRandom, &DescRV, &g_pRandomRV ) );
	}



	#ifdef EXDATA
	//--------------------------------------------------------------------------------------------------
	// extended data
	//--------------------------------------------------------------------------------------------------
	V_RETURN( CreateParticleExDataBuffers(pd3dDevice) );
	//--------------------------------------------------------------------------------------------------
	// end extended data
	//--------------------------------------------------------------------------------------------------
	#endif

    // Setup constant buffer
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;
    Desc.ByteWidth = sizeof( CB_GS );
    V_RETURN( pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbGS ) );

    Desc.ByteWidth = sizeof( CB_CS );
    V_RETURN( pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbCS ) );

    // Load the Particle Texture
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"misc\\explosion00.bmp" ) );
    V_RETURN( D3DX11CreateShaderResourceViewFromFile( pd3dDevice, str, NULL, NULL, &g_pParticleTexRV, NULL ) );

    D3D11_SAMPLER_DESC SamplerDesc;
    ZeroMemory( &SamplerDesc, sizeof(SamplerDesc) );
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    V_RETURN( pd3dDevice->CreateSamplerState( &SamplerDesc, &g_pSampleStateLinear ) );

    D3D11_BLEND_DESC BlendStateDesc;
    ZeroMemory( &BlendStateDesc, sizeof(BlendStateDesc) );
    BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
    V_RETURN( pd3dDevice->CreateBlendState( &BlendStateDesc, &g_pBlendingStateParticle ) );

    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
    ZeroMemory( &DepthStencilDesc, sizeof(DepthStencilDesc) );
    DepthStencilDesc.DepthEnable = FALSE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    pd3dDevice->CreateDepthStencilState( &DepthStencilDesc, &g_pDepthStencilState );

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye( -g_fSpread * 2, g_fSpread * 4, -g_fSpread * 3 );
    D3DXVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );

    CompilingShadersDlg.DestroyDialog();

    return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 10.0f, 500000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_Camera.SetButtonMasks( 0, MOUSE_WHEEL, MOUSE_LEFT_BUTTON | MOUSE_MIDDLE_BUTTON | MOUSE_RIGHT_BUTTON );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return hr;
}

void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();    
}

void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 2, 0 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

    g_pTxtHelper->End();
}

//--------------------------------------------------------------------------------------
bool RenderParticles( ID3D11DeviceContext* pd3dImmediateContext, D3DXMATRIX& mView, D3DXMATRIX& mProj )
{
    D3DXMATRIX mWorldViewProj;

    ID3D11BlendState *pBlendState0 = NULL;
    ID3D11DepthStencilState *pDepthStencilState0 = NULL;
    UINT SampleMask0, StencilRef0;
    D3DXVECTOR4 BlendFactor0;
    pd3dImmediateContext->OMGetBlendState( &pBlendState0, &BlendFactor0.x, &SampleMask0 );
    pd3dImmediateContext->OMGetDepthStencilState( &pDepthStencilState0, &StencilRef0 );

    pd3dImmediateContext->VSSetShader( g_pRenderParticlesVS, NULL, 0 );
    pd3dImmediateContext->GSSetShader( g_pRenderParticlesGS, NULL, 0 );
    pd3dImmediateContext->PSSetShader( g_pRenderParticlesPS, NULL, 0 );
    
    pd3dImmediateContext->IASetInputLayout( g_pParticleVertexLayout );

    // Set IA parameters
	/*
    ID3D11Buffer* pBuffers[1] = { g_pParticleBuffer };
    UINT stride[1] = { sizeof( PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
    pd3dImmediateContext->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
	//*/
    pd3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

    ID3D11ShaderResourceView* aRViews[ 1 ] = { g_pParticleRV0 };
    pd3dImmediateContext->VSSetShaderResources( 0, 1, aRViews );

	#ifdef EXDATA
	//--------------------------------------------------------------------------------------------------
	// extended data
	//--------------------------------------------------------------------------------------------------
	ID3D11ShaderResourceView* aRViewsExData[ 1 ] = { g_pParticleExDataRV0 };
	pd3dImmediateContext->VSSetShaderResources( 1, 1, aRViewsExData );
	//--------------------------------------------------------------------------------------------------
	// end extended data
	//--------------------------------------------------------------------------------------------------
	#endif

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    pd3dImmediateContext->Map( g_pcbGS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    CB_GS* pCBGS = ( CB_GS* )MappedResource.pData; 
    D3DXMatrixMultiply( &pCBGS->m_WorldViewProj, &mView, &mProj );   
    D3DXMatrixInverse( &pCBGS->m_InvView, NULL, &mView );
    pd3dImmediateContext->Unmap( g_pcbGS, 0 );
    pd3dImmediateContext->GSSetConstantBuffers( 0, 1, &g_pcbGS );

    pd3dImmediateContext->PSSetShaderResources( 0, 1, &g_pParticleTexRV );
    pd3dImmediateContext->PSSetSamplers( 0, 1, &g_pSampleStateLinear );

    pd3dImmediateContext->OMSetBlendState( g_pBlendingStateParticle, D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF  );
    pd3dImmediateContext->OMSetDepthStencilState( g_pDepthStencilState, 0 );

    pd3dImmediateContext->Draw( MAX_PARTICLES, 0 );

    ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };
    pd3dImmediateContext->VSSetShaderResources( 0, 1, ppSRVNULL );
    pd3dImmediateContext->PSSetShaderResources( 0, 1, ppSRVNULL );

	#ifdef EXDATA
	//--------------------------------------------------------------------------------------------------
	// extended data
	//--------------------------------------------------------------------------------------------------
	pd3dImmediateContext->VSSetShaderResources( 1, 1, ppSRVNULL );
	//--------------------------------------------------------------------------------------------------
	// end extended data
	//--------------------------------------------------------------------------------------------------
	#endif

    /*ID3D11Buffer* ppBufNULL[1] = { NULL };
    pd3dImmediateContext->GSSetConstantBuffers( 0, 1, ppBufNULL );*/

    pd3dImmediateContext->GSSetShader( NULL, NULL, 0 );
    pd3dImmediateContext->OMSetBlendState( pBlendState0, &BlendFactor0.x, SampleMask0 ); SAFE_RELEASE(pBlendState0);
    pd3dImmediateContext->OMSetDepthStencilState( pDepthStencilState0, StencilRef0 ); SAFE_RELEASE(pDepthStencilState0);

    return true;
}

void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext )
{
    float ClearColor[4] = { 0.0, 0.0, 0.0, 0.0 };

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }
    
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

    D3DXMATRIX mView;
    D3DXMATRIX mProj;

    // Get the projection & view matrix from the camera class
    mProj = *g_Camera.GetProjMatrix();
    mView = *g_Camera.GetViewMatrix();

    // Render the particles
    RenderParticles( pd3dImmediateContext, mView, mProj );

    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
    RenderText();
    DXUT_EndPerfEvent();

    // The following could be used to output fps stats into debug output window,
    // which is useful because you can then turn off all UI rendering as they cloud performance
    /*static DWORD dwTimefirst = GetTickCount();
    if ( GetTickCount() - dwTimefirst > 5000 )
    {    
        OutputDebugString( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
        OutputDebugString( L"\n" );
        dwTimefirst = GetTickCount();
    }*/
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnD3D11CreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    g_D3DSettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

    SAFE_RELEASE( g_pParticleBuffer ); 
    SAFE_RELEASE( g_pParticleVertexLayout );

    SAFE_RELEASE( g_pParticle0 );
    SAFE_RELEASE( g_pParticle1 );
    SAFE_RELEASE( g_pParticleRV0 );
    SAFE_RELEASE( g_pParticleRV1 );
    SAFE_RELEASE( g_pParticleUAV0 );
    SAFE_RELEASE( g_pParticleUAV1 );

    SAFE_RELEASE( g_pcbGS );
    SAFE_RELEASE( g_pcbCS );

    SAFE_RELEASE( g_pParticleTexRV );

    SAFE_RELEASE( g_pRenderParticlesVS );
    SAFE_RELEASE( g_pRenderParticlesGS );
    SAFE_RELEASE( g_pRenderParticlesPS );
    SAFE_RELEASE( g_pCalcCS );
    SAFE_RELEASE( g_pSampleStateLinear );
    SAFE_RELEASE( g_pBlendingStateParticle );
    SAFE_RELEASE( g_pDepthStencilState );

	SAFE_RELEASE( g_pRandom );
	SAFE_RELEASE( g_pRandomRV );
	SAFE_RELEASE( g_pGroups );
	SAFE_RELEASE( g_pGroupsRV );
	SAFE_RELEASE( g_pInterpolators );
	SAFE_RELEASE( g_pInterpolatorsRV );
	SAFE_RELEASE( g_pModifiers );
	SAFE_RELEASE( g_pModifiersRV );
	SAFE_RELEASE( g_pZones );
	SAFE_RELEASE( g_pZonesRV );

	#ifdef EXDATA
	//--------------------------------------------------------------------------------------------------
	// extended data
	//--------------------------------------------------------------------------------------------------
	SAFE_RELEASE( g_pParticleExData0 );
	SAFE_RELEASE( g_pParticleExDataRV0 );
	SAFE_RELEASE( g_pParticleExDataUAV0 );
	SAFE_RELEASE( g_pParticleExData1 );
	SAFE_RELEASE( g_pParticleExDataRV1 );
	SAFE_RELEASE( g_pParticleExDataUAV1 );
	//--------------------------------------------------------------------------------------------------
	// end extended data
	//--------------------------------------------------------------------------------------------------
	#endif
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	HRESULT hr;

	if( bKeyDown )
	{
		switch( nChar )
		{
		case 'A':
			g_vecInterpolators[1] = createSimpleInterpolator(D3DXVECTOR4(1.0f, 0.1f, 0.1f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.0f));
			// Interpolator
			{
				ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();

				D3D11_BUFFER_DESC desc;
				ZeroMemory( &desc, sizeof(desc) );
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.ByteWidth = UINT(g_vecInterpolators.size() * sizeof(Interpolator));
				desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				desc.StructureByteStride = sizeof(Interpolator);
				desc.Usage = D3D11_USAGE_DEFAULT;

				ID3D11Buffer *pInterpolators = NULL;
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = &g_vecInterpolators[0];
				V( g_pd3dDevice->CreateBuffer( &desc, &InitData, &pInterpolators ) );

				pd3dImmediateContext->CopyResource(g_pInterpolators, pInterpolators);

				SAFE_RELEASE( pInterpolators );
			}
			break;
		case 'Z':
			g_vecGroups[0].Modifiers().z = -1.0f;
			// Group
			{
				ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();

				D3D11_BUFFER_DESC desc;
				ZeroMemory( &desc, sizeof(desc) );
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.ByteWidth = UINT(g_vecGroups.size() * sizeof(Group));
				desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				desc.StructureByteStride = sizeof(Group);
				desc.Usage = D3D11_USAGE_DEFAULT;

				ID3D11Buffer *pGroups = NULL;
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = &g_vecGroups[0];
				V( g_pd3dDevice->CreateBuffer( &desc, &InitData, &pGroups ) );

				pd3dImmediateContext->CopyResource(g_pGroups, pGroups);

				SAFE_RELEASE( pGroups );
			}
			break;
		}
	}
}