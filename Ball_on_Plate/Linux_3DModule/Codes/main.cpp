#include <signal.h>
#include "EventListener.h"
#include "Rotate.h"
#include "GraphKat.h"
#include "rs232.h"

char s[4096];

int main()
{
	signal(SIGINT, signalHandler);

	IrrlichtDevice *device = createDevice(EDT_OPENGL, core::dimension2d<u32>(1366, 768), 16, false, true);

	if (device == 0)
		return 1;

	device->setWindowCaption(L"Ball On Plate User Interface");
	device->setResizable(false);

	video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* env = device->getGUIEnvironment();

	// bu font eklemek için kullanilir.
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("../fonthaettenschweiler.bmp");

	if (font)
		skin->setFont(font);
	skin->setFont(env->getBuiltInFont(), EGDF_TOOLTIP);

	env->addTable(rect<s32>(10, 150, 180, 450), 0, 0, true);
	env->addStaticText(L"        Set Balance Point", rect<s32>(20, 180, 170, 200), true, true, 0, 0, true);
	//env->addStaticText(L"Ball Coordinates:", rect<s32>(190, 150, 330, 170), true, true, 0, 0, true);
	env->addStaticText(L"3D MODEL", rect<s32>(600, 50, 650, 70), true, true, 0, 0, true);
	//IGUIListBox * listbox = env->addListBox(rect<s32>(190, 150, 330, 450), 0, 0, true);
	
	// edit box x ve y için ekler ve maksimum 8 karakter girmesini saglar.
	irr::s32 size = 8;
	env->addEditBox(L"", rect<s32>(20, 210, 90, 200 + 32), true, 0, GUI_ID_XCOOR_EDITBOX)->setMax(size);
	env->addEditBox(L"", rect<s32>(100, 210, 170, 200 + 32), true, 0, GUI_ID_YCOOR_EDITBOX)->setMax(size);

	env->addButton(rect<s32>(20, 250, 170, 250 + 32), 0, GUI_ID_ENTER_BUTTON, L"Change Balance Point", L"Ball Position");
	env->addButton(rect<s32>(20, 290, 170, 290 + 32), 0, GUI_ID_CIRCLE_BUTTON, L"Circle", L"Draw Circle");
	env->addButton(rect<s32>(20, 330, 170, 330 + 32), 0, GUI_ID_RECTANGLE_BUTTON, L"Rectangle", L"Draw Rectangle");
	env->addButton(rect<s32>(20, 370, 170, 370 + 32), 0, GUI_ID_GAME_BUTTON, L"Game Mode", L"Play Game");
	env->addButton(rect<s32>(20, 410, 170, 410 + 32), 0, GUI_ID_QUIT_BUTTON, L"Quit", L"Exits Program");

	// constructor için context classimiz set edilir.
	// Event listener olusturulur yani classimiz
	// cihazimiza event listenerimiz set edilir.
	SAppContext context;
	context.device = device;
	context.counter = 0;
	//context.listbox = listbox;
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	// camera ve isik ayarlari
	// Maya camera mouse ile istedigimiz yonden bakmamizi saglar.
	smgr->addLightSceneNode(0, core::vector3df(30, 50, 30), video::SColorf(1.0f, 1.0f, 1.0f), 100);
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeMaya(0, -100.0f, 100.0f, 100.0f, 0, 35.0f);
	camera->setTarget(core::vector3df(0, 0, 0));

	// objelerin ekleme yeri
	IMesh* meshBox = smgr->getMesh("../ustTahta.obj");
	if (!meshBox)
	{
		device->drop();
		return 1;
	}

	IMesh* meshAlt = smgr->getMesh("../altTahta.obj");
	if (!meshAlt)
	{
		device->drop();
		return 1;
	}

	IMesh* meshBall = smgr->getMesh("../ball.obj");
	if (!meshBall)
	{
		device->drop();
		return 1;
	}

	IMesh* meshCubukZ = smgr->getMesh("../cubuk.obj");
	if (!meshCubukZ)
	{
		device->drop();
		return 1;
	}

	IMesh* meshCubukX = smgr->getMesh("../cubuk.obj");
	if (!meshCubukX)
	{
		device->drop();
		return 1;
	}

	IMesh* meshYildizZ = smgr->getMesh("../yildiz.obj");
	if (!meshYildizZ)
	{
		device->drop();
		return 1;
	}

	IMesh* meshYildizX = smgr->getMesh("../yildizX.obj");
	if (!meshYildizX)
	{
		device->drop();
		return 1;
	}

	IMesh* meshIsik22 = smgr->getMesh("../isik.obj");
	IMeshSceneNode* boxNode = smgr->addMeshSceneNode(meshBox);
	if (boxNode)
	{
		boxNode->setPosition(vector3df(0, 0, 0));
		boxNode->setRotation(vector3df(0, 0, 0));
		boxNode->setMaterialFlag(EMF_LIGHTING, true);
		boxNode->setMaterialTexture(0, driver->getTexture("../boxTexture.jpg"));
		boxNode->addShadowVolumeSceneNode();
	}

	IMeshSceneNode* altNode = smgr->addMeshSceneNode(meshAlt);
	if (altNode)
	{
		altNode->setPosition(vector3df(0, -20, 0));
		altNode->setRotation(vector3df(0, 0, 0));
		altNode->setMaterialFlag(EMF_LIGHTING, true);
		altNode->addShadowVolumeSceneNode();
	}

	IMeshSceneNode* zCubukNode = smgr->addMeshSceneNode(meshCubukZ);
	if (zCubukNode)
	{
		zCubukNode->setPosition(vector3df(-3, -12, -14));
		zCubukNode->setRotation(vector3df(0, 0, 0));
		zCubukNode->setMaterialFlag(EMF_LIGHTING, true);
		zCubukNode->addShadowVolumeSceneNode();
	}

	IMeshSceneNode* xCubukNode = smgr->addMeshSceneNode(meshCubukX);
	if (xCubukNode)
	{
		xCubukNode->setPosition(vector3df(-10.5, -12, 3));
		xCubukNode->setRotation(vector3df(0, 0, 0));
		xCubukNode->setMaterialFlag(EMF_LIGHTING, true);
		xCubukNode->addShadowVolumeSceneNode();
	}

	IMeshSceneNode* zYildizNode = smgr->addMeshSceneNode(meshYildizZ);
	if (zYildizNode)
	{
		zYildizNode->setPosition(vector3df(-2, -12, -14.5));
		zYildizNode->setRotation(vector3df(90, 0, 0));
		zYildizNode->setMaterialFlag(EMF_LIGHTING, true);
		zYildizNode->addShadowVolumeSceneNode();
	}

	IMeshSceneNode* xYildizNode = smgr->addMeshSceneNode(meshYildizX);
	if (xYildizNode)
	{
		xYildizNode->setPosition(vector3df(-11, -12, 1.5));
		xYildizNode->setRotation(vector3df(90, 0, 0));
		xYildizNode->setMaterialFlag(EMF_LIGHTING, true);
		xYildizNode->addShadowVolumeSceneNode();
	}

	IMeshSceneNode* nodeBall = smgr->addMeshSceneNode(meshBall);
	if (nodeBall)
	{
		nodeBall->setVisible(false);
		nodeBall->setPosition(vector3df(0, 0, 0));
		nodeBall->setRotation(vector3df(0, 0, 0));
		nodeBall->setMaterialFlag(EMF_LIGHTING, true);
		nodeBall->getMaterial(0).Shininess = 20.0f;
		nodeBall->setMaterialTexture(0, driver->getTexture("../ballMetalTexture.jpg"));
		nodeBall->addShadowVolumeSceneNode();
	}

	// led ekleme yapılan yer
	ILightSceneNode* myPointLight = smgr->addLightSceneNode();
	myPointLight->setPosition(vector3df(10.0f, 2.2f, 0.0f));

	SLight spotLightData;
	spotLightData.Type = ELT_SPOT;
	spotLightData.DiffuseColor = SColor(0, 255, 0, 0);
	spotLightData.Radius = 0;
	spotLightData.OuterCone = 1;
	spotLightData.InnerCone = 10;
	myPointLight->setLightData(spotLightData);

	ISceneNode* pointLightBill = smgr->addBillboardSceneNode(myPointLight, core::dimension2d<f32>(10, 5));
	pointLightBill->setMaterialFlag(video::EMF_LIGHTING, false);
	pointLightBill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	pointLightBill->setMaterialTexture(0, driver->getTexture("../led.bmp"));


	bool flag = false;

	float x = 0;
	float y = 4;
	float z = 0;
	float xAci = 0;
	float zAci = 0;
	float xAciEski = 0;
	float zAciEski = 0;
	float xAciHam = 0;
	float zAciHam = 0;
	float xHam = 0;
	float zHam = 0;
	int setPointX = 0;
	int setPointY = 0;
	float x1 = 0, x2 = 0, x3 = 0, x4 = 0, x5 = 0, x6 = 0;
	float z1 = 0, z2 = 0, z3 = 0, z4 = 0, z5 = 0, z6 = 0;
	float lambaX = 7;
	float lambaY = 3;
	float lambaZ = 16;
	int lamb = 0;
	int lambaSayisi = 0;
	int whileCounter = 0;

	core::vector3df rotateZ(0, 0, 1);
	core::vector3df rotateX(1, 0, 0);
	char *token[7];
	char *next = NULL;

	int i = 0, k = 0, silmeCounter = 9, lambaCounter = 0;
	int n, bdrate = 9600;
	unsigned char buf[2];
	char str[4096];
	int count = 0;
	char mode[] = { '8', 'N', '1', 0 };

	if (RS232_OpenComport(cport_nr, bdrate, mode))
	{
		printf("Can not open comport\n");
		return(0);
	}

	GraphKat *graph[2];
	graph[0] = new GraphKat(L"X Error Chart", device, rect<s32>(1150, 100, 1350, 320));
	graph[1] = new GraphKat(L"Y Error Chart", device, rect<s32>(1150, 330, 1350, 580));

	u32 now = 0, then = 0, NextGraph = 0;
	while (device->run())
	{
		++whileCounter;

		while (1)
		{
			n = RS232_PollComport(cport_nr, buf, 1);
			if (n > 0)
			{
				buf[n] = 0;
				str[i] = buf[0];
				++i;
				if (buf[0] == ' ')
				{
					++count;
				}

				if (strcmp((char *)buf, "-") == 0)
				{
					str[i - 1] = 0;
					i = 0;
					break;
				}
			}
		}

		sprintf(s, "%s", str);

		if (count == 6)
		{
			token[0] = strtok(s, " ");
			for (int k = 1; k < 7; k++)
			{
				token[k] = strtok(NULL, " ");
			}

			xHam = atof(token[0]);
			zHam = atof(token[1]);
			x = scale(xHam, 0, 300, -12, 12);
			z = scale(zHam, 0, 400, -16, 16);

			xAciHam = atof(token[2]);
			zAciHam = atof(token[3]);

			lamb = atoi(token[4]);
			setPointX = atoi(token[5]);
			setPointY = atoi(token[6]);

			if ((int)xAciHam >= 0 && (int)xAciHam <= 115 && (int)zAciHam >= 10 && (int)zAciHam <= 180 && (int)x >= -12 && (int)x <= 12 &&
				(int)z >= -16 && (int)z <= 16 && setPointX >= 0 && setPointX <= 300 && setPointY >= 0 && setPointY <= 400){

				if (xAciHam < 75)
				{
					xAci = scale(xAciHam, 0, 75, -8, 0);
				}
				else
				{
					xAci = scale(xAciHam, 75, 115, 0, 8);
				}

				zAci = scale(zAciHam, 10, 180, -8, 8);

				if (boxNode)
				{
					boxNode->setRotation(vector3df(0, 0, 0));
					boxNode->setPosition(vector3df(0, 0, 0));
					rotateNodeInWorldSpace(boxNode, zAci, rotateX);
					rotateNodeInWorldSpace(boxNode, xAci, rotateZ);
					xAciEski = xAci;
					zAciEski = zAci;
				}

				if (nodeBall)
				{
					if ((int)zAciHam == 95 && (int)xAciHam == 75)
					{
						silmeCounter++;
						if (silmeCounter == 20)
						{
							nodeBall->setVisible(false);
							silmeCounter = 0;
						}
					}
					else
					{
						nodeBall->setVisible(true);
						nodeBall->setRotation(vector3df(0, 0, 0));
						nodeBall->setPosition(vector3df(x, y, z));
						revolveNodeInWorldSpace(nodeBall, zAci, rotateX, vector3df(0, 0, 0));
						revolveNodeInWorldSpace(nodeBall, xAci, rotateZ, vector3df(0, 0, 0));
					}
				}

				if (myPointLight)
				{
					if (lamb == 22){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(65, 0, 400, -16, 16);
					}
					else if (lamb == 23){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(110, 0, 400, -16, 16);
					}
					else if (lamb == 24){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(170, 0, 400, -16, 16);
					}
					else if (lamb == 25){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(225, 0, 400, -16, 16);
					}
					else if (lamb == 26){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(270, 0, 400, -16, 16);
					}
					else if (lamb == 27){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(320, 0, 400, -16, 16);
					}
					else if (lamb == 28){
						lambaX = scale(242, 0, 300, -12, 12);
						lambaZ = scale(375, 0, 400, -16, 16);
					}
					else if (lamb == 32){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(65, 0, 400, -16, 16);
					}
					else if (lamb == 33){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(110, 0, 400, -16, 16);
					}
					else if (lamb == 34){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(170, 0, 400, -16, 16);
					}
					else if (lamb == 35){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(225, 0, 400, -16, 16);
					}
					else if (lamb == 36){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(270, 0, 400, -16, 16);
					}
					else if (lamb == 37){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(320, 0, 400, -16, 16);
					}
					else if (lamb == 38){
						lambaX = scale(150, 0, 300, -12, 12);
						lambaZ = scale(375, 0, 400, -16, 16);
					}
					else if (lamb == 42){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(65, 0, 400, -16, 16);
					}
					else if (lamb == 43){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(110, 0, 400, -16, 16);
					}
					else if (lamb == 44){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(170, 0, 400, -16, 16);
					}
					else if (lamb == 45){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(225, 0, 400, -16, 16);
					}
					else if (lamb == 46){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(270, 0, 400, -16, 16);
					}
					else if (lamb == 47){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(320, 0, 400, -16, 16);
					}
					else if (lamb == 48){
						lambaX = scale(60, 0, 300, -12, 12);
						lambaZ = scale(375, 0, 400, -16, 16);
					}
					else{
						lambaX = 500;
						lambaZ = 500;
					}

					myPointLight->setRotation(vector3df(0, 0, 0));
					myPointLight->setPosition(vector3df(lambaX, lambaY, lambaZ));
					revolveNodeInWorldSpace(myPointLight, zAci, rotateX, vector3df(0, 0, 0));
					revolveNodeInWorldSpace(myPointLight, xAci, rotateZ, vector3df(0, 0, 0));
				}

				if (zCubukNode)
				{
					zCubukNode->setRotation(vector3df(0, 0, 0));
					zCubukNode->setPosition(vector3df(-3, -12.5, -14.5));
					revolveNodeInWorldSpace(zCubukNode, zAci, rotateX, vector3df(-3, -12.5, 0));
					zCubukNode->setRotation(vector3df(0, 0, 0));
				}

				if (zYildizNode)
				{
					zYildizNode->setRotation(vector3df(90, 0, -zAciHam));
				}

				if (xYildizNode)
				{
					xYildizNode->setPosition(vector3df(-10.5, -12, 2));
					xYildizNode->setRotation(vector3df(xAciHam, 0, 0));
				}

				if (xCubukNode)
				{
					xCubukNode->setRotation(vector3df(0, 0, 0));
					xCubukNode->setPosition(vector3df(-11, -12.5, 3));
					revolveNodeInWorldSpace(xCubukNode, xAci, rotateZ, vector3df(0, -12.5, 3));
					xCubukNode->setRotation(vector3df(0, 0, 0));
				}
			}
		}

		count = 0;
		driver->beginScene(true, true, SColor(0, 100, 100, 100));
		now = device->getTimer()->getTime();
		if (then < now)
		{
			// draw each graph
			graph[0]->drawGreen();
			graph[1]->drawOrange();

			//feed them artificially every 120ms
			if (NextGraph < now)
			{
				if (rand() % 2 == 0)
				{
					if (xAciHam == 75 && zAciHam == 95)
					{
						graph[0]->addValue(0);
						graph[1]->addValue(0);
					}
					else
					{
						graph[0]->addValue(abs((int)setPointX - xHam));
						graph[1]->addValue(abs((int)setPointY - zHam));
					}
				}
				NextGraph = now + 120;
			}

			smgr->drawAll();
			env->drawAll();
			driver->endScene();
			then = now + 30;
		}

		lambaCounter++;
	}

	device->drop();
	

	return 0;
}