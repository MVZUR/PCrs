/*
 * pcrs_icons.c
 *
 *  Created on: Sep 25, 2025
 *      Author: mmazur
 */

#include "main.h"

void menu_home_lightbulb_A() {
	// letter A
	ssd1306_DrawPixel(83,6,Black);
	ssd1306_DrawPixel(82,7,Black);
	ssd1306_DrawPixel(82,8,Black);
	ssd1306_DrawPixel(82,9,Black);
	ssd1306_DrawPixel(83,8,Black);
	ssd1306_DrawPixel(84,7,Black);
	ssd1306_DrawPixel(84,8,Black);
	ssd1306_DrawPixel(84,9,Black);
}

void menu_home_lightbulb() {
	// outline
	// left
	ssd1306_DrawPixel(82,3,Black);
	ssd1306_DrawPixel(81,3,Black);
	ssd1306_DrawPixel(80,4,Black);
	ssd1306_DrawPixel(79,5,Black);
	ssd1306_DrawPixel(79,6,Black);
	ssd1306_DrawPixel(78,7,Black);
	ssd1306_DrawPixel(78,8,Black);
	ssd1306_DrawPixel(79,9,Black);
	ssd1306_DrawPixel(79,10,Black);
	ssd1306_DrawPixel(80,11,Black);
	ssd1306_DrawPixel(81,12,Black);
	ssd1306_DrawPixel(81,13,Black);
	ssd1306_DrawPixel(81,14,Black);
	ssd1306_DrawPixel(81,15,Black);
	ssd1306_DrawPixel(82,15,Black);

	ssd1306_DrawPixel(83,3,Black);

	// right
	ssd1306_DrawPixel(84,3,Black);
	ssd1306_DrawPixel(85,3,Black);
	ssd1306_DrawPixel(86,4,Black);
	ssd1306_DrawPixel(87,5,Black);
	ssd1306_DrawPixel(87,6,Black);
	ssd1306_DrawPixel(88,7,Black);
	ssd1306_DrawPixel(88,8,Black);
	ssd1306_DrawPixel(87,9,Black);
	ssd1306_DrawPixel(87,10,Black);
	ssd1306_DrawPixel(86,11,Black);
	ssd1306_DrawPixel(85,12,Black);
	ssd1306_DrawPixel(85,13,Black);
	ssd1306_DrawPixel(85,14,Black);
	ssd1306_DrawPixel(85,15,Black);
	ssd1306_DrawPixel(84,15,Black);

	ssd1306_DrawPixel(83,15,Black);

	//screw thread
	ssd1306_Line(81,13,85,13,Black);
}


void menu_lights_lightbulb() {
	//inline
	ssd1306_DrawArc(80,32,10,90,270,Black);
	ssd1306_DrawPixel(72,33,Black);
	ssd1306_DrawPixel(72,34,Black);
	ssd1306_DrawPixel(73,35,Black);
	ssd1306_DrawPixel(73,36,Black);
	ssd1306_DrawPixel(74,37,Black);
	ssd1306_DrawPixel(75,38,Black);
	ssd1306_DrawPixel(76,39,Black);
	ssd1306_Line(77,40,77,50,Black);
	ssd1306_Line(77,50,83,50,Black);
	ssd1306_Line(83,50,83,40,Black);
	ssd1306_DrawPixel(88,33,Black);
	ssd1306_DrawPixel(88,34,Black);
	ssd1306_DrawPixel(87,35,Black);
	ssd1306_DrawPixel(87,36,Black);
	ssd1306_DrawPixel(86,37,Black);
	ssd1306_DrawPixel(85,38,Black);
	ssd1306_DrawPixel(84,39,Black);

	//outline
	ssd1306_DrawArc(80,32,12,90,270,Black);
	ssd1306_DrawPixel(70,33,Black);
	ssd1306_DrawPixel(70,34,Black);
	ssd1306_DrawPixel(71,35,Black);
	ssd1306_DrawPixel(71,36,Black);
	ssd1306_DrawPixel(72,37,Black);
	ssd1306_DrawPixel(73,38,Black);
	ssd1306_DrawPixel(74,39,Black);
	ssd1306_Line(75,40,75,52,Black);
	ssd1306_Line(75,52,85,52,Black);
	ssd1306_Line(85,52,85,40,Black);
	ssd1306_DrawPixel(90,33,Black);
	ssd1306_DrawPixel(90,34,Black);
	ssd1306_DrawPixel(89,35,Black);
	ssd1306_DrawPixel(89,36,Black);
	ssd1306_DrawPixel(88,37,Black);
	ssd1306_DrawPixel(87,38,Black);
	ssd1306_DrawPixel(86,39,Black);

	//screw thread
	ssd1306_Line(75,48,85,46,Black);
	ssd1306_Line(75,44,85,42,Black);
}


