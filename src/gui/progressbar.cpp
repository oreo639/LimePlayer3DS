#include "progressbar.hpp"
#include "color.hpp"

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

void ProgressBar::UpdateProgress(int percent)
{
	if (percent >= -1)
		progress = percent;
}

int ProgressBar::SeekByClick(uint32_t clickX, uint32_t clickY)
{
	if ((clickX > locationX) && (clickY > locationY) && (clickX < locationX + width) && (clickY < locationY + height)) {
		return (int)(((clickX-locationX)/width) * 100);
	}

	return -1;
}

void ProgressBar::Draw(void)
{
	if (progress >= 0) {
		Gui::DrawSolidRectangle(locationX, locationY,  width, height, COLOR_GRAY);
		Gui::DrawSolidRectangle(locationX+1, locationY+1, (width-2) * (progress/100.0f), height-2, color);
	} else
		Gui::DrawSolidRectangle(locationX, locationY,  width, height, COLOR_DARKGRAY);
}
