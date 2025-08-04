/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TitleScreen.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 10:39:14 by mbatty            #+#    #+#             */
/*   Updated: 2025/08/04 20:48:29 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Scene.hpp"
#include "TitleScreen.hpp"
#include "SceneManager.hpp"
#include "ShaderManager.hpp"
#include "FrameBuffer.hpp"
#include "Skybox.hpp"

extern Skybox	*SKYBOX;
extern ShaderManager	*SHADER_MANAGER;
void	closeWindow(ButtonInfo);
extern SceneManager		*SCENE_MANAGER;

//A FAIRE: 1/42 d'avoir ft_xov

#define TITLES_COUNT 9
#define TITLE_TIME 8

std::string	popupTitles[TITLES_COUNT] =
{
	"by mbatty and mbirou!",
	"outstanding project!",
	"ft_minecraft?",
	"42 angouleme",
	"also try cub3d!",
	"chicken jockey!",
	"scraeyme approved",
	"dont forget to git push",
	"c++ > all"
};

void	startGame()
{
	SCENE_MANAGER->swap("game_scene");
	SCENE_MANAGER->get("title_scene")->getInterfaceManager()->use("leaving");
}

void	startGame(ButtonInfo)
{
	startGame();
}

std::string	selectedWorld;

void	selectWorld(ButtonInfo infos)
{
	selectedWorld = infos.id;
	SCENE_MANAGER->get("title_scene")->getInterfaceManager()->use("world_creation");
}

static void	_buildMainInterface(Interface *interface)
{
	interface->addElement("button_singleplayer", new Button(UIAnchor::UI_CENTER, "start", glm::vec2(0, -90), glm::vec2(300, 80), NULL, NULL));

	interface->addElement("button_options", new Button(UIAnchor::UI_CENTER, "options", glm::vec2(0, 0), glm::vec2(300, 80), []
		(ButtonInfo)
		{
			SCENE_MANAGER->get("title_scene")->getInterfaceManager()->use("options");
		}, NULL));

	interface->addElement("button_quit_game", new Button(UIAnchor::UI_CENTER, "quit game", glm::vec2(0, 90), glm::vec2(300, 80), closeWindow, NULL));

	Text	*text_popup = static_cast<Text*>(interface->addElement("text_popup", new Text(UIAnchor::UI_TOP_CENTER_HALF, "by mbatty and mbirou!", glm::vec2(175, -40), NULL, false)));
	// interface->addElement("image_icon", new Image(UIAnchor::UI_TOP_CENTER_HALF, glm::vec2(0, 0), glm::vec2(400, 150)));
	Text	*text_template = static_cast<Text*>(interface->addElement("text_template", new Text(UIAnchor::UI_TOP_CENTER_HALF, "template program", glm::vec2(0, 0), NULL, false)));

	text_template->setScale(glm::vec2(2, 2));

	text_popup->setRotation(glm::vec3(0.0, 0.0, 1.0));
	text_popup->setAngle(-10);
	text_popup->setColor(glm::vec3(1.0, 1.0, 0.0));

	interface->setUpdateFunc([]
		(Interface *interface)
		{
			static double	lastUpdate = 0;
			Text		*text_popup = static_cast<Text*>(interface->getElement("text_popup"));

			if (glfwGetTime() - lastUpdate >= TITLE_TIME)
			{
				text_popup->setText(popupTitles[rand() % TITLES_COUNT]);
				lastUpdate = glfwGetTime();
			}

			float	scale = 1 + std::abs(cos(glfwGetTime() * 5)) / 10;

			text_popup->setScale(glm::vec2(scale, scale));
		});
}

static void	_buildOptionsInterface(Interface *interface)
{
	interface->addElement("button_leave", new Button(UIAnchor::UI_BOTTOM_CENTER, "leave", glm::vec2(0, -10), glm::vec2(200, 60), []
		(ButtonInfo)
		{
			SCENE_MANAGER->get("title_scene")->getInterfaceManager()->use("main");
		}, NULL));

	interface->addElement("fun_text", new Text(UIAnchor::UI_CENTER, "why are you here? ... there are no options...", glm::vec2(0, 0), NULL, false));

	interface->setUpdateFunc([]
		(Interface *interface)
		{
			Text		*text_popup = static_cast<Text*>(interface->getElement("fun_text"));

			text_popup->setColor(glm::vec3(1.0, 0.2, 0.2));
			text_popup->setRotation(glm::vec3(0.0, 0.0, 1.0));
			text_popup->setAngle(cos(glfwGetTime() * 25));
		});
}

static void	_buildInterface(Scene *scene)
{
	InterfaceManager	*manager = scene->getInterfaceManager();

	Interface	*main = manager->load("main");
	_buildMainInterface(main);

	Interface	*options = manager->load("options");
	_buildOptionsInterface(options);

	Interface	*leaving = manager->load("leaving");
	leaving->addElement("leaving", new Text(UIAnchor::UI_CENTER, "joining world...", glm::vec2(0, 0), NULL, false));
}

static void	_frameKeyHook(Scene *scene)
{
	(void)scene;
}

static void	_updateShaders(ShaderManager *shaders)
{
	Shader	*textShader = shaders->get("text");

	textShader->use();
	textShader->setFloat("time", glfwGetTime());
	textShader->setFloat("SCREEN_WIDTH", SCREEN_WIDTH);
	textShader->setFloat("SCREEN_HEIGHT", SCREEN_HEIGHT);
}

static void	_keyHookFunc(Scene *scene, int key, int action)
{
	scene->getInterfaceManager()->getCurrent()->specialInput(key, action);
}

static void	_charHookFunc(Scene *scene, uint key)
{
	scene->getInterfaceManager()->getCurrent()->charInput(key);
}

void	TitleScreen::build(Scene *scene)
{
	_buildInterface(scene);
	scene->getCamera()->pos = glm::vec3(0, 0, 0);
	scene->getCamera()->pitch = 0;
	scene->getCamera()->yaw = 0;
	scene->setKeyHook(_keyHookFunc);
	scene->setCharHook(_charHookFunc);
	scene->getInterfaceManager()->use("main");
}

void	TitleScreen::destructor(Scene *scene)
{
	(void)scene;
}

void	TitleScreen::render(Scene *scene)
{
    glDisable(GL_DEPTH_TEST);
	FrameBuffer::drawFrame(SHADER_MANAGER->get("title_bg"), TEXTURE_MANAGER->get(DIRT_TEXTURE_PATH)->getID());
	scene->getInterfaceManager()->draw();
    glEnable(GL_DEPTH_TEST);
}

void	TitleScreen::update(Scene *scene)
{
	_frameKeyHook(scene);
	scene->getInterfaceManager()->update();
	_updateShaders(SHADER_MANAGER);
}

void	TitleScreen::close(Scene *scene)
{
	(void)scene;
}

void	TitleScreen::open(Scene *scene)
{
	(void)scene;
	scene->getInterfaceManager()->use("main");
}