void menu_stand_sidestand() {

	//stand
	ssd1306_Line(77,27,95,52,Black);
	ssd1306_Line(95,52,98,54,Black);
	ssd1306_Line(98,54,102,54,Black);
	ssd1306_Line(82,27,98,50,Black);
	ssd1306_Line(98,50,101,52,Black);
	ssd1306_Line(101,52,102,53,Black);

	//mounting
	ssd1306_FillCircle(81,28,4,Black);
	ssd1306_FillCircle(81,28,3,White);
}

void menu_home_sidestand() {
	ssd1306_DrawPixel(97,3,Black);
	ssd1306_DrawPixel(97,4,Black);
	ssd1306_DrawPixel(98,5,Black);
	ssd1306_DrawPixel(98,6,Black);
	ssd1306_DrawPixel(99,7,Black);
	ssd1306_DrawPixel(99,8,Black);
	ssd1306_DrawPixel(100,9,Black);
	ssd1306_DrawPixel(100,10,Black);
	ssd1306_DrawPixel(101,11,Black);
	ssd1306_DrawPixel(101,12,Black);
	ssd1306_DrawPixel(102,13,Black);
	ssd1306_DrawPixel(102,14,Black);
	ssd1306_DrawPixel(103,15,Black);
	ssd1306_DrawPixel(104,15,Black);

	ssd1306_DrawPixel(96,3,Black);
	ssd1306_DrawPixel(96,4,Black);
	ssd1306_DrawPixel(97,5,Black);
	ssd1306_DrawPixel(97,6,Black);
	ssd1306_DrawPixel(98,7,Black);
	ssd1306_DrawPixel(98,8,Black);
	ssd1306_DrawPixel(99,9,Black);
	ssd1306_DrawPixel(99,10,Black);
	ssd1306_DrawPixel(100,11,Black);
	ssd1306_DrawPixel(100,12,Black);
	ssd1306_DrawPixel(101,13,Black);
	ssd1306_DrawPixel(101,14,Black);
	ssd1306_DrawPixel(102,15,Black);
	ssd1306_DrawPixel(103,15,Black);
}

void menu_stand_sidestand_cross() {
	ssd1306_Line(74,22,107,55,Black);
	ssd1306_Line(73,22,106,55,Black);

	ssd1306_Line(74,55,107,22,Black);
	ssd1306_Line(73,55,106,22,Black);
}

void menu_home_sidestand_cross() {
	ssd1306_Line(94,3,106,15,Black);
	ssd1306_Line(94,15,106,3,Black);
}


void menu_home_hazardlights_triangle() {
	// outline
	ssd1306_Line(110,15,116,3,Black);
	ssd1306_Line(117,4,122,15,Black);
	ssd1306_Line(110,15,121,15,Black);
	// inline
	ssd1306_Line(111,13,116,4,Black);
	ssd1306_Line(116,4,121,13,Black);
	ssd1306_DrawPixel(111,14,Black);
	ssd1306_DrawPixel(121,14,Black);

	// triangle inside
	ssd1306_DrawPixel(116,9,Black);

	ssd1306_DrawPixel(115,10,Black);
	ssd1306_DrawPixel(115,11,Black);
	ssd1306_DrawPixel(114,12,Black);
	ssd1306_DrawPixel(114,13,Black);
	ssd1306_DrawPixel(115,13,Black);

	ssd1306_DrawPixel(117,10,Black);
	ssd1306_DrawPixel(117,11,Black);
	ssd1306_DrawPixel(118,12,Black);
	ssd1306_DrawPixel(118,13,Black);
	ssd1306_DrawPixel(117,13,Black);

	ssd1306_DrawPixel(116,13,Black);
}



