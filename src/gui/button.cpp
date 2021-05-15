#include "button.hpp"

extern C2D_SpriteSheet spriteSheet;

Button::Button(float x, float y, int imageid)  : m_LocationX(x), m_LocationY(y), m_imageid(imageid)
{
	C2D_Image img = C2D_SpriteSheetGetImage(spriteSheet, m_imageid);
	m_Width = img.subtex->width;
	m_Height = img.subtex->height;
}

Button::~Button()
{
}

void Button::SetSize(float w, float h)
{
	m_Width = w;
	m_Height = h;
}

void Button::SetLocation(float x, float y)
{
	m_LocationX = x;
	m_LocationY = y;
}

void Button::SetIcon(int imageid)
{
	m_imageid = imageid;
}

bool Button::Update(int clickX, int clickY)
{
	if ((clickX > m_LocationX) && (clickY > m_LocationY) && (clickX < m_LocationX + m_Width) && (clickY < m_LocationY + m_Height))
		return true;

	return false;
}

void Button::Draw(void)
{
	Gui::DrawImage(m_imageid, m_LocationX, m_LocationY);
}
