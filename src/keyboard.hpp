#pragma once

#include <Geode/utils/Keyboard.hpp>

#include "BetterTextInputNode.hpp"
#include "utils.hpp"

namespace BI::keyboard
{
	inline bool handleInput(geode::KeyboardInputData const& data)
	{
		if (!g_selectedInput)
			return false;

		using geode::KeyboardModifier;
		using geode::KeyboardInputData;
		using namespace cocos2d;

		bool const isCtrl = data.modifiers & KeyboardModifier::Control;
		bool const isShift = data.modifiers & KeyboardModifier::Shift;

		if (data.action != KeyboardInputData::Action::Release)
		{
			if (!isCtrl && !isShift)
			{
				switch (data.key)
				{
					case KEY_Escape:
						if (!BI::geode::get<bool>("alternate-deselect"))
							g_selectedInput->deselectInput();
						return true;

					case KEY_Backspace:
					case KEY_Delete:
						g_selectedInput->onDelete(false, data.key == KEY_Delete);
						return true;

					default:
						break;
				}
			}

			switch (data.key)
			{
				case KEY_Up:
					g_selectedInput->onUpArrowKey(isShift);
					return true;

				case KEY_Down:
					g_selectedInput->onDownArrowKey(isShift);
					return true;

				case KEY_Right:
					g_selectedInput->onRightArrowKey(isCtrl, isShift);
					return true;

				case KEY_Left:
					g_selectedInput->onLeftArrowKey(isCtrl, isShift);
					return true;

				default:
					break;
			}
		}

		if (data.action == KeyboardInputData::Action::Press && isCtrl && !isShift)
		{
			switch (data.key)
			{
				case KEY_A:
					g_selectedInput->highlightFromToPos(0, -1);
					return true;

				case KEY_Insert:
				case KEY_C:
					g_selectedInput->onCopy();
					return true;

				case KEY_V:
					g_selectedInput->onPaste();
					return true;

				case KEY_X:
					g_selectedInput->onCut();
					return true;

				case KEY_Backspace:
				case KEY_Delete:
					g_selectedInput->onDelete(true, data.key == KEY_Delete);
					return true;

				default:
					break;
			}
		}

		if (data.action == KeyboardInputData::Action::Press)
		{
			if (!isCtrl)
			{
				switch (data.key)
				{
					case KEY_Home:
						g_selectedInput->onHomeKey(isShift);
						return true;

					case KEY_End:
						g_selectedInput->onEndKey(isShift);
						return true;

					default:
						break;
				}
			}

			if (isShift && !isCtrl && data.key == KEY_Insert)
			{
				g_selectedInput->onPaste();
				return true;
			}
		}

		return false;
	}
}
