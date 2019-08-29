/*   LimePlayer3DS FOSS graphcal music player for the Nintendo 3DS.
*    Copyright (C) 2018-2019  LimePlayer Team
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "overlay.hpp"
#include "menu.hpp"
#include "gui.hpp"

Overlay::Overlay(Menu& screen) : me(screen.currentOverlay) {}

Overlay::Overlay(Overlay& overlay) : me(overlay.overlay) {}

void Overlay::dim() const
{
     Gui::DrawSolidRectangle(0, 0, 400, 240, C2D_Color32(0, 0, 0, 190));
}

void Overlay::doTopDraw() const
{
    if (overlay)
    {
        if (!overlay->willReplaceTop())
        {
            drawTop();
        }
        overlay->doTopDraw();
    }
    else
    {
        drawTop();
    }
}

void Overlay::doBottomDraw() const
{
    if (overlay)
    {
        if (!overlay->willReplaceBottom())
        {
            drawBottom();
        }
        overlay->doBottomDraw();
    }
    else
    {
        drawBottom();
    }
}

void Overlay::doUpdate(touchPosition* touch)
{
    if (overlay && overlay->willHandleUpdate())
    {
        overlay->doUpdate(touch);
    }
    else
    {
        update(touch);
    }
}

bool Overlay::willHandleUpdate() const
{
    if (overlay)
    {
        return overlay->willHandleUpdate();
    }
    else
    {
        return handlesUpdate();
    }
}

bool Overlay::willReplaceBottom() const
{
    if (overlay)
    {
        return overlay->willReplaceBottom();
    }
    else
    {
        return replacesBottom();
    }
}

bool Overlay::willReplaceTop() const
{
    if (overlay)
    {
        return overlay->willReplaceTop();
    }
    else
    {
        return replacesTop();
    }
}
