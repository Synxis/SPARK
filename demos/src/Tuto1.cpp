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

#include <SPK.h>
#include <SPK_GL.h>

using namespace std;
using namespace SPK;
using namespace SPK::GL;

float angleY = 0.0f;
float camPosZ = 5.0f;

int deltaTime = 0;

FTGLTextureFont* fontPtr;

const string STR_NB_PARTICLES = "NB PARTICLES : ";
const string STR_FPS = "FPS : ";

string strNbParticles = STR_NB_PARTICLES;
string strFps = STR_FPS;

int screenWidth;
int screenHeight;
float screenRatio;

int drawText = 2;

deque<System*> particleSystems;

SPK_ID BaseSystemID = NO_ID;

// Converts an int into a string
string int2Str(int a)
{
    ostringstream stm;
    stm << a;
    return stm.str();
}

// Converts a HSV color to RGB
// h E [0,360]
// s E [0,1]
// v E [0,1]
Vector3D convertHSV2RGB(const Vector3D& hsv)
{
	float h = hsv.x;
	float s = hsv.y;
	float v = hsv.z;

	int hi = static_cast<int>(h / 60.0f) % 6;
	float f = h / 60.0f - hi;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch(hi)
	{
	case 0 : return Vector3D(v,t,p);
	case 1 : return Vector3D(q,v,p);
	case 2 : return Vector3D(p,v,t);
	case 3 : return Vector3D(p,q,v);
	case 4 : return Vector3D(t,p,v);
	default : return Vector3D(v,p,q);
	}
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

void renderFirstFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers();
	SDL_GL_SwapBuffers();
}

// Renders the scene
void render()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,screenRatio,0.01f,80.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();                       

	glTranslatef(0.0f,0.0f,-camPosZ);
	glRotatef(angleY,0.0f,1.0f,0.0f);

	// Renders all the particle systems
	for (deque<System*>::const_iterator it = particleSystems.begin(); it != particleSystems.end(); ++it)
		(*it)->render();

	if (drawText != 0)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,screenWidth,0,screenHeight);

		// Renders info strings
		glColor3f(1.0f,1.0f,1.0f);
		if (drawText == 2)
			fontPtr->Render(strNbParticles.c_str(),-1,FTPoint(4.0f,40.0f));
		fontPtr->Render(strFps.c_str(),-1,FTPoint(4.0f,8.0f));
	}

	SDL_GL_SwapBuffers();
}

// Creates the base system and returns its ID
SPK_ID createParticleSystemBase(GLuint textureIndex)
{
	// Creates the model
	Model* model = Model::create(FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_ALPHA,
		FLAG_ALPHA,
		FLAG_RED | FLAG_GREEN | FLAG_BLUE);
	model->setParam(PARAM_ALPHA,1.0f,0.0f); // This makes the particles fade out over time
	model->setLifeTime(1.0f,2.0f);

	// Create the renderer
	GLRenderer* renderer = NULL;
	// If the point sprite extension and the point parameter extensions are supported by the hardware, we use point sprites
	if ((GLPointRenderer::loadGLExtPointSprite())&&(GLPointRenderer::loadGLExtPointParameter()))
	{
		GLPointRenderer* pointRenderer = GLPointRenderer::create();
		pointRenderer->setType(POINT_SPRITE);
		pointRenderer->enableWorldSize(true);
		GLPointRenderer::setPixelPerUnit(45.0f * 3.14159f / 180.f,screenHeight);
		pointRenderer->setSize(0.1f);
		pointRenderer->setTexture(textureIndex);
		renderer = pointRenderer;
	}
	else // else we use quads
	{
		GLQuadRenderer* quadRenderer = GLQuadRenderer::create();
		quadRenderer->setTexturingMode(TEXTURE_2D);
		quadRenderer->setScale(0.1f,0.1f);
		quadRenderer->setTexture(textureIndex);
		renderer = quadRenderer;
	}

	renderer->enableBlending(true);
	renderer->setBlendingFunctions(GL_SRC_ALPHA,GL_ONE); // additive blending
	renderer->setTextureBlending(GL_MODULATE); // the texture is modulated with the particle's color
	renderer->enableRenderingHint(DEPTH_TEST,false); // the depth test is disabled

	// Creates the zone
	Point* source = Point::create();

	// Creates the emitter
	RandomEmitter* emitter = RandomEmitter::create();
	emitter->setForce(2.8f,3.2f);
	emitter->setZone(source);
	emitter->setTank(500);
	emitter->setFlow(-1); // Creates all the particles in the tank at the first frame

	// Creates the Group
	Group* group = Group::create(model,500); // 500 particles is the maximum capacity of the group
	group->addEmitter(emitter);
	group->setGravity(Vector3D(0.0f,-1.0f,0.0f));
	group->setFriction(2.0f);
	group->setRenderer(renderer);
	group->enableEmittersRemoval(true); // This deletes the emitters as soon as they are sleeping

	// Creates the System
	System* system = System::create();
	system->addGroup(group);

	// Defines which objects will be shared by all systems
	model->setShared(true);
	renderer->setShared(true);

	// Creates the base and gets a pointer to the base
	return system->getID();
}

