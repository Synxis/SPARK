//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
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


#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL.h>

#include <FTGL/ftgl.h>

#include "SPK.h"
#include "SPK_GL.h"

using namespace std;
using namespace SPK;
using namespace SPK::GL;

float angleY = 30.0f;
float angleX = 25.0f;
float camPosZ = 12.0f;

int deltaTime = 0;

FTGLTextureFont* fontPtr;

GLuint textureGrass;
GLuint textureLight;

Group* fireGroup = NULL;
Group* smokeGroup = NULL;
Emitter* smokeEmitter = NULL;
System* particleSystem = NULL;

const float PI = 3.14159265358979323846f;

const string STR_LOADING = "PLEASE WAIT...";
const string STR_SMOKE = "SMOKE : ";
const string STR_NB_PARTICLES = "NB PARTICLES : ";
const string STR_FPS = "FPS : ";

string strSmoke = STR_SMOKE + "ON";
string strNbParticles = STR_NB_PARTICLES;
string strFps = STR_FPS;

int screenWidth;
int screenHeight;
float screenRatio;

int drawText = 2;

int lightTime = 0;
float lightDist = 0.0f;
float lightIntensity = 1.0f;

bool renderEnv = true;
bool paused = false;

// Converts an int into a string
string int2Str(int a)
{
    ostringstream stm;
    stm << a;
    return stm.str();
}

// Loads a texture
bool loadTexture(GLuint& index,char* path,GLuint type,GLuint clamp,bool mipmap)
{
	SDL_Surface *particleImg; 
	particleImg = SDL_LoadBMP(path);
	if (particleImg == NULL)
	{
		cout << "Unable to load bitmap :" << SDL_GetError() << endl;
		return false;
	}

	// converts from BGR to RGB
	if ((type == GL_RGB)||(type == GL_RGBA))
	{
		const int offset = (type == GL_RGB ? 3 : 4);
		unsigned char* iterator = static_cast<unsigned char*>(particleImg->pixels);
		unsigned char *tmp0,*tmp1;
		for (int i = 0; i < particleImg->w * particleImg->h; ++i)
		{
			tmp0 = iterator;
			tmp1 = iterator + 2;
			swap(*tmp0,*tmp1);
			iterator += offset;
		}
	}

	glGenTextures(1,&index);
	glBindTexture(GL_TEXTURE_2D,index);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clamp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clamp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if (mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D,
			type,
			particleImg->w,
			particleImg->h,
			type,
			GL_UNSIGNED_BYTE,
			particleImg->pixels);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			type,
			particleImg->w,
			particleImg->h,
			0,
			type,
			GL_UNSIGNED_BYTE,
			particleImg->pixels);
	}

	SDL_FreeSurface(particleImg);

	return true;
}

// Draws the bounding box for a particle group
void drawBoundingBox(const Group& group)
{
	if (!group.isAABBComputingEnabled())
		return;

	Vector3D AABBMin = group.getAABBMin();
	Vector3D AABBMax = group.getAABBMax();

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,0.0f);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	
	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);
	glEnd();
}

void renderFirstFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers();
	SDL_GL_SwapBuffers();
}
// Renders the scene
void render()
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,screenRatio,0.01f,80.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();                       

	glPushMatrix();
	glTranslatef(0.0f,0.0f,-camPosZ);
	glRotatef(angleX,1.0f,0.0f,0.0f);
	glRotatef(angleY,0.0f,1.0f,0.0f);
	
	if (renderEnv)
	{
		if (!paused)
		{
			lightTime += deltaTime;
			if (lightTime >= 50)
			{
				lightDist = -random(0.0f,0.05f);
				lightIntensity = 1.0f + (lightDist * 1.0f);
				lightTime -= lightTime;
			}
		}

		// light mask
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textureLight);
		glBegin(GL_QUADS);
		glColor3f(lightIntensity,lightIntensity,lightIntensity);
		glTexCoord2f(1.0f - lightDist,1.0f - lightDist);
		glVertex3f(10.0f,-1.2f,10.0f);
		glTexCoord2f(1.0f - lightDist,lightDist);
		glVertex3f(10.0f,-1.2f,-10.0f);
		glTexCoord2f(lightDist,lightDist);
		glVertex3f(-10.0f,-1.2f,-10.0f);
		glTexCoord2f(lightDist,1.0f - lightDist);
		glVertex3f(-10.0f,-1.2f,10.0f);
		glEnd();

		// grass
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
		glBindTexture(GL_TEXTURE_2D,textureGrass);
		glBegin(GL_QUADS);
		glColor3f(0.5f,1.0f,0.2f);
		glTexCoord2f(5.0f,5.0f);
		glVertex3f(10.0f,-1.2f,10.0f);
		glTexCoord2f(5.0f,-5.0f);
		glVertex3f(10.0f,-1.2f,-10.0f);
		glTexCoord2f(-5.0f,-5.0f);
		glVertex3f(-10.0f,-1.2f,-10.0f);
		glTexCoord2f(-5.0f,5.0f);
		glVertex3f(-10.0f,-1.2f,10.0f);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	drawBoundingBox(*fireGroup);
	drawBoundingBox(*smokeGroup);

	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	GLRenderer::saveGLStates();
	particleSystem->render();
	GLRenderer::restoreGLStates();

	if (drawText != 0)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,screenWidth,0,screenHeight);

		// Renders info strings
		glDisable(GL_DEPTH_TEST);
		glColor3f(1.0f,1.0f,1.0f);
		if (drawText == 2)
		{
			fontPtr->Render(strSmoke.c_str(),-1,FTPoint(4.0f,72.0f));
			fontPtr->Render(strNbParticles.c_str(),-1,FTPoint(4.0f,40.0f));
		}
		fontPtr->Render(strFps.c_str(),-1,FTPoint(4.0f,8.0f));
	}

	SDL_GL_SwapBuffers();
}

// Calls back function to create smoke when a fire particle dies (1 chance out of 4)
void createSmoke(Particle& particle)
{
	if (random(0,4) == 0)
		smokeGroup->addParticles(1,particle.position(),smokeEmitter);
}

