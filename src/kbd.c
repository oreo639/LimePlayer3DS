#include <stdio.h>
#include <3ds.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"

//some setup for the keyboard.
static SwkbdState swkbd;
SwkbdButton button = SWKBD_BUTTON_NONE;

//Will be removed at further notice. Not such a great idea.
void getTextFromKbd(const char* hint, char* strBuff, int strSize) {
			char *oldStr = strdup(strBuff);
			swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, strSize);
			swkbdSetInitialText(&swkbd, strBuff);
			swkbdSetHintText(&swkbd, hint);
			swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "cancel", false);
			//swkbdSetButton(&swkbd, SWKBD_BUTTON_MIDDLE, "~Middle~", true);
			swkbdSetButton(&swkbd, SWKBD_BUTTON_MIDDLE, "save", true);
			swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT);
			SwkbdDictWord words[2];
			swkbdSetDictWord(&words[0], "defaultpath", "/3ds/limeplayer3ds/dugguspat/wildmidi.cfg");
			swkbdSetDictWord(&words[1], "limeplayer", "/3ds/limeplayer3ds");
			swkbdSetDictionary(&swkbd, words, sizeof(words)/sizeof(SwkbdDictWord));
			button = swkbdInputText(&swkbd, strBuff, strSize);
			if (button == SWKBD_BUTTON_LEFT) {
				sprintf(strBuff, "%s", oldStr); 	//When you cancel normaly, it clears the whole buffer, 
									//so this coppies it back.
			}
			free(oldStr);
			DEBUG_printf(strBuff);
}
