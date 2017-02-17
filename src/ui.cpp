/*
 * ui.cpp
 *
 *  Created on: Feb 17, 2017
 *      Author: romkal
 */
#include <menu.h>
#include <menuIO/u8g2Out.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>
#include "carriage.h"
using namespace Menu;

U8G2_SSD1309_128X64_NONAME_1_2ND_HW_I2C u8g2(&u8g2_cb_r0);

result showPattern(menuOut& o,idleEvent e)
{
	pattern->draw(u8g2);
	return result::quit;
}

result changePattern(menuOut& o,idleEvent e)
{
	return result::quit;
}


TOGGLE(patternProgression.invert, invertToogle, "Invert", doNothing,noEvent,wrapStyle,
		VALUE("Normal", false, doNothing, noEvent),
		VALUE("Inverted", true, doNothing, noEvent));

MENU(root, "Main menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
		OP("Show pattern", showPattern, Menu::enterEvent),
		OP("Change pattern", changePattern, Menu::enterEvent),
		FIELD(patternProgression.scale_x, "Scale X", "x", 1, 5, 1, 1, doNothing, noEvent, wrapStyle),
		FIELD(patternProgression.scale_y, "Scale Y", "x", 1, 5, 1, 1, doNothing, noEvent, wrapStyle),
		FIELD(patternProgression.repeat, "Repeat", "x", 1, 50, 5, 1, doNothing, noEvent, wrapStyle),
		SUBMENU(invertToogle),
		EXIT("<Back"));


#define fontName u8g2_font_5x7_tf
#define fontX 5
#define fontY 8
#define offsetX 32
#define offsetY 16
#define MAX_DEPTH 2
const colorDef<uint8_t> colors[] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,0},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{0,0},{1,1,1}},//titleColor
};
encoderIn<1, 2> encoder;
encoderInStream<1, 2> encoderStream(encoder, 1);
keyMap encBtn_map[]={{-3,options->getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys
MENU_INPUTS(in,&encoderStream, &encButton);

MENU_OUTPUTS(out,MAX_DEPTH
  ,U8G2_OUT(u8g2,colors,fontX,fontY,offsetX,offsetY,{0,0,64/fontX,48/fontY})
  ,NONE//must have 2 items at least
);

NAVROOT(nav, root, 1, in, out);