// Main function
int main(int argc, char *argv[])
{
	// random seed
	randomSeed = static_cast<unsigned int>(time(NULL));

	SDL_Event event;

	// inits SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SPARK Fire Demo",NULL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	// vsync
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);

	SDL_SetVideoMode(0,0,32,SDL_OPENGL | SDL_FULLSCREEN);
	SDL_ShowCursor(0);

	SDL_Surface screen = *SDL_GetVideoSurface();
	renderFirstFrame();

	// inits openGL
	screenWidth = screen.w;
	screenHeight = screen.h;
	screenRatio = (float)screen.w / (float)screen.h;
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glViewport(0,0,screen.w,screen.h);

	// Loads texture font
	FTGLTextureFont font = FTGLTextureFont("res/font.ttf");
	if(font.Error())
		return 1;
	font.FaceSize(24);
	fontPtr = &font;

	// Loads grass texture
	if (!loadTexture(textureGrass,"res/grass.bmp",GL_RGB,GL_REPEAT,true))
		return 1;

	// Loads lightmap
	if (!loadTexture(textureLight,"res/lightmap.bmp",GL_RGB,GL_CLAMP,false))
		return 1;

	// Loads fire texture
	GLuint textureFire;
	if (!loadTexture(textureFire,"res/fire.bmp",GL_RGBA,GL_CLAMP,false))
		return 1;

	// Loads smoke texture
	GLuint textureSmoke;
	if (!loadTexture(textureSmoke,"res/smoke.bmp",GL_RGBA,GL_CLAMP,false))
		return 1;

	// Inits Particle Engine

	// Those vectors are inverted gravity so that the fire and smoke will rise up
	Vector3D fireForce(0.0f,0.8f,0.0f);
	Vector3D smokeForce(0.0f,0.4f,0.0f);

	// Renderers
	GLPointRenderer* basicRenderer = GLPointRenderer::create();

	GLQuadRenderer* fireRenderer = GLQuadRenderer::create();
	fireRenderer->setScale(0.3f,0.3f);
	fireRenderer->setTexturingMode(TEXTURE_2D);
	fireRenderer->setTexture(textureFire);
	fireRenderer->setTextureBlending(GL_MODULATE);
	fireRenderer->setBlending(BLENDING_ADD);
	fireRenderer->enableRenderingHint(DEPTH_WRITE,false);

	GLQuadRenderer* smokeRenderer = GLQuadRenderer::create();
	smokeRenderer->setScale(0.3f,0.3f);
	smokeRenderer->setTexturingMode(TEXTURE_2D);
	smokeRenderer->setTexture(textureSmoke);
	smokeRenderer->setTextureBlending(GL_MODULATE);
	smokeRenderer->setBlending(BLENDING_ALPHA);
	smokeRenderer->enableRenderingHint(DEPTH_WRITE,false);

	// Models
	Model* fireModel = Model::create(FLAG_ALPHA | FLAG_SIZE | FLAG_ANGLE,
		FLAG_ALPHA | FLAG_ANGLE,
		FLAG_SIZE | FLAG_ANGLE);
	fireModel->setParam(PARAM_ALPHA,0.4f,0.0f);
	fireModel->setParam(PARAM_SIZE,0.4f,1.2f);
	fireModel->setParam(PARAM_ANGLE,0.0f,2.0f * PI,0.0f,2.0f * PI); // the texture is rotated over time
	fireModel->setLifeTime(0.2f,2.0f);

	Model* smokeModel = Model::create(FLAG_ALPHA | FLAG_SIZE | FLAG_ANGLE,
		FLAG_ALPHA | FLAG_SIZE,
		FLAG_SIZE | FLAG_ANGLE);
	smokeModel->setParam(PARAM_ALPHA,0.075f,0.0f);
	smokeModel->setParam(PARAM_SIZE,0.2f,0.8f,4.8f,4.8f);
	smokeModel->setParam(PARAM_ANGLE,0.0f,2.0f * PI);
	smokeModel->setLifeTime(3.0f,6.0f);

	// Emitters
	// The emitters are arranged so that the fire looks realistic
	StraightEmitter* fireEmitter1 = StraightEmitter::create(Vector3D(1.0f,0.2f,0.0f));
	fireEmitter1->setZone(Sphere::create(Vector3D(0.15f,-1.2f,0.075f),0.1f));
	fireEmitter1->setFlow(100);
	fireEmitter1->setForce(0.1f,0.5f);

	StraightEmitter* fireEmitter2 = StraightEmitter::create(Vector3D(0.0f,1.0f,0.0f));
	fireEmitter2->setZone(Sphere::create(Vector3D(0.0f,-1.0f,0.0f),0.5f));
	fireEmitter2->setFlow(500);
	fireEmitter2->setForce(0.1f,0.8f);

	StraightEmitter* fireEmitter3 = StraightEmitter::create(Vector3D(-0.6f,0.4f,-0.8f));
	fireEmitter3->setZone(Sphere::create(Vector3D(-0.375f,-1.15f,-0.375f),0.3f));
	fireEmitter3->setFlow(300);
	fireEmitter3->setForce(0.1f,0.6f);

	StraightEmitter* fireEmitter4 = StraightEmitter::create(Vector3D(-0.8f,0.2f,0.2f));
	fireEmitter4->setZone(Sphere::create(Vector3D(-0.255f,-1.2f,0.225f),0.2f));
	fireEmitter4->setFlow(100);
	fireEmitter4->setForce(0.1f,0.5f);

	StraightEmitter* fireEmitter5 = StraightEmitter::create(Vector3D(0.1f,0.2f,-1.0f));
	fireEmitter5->setZone(Sphere::create(Vector3D(-0.075f,-1.2f,-0.3f),0.2f));
	fireEmitter5->setFlow(100);
	fireEmitter5->setForce(0.1f,0.5f);

	smokeEmitter = SphericEmitter::create(Vector3D(0.0f,1.0f,0.0f),0.0f,0.5f * PI);
	smokeEmitter->setForce(0.5f,1.0f);

	// Groups
	fireGroup = Group::create(fireModel,1400);
	fireGroup->addEmitter(fireEmitter1);
	fireGroup->addEmitter(fireEmitter2);
	fireGroup->addEmitter(fireEmitter3);
	fireGroup->addEmitter(fireEmitter4);
	fireGroup->addEmitter(fireEmitter5);
	fireGroup->setRenderer(fireRenderer);
	fireGroup->setCustomDeath(&createSmoke);
	fireGroup->setGravity(fireForce);

	smokeGroup = Group::create(smokeModel,1800);
	smokeGroup->setRenderer(smokeRenderer);
	smokeGroup->setGravity(smokeForce);
	
	// System
	particleSystem = System::create();
	particleSystem->addGroup(smokeGroup);
	particleSystem->addGroup(fireGroup);
	
	bool smoke = true;
	bool exit = false;
	
	// renderValue :
	// 0 : normal
	// 1 : basic render
	// 2 : no render
	unsigned int renderValue = 0;

	cout << "\nSPARK FACTORY AFTER INIT :" << endl;
	SPKFactory::getInstance().traceAll();

	SDL_Delay(3000);
	while (SDL_PollEvent(&event)){}

	std::deque<unsigned int> frameFPS;
	frameFPS.push_back(SDL_GetTicks());

	while(!exit)
	{
		while (SDL_PollEvent(&event))
		{
			// if esc is pressed, exit
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_ESCAPE))
				exit = true;

			// if del is pressed, reinit the system
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_DELETE))
				particleSystem->empty();

			// if F1 is pressed, we display or not the text
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F1))
			{
				--drawText;
				if (drawText < 0)
					drawText = 2;
			}

			// if F2 is pressed, we display or not the bounding boxes
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F2))
			{
				fireGroup->enableAABBComputing(!fireGroup->isAABBComputingEnabled());
				smokeGroup->enableAABBComputing(!smokeGroup->isAABBComputingEnabled());

				if (paused)
					particleSystem->computeAABB();
			}

			// if F4 is pressed, the renderers are changed
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F4))
			{
				renderValue = (renderValue + 1) % 3;

				switch (renderValue)
				{
				case 0 :
					fireGroup->setRenderer(fireRenderer);
					smokeGroup->setRenderer(smokeRenderer);
					break;

				case 1 :
					fireGroup->setRenderer(basicRenderer);
					smokeGroup->setRenderer(basicRenderer);
					break;

				case 2 :
					fireGroup->setRenderer(NULL);
					smokeGroup->setRenderer(NULL);
					break;
				}
			}

			// if F5 is pressed, environment is rendered are not
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F5))
				renderEnv = !renderEnv;

			// if pause is pressed, the system is paused
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_PAUSE))
				paused = !paused;

			// if space is pressed, add or not smoke
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_SPACE))
			{
				smoke = !smoke;
				if (smoke)
				{
					fireGroup->setCustomDeath(&createSmoke);
					strSmoke = STR_SMOKE + "ON";
				}
				else
				{
					fireGroup->setCustomDeath(NULL);
					strSmoke = STR_SMOKE + "OFF";
				}
			}

			// Moves the camera with the mouse
			if (event.type == SDL_MOUSEMOTION)
			{
				angleY += event.motion.xrel * 0.05f;
				angleX += event.motion.yrel * 0.05f;
				angleX = min(45.0f,max(1.0f,angleX));	// we cannot look under the ground
			}

			// Zoom in and out
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_WHEELDOWN)
					camPosZ = min(18.0f,camPosZ + 0.5f);
				if (event.button.button == SDL_BUTTON_WHEELUP)
					camPosZ = max(3.0f,camPosZ - 0.5f);
			}
		}

		if (!paused)
		{
			// Updates particle system
			particleSystem->update(deltaTime * 0.001f); // 1 defined as a second
		}

		// Renders scene
		render();

		// Computes delta time
		int time = SDL_GetTicks();
		deltaTime = time - frameFPS.back();
		
		frameFPS.push_back(time);

		while((frameFPS.back() - frameFPS.front() > 1000)&&(frameFPS.size() > 2))
			frameFPS.pop_front();

		// Updates info strings
		strNbParticles = STR_NB_PARTICLES + int2Str(particleSystem->getNbParticles());
		int fps = static_cast<int>(((frameFPS.size() - 1) * 1000.0f) / (frameFPS.back() - frameFPS.front()));
		if (drawText == 2)
			strFps = STR_FPS + int2Str(fps);	
		else
			strFps = int2Str(fps);
	}

	cout << "\nSPARK FACTORY BEFORE DESTRUCTION :" << endl;
	SPKFactory::getInstance().traceAll();
	SPKFactory::getInstance().destroyAll();
	cout << "\nSPARK FACTORY AFTER DESTRUCTION :" << endl;
	SPKFactory::getInstance().traceAll();
	SDL_Quit();

	cout << endl;
	system("pause"); // Waits for the user to close the console
	
	return 0;
}