// Creates a particle system from the base system
System* createParticleSystem(const Vector3D& pos,const Vector3D& color)
{
	// Creates a copy of the base system
	System* system = SPK_Copy(System,BaseSystemID);

	// Updates the model with the given color
	Model* model = system->getGroup(0)->getModel();
	model->setParam(PARAM_RED,max(0.0f,color.x - 0.25f),min(1.0f,color.x + 0.25f));
	model->setParam(PARAM_GREEN,max(0.0f,color.y - 0.25f),min(1.0f,color.y + 0.25f));
	model->setParam(PARAM_BLUE,max(0.0f,color.z - 0.25f),min(1.0f,color.z + 0.25f));

	// Locates the zone of the emitter at the given position
	Zone* zone = system->getGroup(0)->getEmitter(0)->getZone();
	zone->setPosition(pos);

	return system;
}

// destroy a particle system
void destroyParticleSystem(System*& system)
{
	// Destroys the given system
	SPK_Destroy(system);
	system = NULL;
}

// Main function
int main(int argc, char *argv[])
{
	SDL_Event event;

	// inits SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SPARK Tutorial 1",NULL);
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

	// Loads particle texture
	GLuint textureParticle;
	if (!loadTexture(textureParticle,"res/flare.bmp",GL_ALPHA,GL_CLAMP,false))
		return 1;

	// random seed
	randomSeed = static_cast<unsigned int>(time(NULL));

	// creates the base system
	BaseSystemID = createParticleSystemBase(textureParticle);
	
	bool exit = false;
	bool paused = false;

	cout << "\nSPARK FACTORY AFTER INIT :" << endl;
	SPKFactory::getInstance().traceAll();
	
	SDL_Delay(3000);
	while (SDL_PollEvent(&event)){}
	
	std::deque<unsigned int> frameFPS;
	frameFPS.push_back(SDL_GetTicks());

	float spacePressed = -1.0f;

	while(!exit)
	{
		while (SDL_PollEvent(&event))
		{
			// if space is pressed, a new system is added
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_SPACE))
			{
				spacePressed = 200.0f;
			}

			if ((event.type == SDL_KEYUP)&&(event.key.keysym.sym == SDLK_SPACE))
			{
				spacePressed = -1.0f;
			}

			// if esc is pressed, exit
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_ESCAPE))
				exit = true;

			// if F1 is pressed, we display or not the text
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F1))
			{
				--drawText;
				if (drawText < 0)
					drawText = 2;
			}

			// if pause is pressed, the system is paused
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_PAUSE))
				paused = !paused;

			// Moves the camera with the mouse
			if (event.type == SDL_MOUSEMOTION)
				angleY += event.motion.xrel * 0.05f;

			// Zoom in and out
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_WHEELDOWN)
					camPosZ = min(10.0f,camPosZ + 0.5f);
				if (event.button.button == SDL_BUTTON_WHEELUP)
					camPosZ = max(0.5f,camPosZ - 0.5f);
			}
		}

		if (!paused)
		{
			// if space is pressed, a new system is added
			if (spacePressed >= 0.0f)
			{
				spacePressed += deltaTime;

				if (spacePressed >= 200.0f)
				{
					Vector3D color(random(0.0f,360.0f),0.8f,1.0f);
					Vector3D position(random(-2.0f,2.0f),random(-2.0f,2.0f),random(-2.0f,2.0f));
					particleSystems.push_back(createParticleSystem(position,convertHSV2RGB(color)));

					spacePressed = 0.0f;
				}
			}

			deque<System*>::iterator it = particleSystems.begin();
			while(it != particleSystems.end())
			{
				// Updates the particle systems
				if (!(*it)->update(deltaTime * 0.001f))
				{
					// If a system is sleeping, destroys it
					destroyParticleSystem(*it);
					// And erases its entry in the container
					it = particleSystems.erase(it);
				}
				else
					++it;
			}
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
		unsigned int nbParticles = 0;
		for (deque<System*>::const_iterator it = particleSystems.begin(); it != particleSystems.end(); ++it)
			nbParticles += (*it)->getNbParticles();
		strNbParticles = STR_NB_PARTICLES + int2Str(nbParticles);
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