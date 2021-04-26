/*
* Lambda Menu
* (C) Oui 2017
* https://lambda.menu
*
* The trainer menu code was based on the Enhanced Native Trainer project.
* https://github.com/gtav-ent/GTAV-EnhancedNativeTrainer
* (C) Rob Pridham and fellow contributors 2015
*
* Some of this code began its life as a part of GTA V SCRIPT HOOK SDK.
* http://dev-c.com
* (C) Alexander Blade 2015
*/


#include "menu_functions.h"
#include "noclip.h"
#include "keyboard.h"
#include "config_io.h"
#include "script.h"
#include "colors.h"

bool exitFlag = false;

char* noclip_ANIM_A = "amb@world_human_stand_impatient@male@no_sign@base";
char* noclip_ANIM_B = "base";

int travelSpeed = 2;

int g_maxSpeedSettings = 7;

bool in_noclip_mode = false;

bool help_showing = true;

Vector3 curLocation;
Vector3 curRotation;
float curHeading;

//Converts Radians to Degrees
float deg_to_rad(float degs)
{
	return degs * 3.14159265358979323846 / 180;
}

std::string noclipStatusLines[15];

DWORD noclipStatusTextDrawTicksMax;
bool noclipStatusTextGxtEntry;

void exit_noclip_menu_if_showing()
{
	exitFlag = true;
}

void process_noclip_menu()
{
	exitFlag = false;

	const float lineWidth = 264.0;
	const int lineCount = 1;
	bool loadedAnims = false;

	Ped playerPed = PLAYER::PLAYER_PED_ID();
	bool inVehicle = PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0) ? true : false;

	if (!inVehicle)
	{
		STREAMING::REQUEST_ANIM_DICT(noclip_ANIM_A);
		while (!STREAMING::HAS_ANIM_DICT_LOADED(noclip_ANIM_A))
		{
			WAIT(0);
		}
		loadedAnims = true;
	}

	curLocation = ENTITY::GET_ENTITY_COORDS(playerPed, 0);
	curRotation = ENTITY::GET_ENTITY_ROTATION(playerPed, 0);
	curHeading = ENTITY::GET_ENTITY_HEADING(playerPed);

	while (true && !exitFlag)
	{
		in_noclip_mode = true;

		if (help_showing)
		{
			std::string caption = "~p~LAMBDA ~s~MENU  NOCLIP MODE";
			draw_menu_header_line(caption, 264.0f, 35.0f, 0.0f, 1016.0f, 1026.0f, false);
		}

		make_periodic_feature_call();


		if (ENTITY::IS_ENTITY_DEAD(playerPed))
		{
			exitFlag = true;
		}
		else if (noclip_switch_pressed())
		{
			menu_beep();
			break;
		}

		noclip(inVehicle);

		WAIT(0);
	}

	if (!inVehicle)
	{
		AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
		AUDIO::SET_USER_RADIO_CONTROL_ENABLED(1);
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), FALSE);
	}
	else
	{
		AUDIO::SET_USER_RADIO_CONTROL_ENABLED(1);
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), FALSE);
		Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		ENTITY::SET_ENTITY_INVINCIBLE(veh, FALSE);
	}

	exitFlag = false;
	in_noclip_mode = false;
}

