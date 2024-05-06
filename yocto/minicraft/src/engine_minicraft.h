#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"
#include "shape.h"
#include "terrain.h"


#include <thread>
#include <chrono>

class MEngineMinicraft : public YEngine {
public :
	//Instance
	static MEngineMinicraft* mInstance;

	//World
	MWorld world;

	//Avatar
	MAvatar* player;

	//GUI
	GUIBouton* genWorldSeedButton ;
	GUIEdtBox* genWorldSeedEdtBox;
	GUILabel* genWorldSeedLabel;

	GLuint cubeShader;
	GLuint cube_color;
	YVbo* cubeVbo;

	YVbo* chunkVboO0;
	YVbo* chunkVboO1;
	YVbo* chunkVboO2;
	YVbo* chunkVboO3;
	YVbo* chunkVboT0;
	YVbo* chunkVboT1;
	YVbo* chunkVboT2;
	YVbo* chunkVboT3;

	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		cubeShader = Renderer->createProgram("shaders/cube");
	}

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f,0.0f,0.0f,1.0f));

		initGui();

		player = new MAvatar(Renderer->Camera, &world);

		CubeShape shape(CubeShape::ALL_FACES);
		cubeVbo = shape.createVbo();

		Terrain::Chunk chunk0 = Terrain::Chunk({ 0, 0, 0 });
		Terrain::Chunk chunk1 = Terrain::Chunk({ 1, 0, 0 });
		Terrain::Chunk chunk2 = Terrain::Chunk({ 0, 1, 0 });
		Terrain::Chunk chunk3 = Terrain::Chunk({ 1, 1, 0 });

		chunk0.generate(0);
		chunk1.generate(0);
		chunk2.generate(0);
		chunk3.generate(0);

		chunkVboO0 = chunk0.buildVboOpaque();
		chunkVboO1 = chunk1.buildVboOpaque();
		chunkVboO2 = chunk2.buildVboOpaque();
		chunkVboO3 = chunk3.buildVboOpaque();

		chunkVboT0 = chunk0.buildVboTransparent();
		chunkVboT1 = chunk1.buildVboTransparent();
		chunkVboT2 = chunk2.buildVboTransparent();
		chunkVboT3 = chunk3.buildVboTransparent();
	}

	void update(float elapsed) 
	{
		player->update(elapsed);
	}

	void renderObjects() 
	{
		glUseProgram(0);
		
		//Rendu des axes
		glDisable(GL_LIGHTING);

		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10000, 0, 0);
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10000, 0);
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10000);
		glEnd();

		glEnable(GL_LIGHTING);

		glPushMatrix();
		glUseProgram(cubeShader);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(cubeShader);
		
		//Opaque
		glDisable(GL_BLEND);
		chunkVboO0->render();
		chunkVboO1->render();
		chunkVboO2->render();
		chunkVboO3->render();
		
		//Transparent
		glEnable(GL_BLEND);
		chunkVboT0->render();
		chunkVboT1->render();
		chunkVboT2->render();
		chunkVboT3->render();

		glPopMatrix();
	}

	void resize(int width, int height) {
	
	}

	/*INPUTS*/
	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{	
		if (special)
			ScreenManager->specialKeyCallback(key, down, 0);
		else
			ScreenManager->keyCallback(key, down, 0);
		
		if (key == GLUT_KEY_F6 && down)
			Mouse::changeMode();
	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{

	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{

	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{

	}

	void initGui()
	{
		genWorldSeedButton = new GUIBouton();
		genWorldSeedButton->Titre = "Regen";
		genWorldSeedButton->Width = 50;
		genWorldSeedButton->X = 180;
		genWorldSeedButton->Y = 40;

		genWorldSeedButton->setOnClick(onGenWorldSeedButtonClick);

		genWorldSeedEdtBox = new GUIEdtBox();
		genWorldSeedEdtBox->DrawBorder = true;
		genWorldSeedEdtBox->X = 70;
		genWorldSeedEdtBox->Y = 40;

		genWorldSeedLabel = new GUILabel();
		genWorldSeedLabel->Text = "Seed :";
		genWorldSeedLabel->Width = 50;
		genWorldSeedLabel->X = 10;
		genWorldSeedLabel->Y = 40;

		ScreenParams->addElement(genWorldSeedButton);
		ScreenParams->addElement(genWorldSeedEdtBox);
		ScreenParams->addElement(genWorldSeedLabel);
	}

	static void onGenWorldSeedButtonClick(GUIBouton* bouton)
	{
		hash<string> hasher;
		size_t newSeed = hasher(mInstance->genWorldSeedEdtBox->Text);
		mInstance->world.init_world(newSeed);
	}
};

MEngineMinicraft* MEngineMinicraft::mInstance = static_cast<MEngineMinicraft*>(getInstance());

#endif