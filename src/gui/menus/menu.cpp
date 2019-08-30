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
#include "menu.hpp"

void Menu::doTopDraw() const
{
	if (currentOverlay)
	{
		if (!currentOverlay->replacesTop())
			drawTop();

		currentOverlay->doTopDraw();
	} else
		drawTop();
}

void Menu::doBottomDraw() const
{
	if (currentOverlay)
	{
		if (!currentOverlay->replacesBottom())
			drawBottom();

		currentOverlay->doBottomDraw();
	} else
		drawBottom();
}

void Menu::doUpdate(touchPosition* touch)
{
	if (currentOverlay && currentOverlay->willHandleUpdate())
		currentOverlay->doUpdate(touch);
	else
		update(touch);
}