void update_noclip_text()
{
	if (help_showing)
	{
		if (GetTickCount() < noclipStatusTextDrawTicksMax)
		{
			int numLines = sizeof(noclipStatusLines) / sizeof(noclipStatusLines[0]);

			float textY = 0.053f;

			int numActualLines = 0;
			for (int i = 0; i < numLines; i++)
			{

				numActualLines++;

				UI::SET_TEXT_FONT(0);
				UI::SET_TEXT_SCALE(0.332, 0.332);
				if (i == 0 || i == 8 || i == 14)
				{
					UI::SET_TEXT_COLOUR(255, 180, 0, 255);
				}
				else
				{
					UI::SET_TEXT_COLOUR(255, 255, 255, 255);
				}
				UI::SET_TEXT_WRAP(0.0, 1.0);
				UI::SET_TEXT_EDGE(1, 0, 0, 0, 305);
				if (noclipStatusTextGxtEntry)
				{
					UI::_SET_TEXT_ENTRY((char *)noclipStatusLines[i].c_str());
				}
				else
				{
					UI::_SET_TEXT_ENTRY("STRING");
					UI::_ADD_TEXT_COMPONENT_STRING((char *)noclipStatusLines[i].c_str());
				}
				UI::_DRAW_TEXT(0.802, textY); //increase to move right

				textY += 0.024f; //verticle spacing
			}

			int screen_w, screen_h;
			GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);
			float rectWidthScaled = 264 / (float)screen_w;
			float rectHeightScaled = (11 + (numActualLines * 9)) / (float)screen_h;
			float rectXScaled = 571.5 / (float)screen_h; //increase to move right
			float rectYScaled = 35 / (float)screen_h;

			int rect_col[4] = { 0, 0, 0, 50 };

			// rect
			draw_rect(rectXScaled, rectYScaled,
				rectWidthScaled, rectHeightScaled,
				rect_col[0], rect_col[1], rect_col[2], rect_col[3]);
		}
	}
}

void create_noclip_help_text()
{
	std::stringstream ss;

	std::string travelSpeedStr = "<C>Medium</C>";
	switch (travelSpeed)
	{
	case 0:
		travelSpeedStr = "<C>Very Slow</C>";
		break;
	case 1:
		travelSpeedStr = "<C>Slow</C>";
		break;
	case 3:
		travelSpeedStr = "<C>Fast</C>";
		break;
	case 4:
		travelSpeedStr = "<C>Very Fast</C>";
		break;
	case 5:
		travelSpeedStr = "<C>Extremely Fast</C>";
		break;
	case 6:
		travelSpeedStr = "<C>~r~Fast as fuck!</C>";
		break;
	case 7:
		travelSpeedStr = "<C>~r~Faster than fast as fuck, this shit is so fucking fast!";
		break;

	default:
		travelSpeedStr = "<C>Medium</C>";
		break;
	}

	ss << "~w~Current Travel Speed: ~p~" << travelSpeedStr;

	int index = 0;
	noclipStatusLines[index++] = "~w~H ~HUD_COLOUR_BLACK~- ~w~Hide ~HUD_COLOUR_BLACK~/ ~w~Show NoClip Menu";
	noclipStatusLines[index++] = " ";
	noclipStatusLines[index++] = "~w~Q ~HUD_COLOUR_BLACK~/ ~w~Z ~HUD_COLOUR_BLACK~- ~w~Move Up ~HUD_COLOUR_BLACK~/ ~w~Down";
	noclipStatusLines[index++] = "~w~A ~HUD_COLOUR_BLACK~/ ~w~D ~HUD_COLOUR_BLACK~- ~w~Rotate Left ~HUD_COLOUR_BLACK~/ ~w~Right";
	noclipStatusLines[index++] = "~w~W ~HUD_COLOUR_BLACK~/ ~w~S ~HUD_COLOUR_BLACK~- ~w~Move Forward ~HUD_COLOUR_BLACK~/ ~w~Back";
	noclipStatusLines[index++] = "~w~Shift ~HUD_COLOUR_BLACK~- ~w~Toggle Move Speed";
	noclipStatusLines[index++] = " ";
	noclipStatusLines[index++] = ss.str();

	noclipStatusTextDrawTicksMax = GetTickCount() + 2500;
	noclipStatusTextGxtEntry = false;
}

void moveThroughDoor()
{
	// common variables
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		return;
	}

	curLocation = ENTITY::GET_ENTITY_COORDS(playerPed, 0);
	curHeading = ENTITY::GET_ENTITY_HEADING(playerPed);

	float forwardPush = 0.6;

	float xVect = forwardPush * sin(deg_to_rad(curHeading)) * -1.0f;
	float yVect = forwardPush * cos(deg_to_rad(curHeading));

	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, curLocation.x + xVect, curLocation.y + yVect, curLocation.z, 1, 1, 1);
}

