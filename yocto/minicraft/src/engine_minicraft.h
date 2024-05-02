#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

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

	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		
	}

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f,0.0f,0.0f,1.0f));

		initGui();

		world.init_world(rand());

		player = new MAvatar(Renderer->Camera, &world);
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