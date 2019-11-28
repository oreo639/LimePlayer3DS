#include "progressbar.hpp"

ProgressBar::ProgressBar(float x, float y, float w, float h, uint32_t color) : locationX(x), locationY(y), width(w), height(h), color(color)
{

}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::UpdateColor(uint32_t color)
{
	this->color = color;
}

void ProgressBar::UpdateSize(float x, float y)
{
	this->width = x;
	this->height = y;
}

void ProgressBar::UpdateLocation(float x, float y)
{
	this->locationX = x;
	this->locationY = y;
}

void ProgressBar::UpdateProgress(float percent)
{
	if (!(percent < 0))
		this->progress = percent;
}

int ProgressBar::SeekByClick(uint32_t clickX, uint32_t clickY)
{
	DEBUG("Click was triggered x:%d, y:%d\n", clickX, clickY);
	DEBUG("|| %d\n", (int)(((clickX-locationX)/width) * 100));
	if ((clickX > locationX) && (clickY > locationY) && (clickX < locationX + width) && (clickY < locationY + height)) {
		return (int)(((clickX-locationX)/width) * 100);
	}

	return -1;
}

void ProgressBar::Draw(void)
{
	Gui::DrawSolidRectangle(locationX, locationY,  width, height, C2D_Color32(0xC9, 0xCA, 0xDD, 0xFF));
	Gui::DrawSolidRectangle(locationX+1, locationY+1, (width-2) * progress, height-2, color);
}