bool lshiftWasDown = false;

void noclip(bool inVehicle)
{
	// common variables
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(playerPed);

	float rotationSpeed = 2.5;
	float forwardPush;

	switch (travelSpeed)
	{
	case 0:
		forwardPush = 0.05f; // very slow
		break;
	case 1:
		forwardPush = 0.2f; // slow
		break;
	case 2:
		forwardPush = 0.8f; // medium
		break;
	case 3:
		forwardPush = 1.8f; // fast
		break;
	case 4:
		forwardPush = 3.6f; // very fast
		break;
	case 5:
		forwardPush = 5.4f; // extremely fast
		break;
	case 6:
		forwardPush = 10.0f; // fast as fuck
		break;
	case 7:
		forwardPush = 30.0f; // faster than fast as fuck
		break;
	}

	float xVect = forwardPush * sin(deg_to_rad(curHeading)) * -1.0f;
	float yVect = forwardPush * cos(deg_to_rad(curHeading));

	KeyInputConfig* keyConfig = get_config()->get_key_config();

	bool moveUpKey = get_key_pressed(keyConfig->key_noclip_up);
	bool moveDownKey = get_key_pressed(keyConfig->key_noclip_down);
	bool moveForwardKey = get_key_pressed(keyConfig->key_noclip_forward);
	bool moveBackKey = get_key_pressed(keyConfig->key_noclip_back);
	bool rotateLeftKey = get_key_pressed(keyConfig->key_noclip_rotate_left);
	bool rotateRightKey = get_key_pressed(keyConfig->key_noclip_rotate_right);

	//noclip controls vehicle if occupied
	Entity target = playerPed;
	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		target = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	}

	ENTITY::SET_ENTITY_VELOCITY(playerPed, 0.0f, 0.0f, 0.0f);
	ENTITY::SET_ENTITY_ROTATION(playerPed, 0, 0, 0, 0, false);

	if (!inVehicle)
	{
		AI::TASK_PLAY_ANIM(PLAYER::PLAYER_PED_ID(), noclip_ANIM_A, noclip_ANIM_B, 8.0f, 0.0f, -1, 9, 0, 0, 0, 0);
		AUDIO::SET_USER_RADIO_CONTROL_ENABLED(0);
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), TRUE);
	}

	else
	{
		AUDIO::SET_USER_RADIO_CONTROL_ENABLED(0);
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), TRUE);
		Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		ENTITY::SET_ENTITY_INVINCIBLE(veh, TRUE);
	}

	if (IsKeyJustUp(keyConfig->key_noclip_speed))
	{
		travelSpeed++;
		if (travelSpeed > g_maxSpeedSettings)
		{
			travelSpeed = 0;
		}
	}

	if (IsKeyJustUp(VK_KEY_H))
	{
		help_showing = !help_showing;
	}

	create_noclip_help_text();
	update_noclip_text();

	if (moveUpKey)
	{
		curLocation.z += forwardPush / 2;
	}
	else if (moveDownKey)
	{
		curLocation.z -= forwardPush / 2;
	}

	if (moveForwardKey)
	{
		curLocation.x += xVect;
		curLocation.y += yVect;
	}
	else if (moveBackKey)
	{
		curLocation.x -= xVect;
		curLocation.y -= yVect;
	}

	if (rotateLeftKey)
	{
		curHeading += rotationSpeed;
	}
	else if (rotateRightKey)
	{
		curHeading -= rotationSpeed;
	}

	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(target, curLocation.x, curLocation.y, curLocation.z, 1, 1, 1);
	ENTITY::SET_ENTITY_HEADING(target, curHeading - rotationSpeed);
}

bool is_in_noclip_mode()
{
	return in_noclip_mode;
}
