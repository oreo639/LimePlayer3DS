#include "touch.h"

int pretouch = false; //makes it so that you can't hold buttons unless specificly specified.

static bool touchSquare(int touchX, int touchY, int posX, int posY, int height, int width) {
	if(touchY > posY && touchY <  posY+height && touchX > posX && touchX < posX+width)
		return true;
	else
		return false;
}

int touch2Key(void) {
	touchPosition touch;
	hidTouchRead(&touch);
	
	
	
	if (touchSquare(touch.px, touch.py, 145, 203, 27, 37) && pretouch == false) 
	{
		pretouch = true;
		return TOUCH_BUTTON_PLAY; //Play/pause
	} 
	else if (touchSquare(touch.px, touch.py, 229, 203, 27, 37) && pretouch == false) 
	{
		pretouch = true;
		return TOUCH_BUTTON_SKPFW; //Skip foward
	} 
	else if (touchSquare(touch.px, touch.py, 58, 203, 27, 37) && pretouch == false) 
	{
		pretouch = true;
		return TOUCH_BUTTON_SKPBW; //Skip backwards
	} 
	else if (touchSquare(touch.px, touch.py, 100, 203, 27, 37) && pretouch == false) 
	{
		pretouch = true;
		return TOUCH_BUTTON_STOP; //Stop playback
	} 
	else if (!touch.py || !touch.px) 
	{
		pretouch = false;
	}

	return 0;
}
