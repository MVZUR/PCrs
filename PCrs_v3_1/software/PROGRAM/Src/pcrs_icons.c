/*
 * pcrs_icons.c
 *
 *  Created on: Sep 25, 2025
 *      Author: mmazur
 */

#include "main.h"


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
