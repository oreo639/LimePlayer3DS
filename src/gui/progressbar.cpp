#include "progressbar.hpp"
#include "color.hpp"

ProgressBar::ProgressBar(float x, float y, float w, float h, uint32_t color) : m_LocationX(x), m_LocationY(y), m_Width(w), m_Height(h), m_Color(color)
{

}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::UpdateColor(uint32_t color)
{
	m_Color = color;
}

void ProgressBar::UpdateSize(float w, float h)
{
	m_Width = w;
	m_Height = h;
}

void ProgressBar::UpdateLocation(float x, float y)
{
	m_LocationX = x;
	m_LocationY = y;
}

void ProgressBar::UpdateProgress(int percent)
{
	if (percent >= -1)
		m_Progress = percent;
}

int ProgressBar::SeekByClick(uint32_t clickX, uint32_t clickY)
{
	if ((clickX > m_LocationX) && (clickY > m_LocationY) && (clickX < m_LocationX + m_Width) && (clickY < m_LocationY + m_Height))
		return (int)(((clickX-m_LocationX)/m_Width) * 100);

	return -1;
}

void ProgressBar::Draw(void)
{
	if (m_Progress >= 0) {
		Gui::DrawSolidRectangle(m_LocationX, m_LocationY, m_Width, m_Height, COLOR_GRAY);
		Gui::DrawSolidRectangle(m_LocationX+1, m_LocationY+1, (m_Width-2) * (m_Progress/100.0f), m_Height-2, m_Color);
	} else {
		Gui::DrawSolidRectangle(m_LocationX, m_LocationY, m_Width, m_Height, COLOR_DARKGRAY);
	}
}
