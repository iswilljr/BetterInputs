#include <Geode/DefaultInclude.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/utils/Keyboard.hpp>

#include "BetterTextInputNode.hpp"

#include "utils.hpp"

// Win64 bindings inline CCEGLView::onGLFWKeyCallback/onGLFWMouseCallBack, so keyboard
// and mouse use Geode's KeyboardInputEvent/MouseInputEvent. Char input still goes through
// onGLFWCharCallback, which we can hook to block ctrl+letter from inserting text.

using namespace geode::prelude;

namespace
{
	bool isCtrl(KeyboardModifier mods)
	{
		return (mods & KeyboardModifier::Control)
			|| BI::platform::keyDown(BI::PlatformKey::LEFT_CONTROL);
	}

	bool isShift(KeyboardModifier mods)
	{
		return (mods & KeyboardModifier::Shift)
			|| BI::platform::keyDown(BI::PlatformKey::LEFT_SHIFT);
	}

	bool hasShortcut(KeyboardModifier mods)
	{
		return isCtrl(mods)
			|| (mods & KeyboardModifier::Alt)
			|| BI::platform::keyDown(BI::PlatformKey::LEFT_ALT);
	}

	ListenerResult handleKey(KeyboardInputData& data)
	{
		if (!g_selectedInput)
			return ListenerResult::Propagate;

		if (data.action == KeyboardInputData::Action::Release)
			return ListenerResult::Propagate;

		auto const key = data.key;
		auto const mods = data.modifiers;

		// on click, can be held
		if (!isCtrl(mods) && !isShift(mods))
		{
			switch (key)
			{
				case KEY_Escape:
					if (!BI::geode::get<bool>("alternate-deselect"))
					{
						g_selectedInput->deselectInput();
						return ListenerResult::Stop;
					}
					break;

				case KEY_Backspace:
				case KEY_Delete:
					g_selectedInput->onDelete(false, key == KEY_Delete);
					return ListenerResult::Stop;

				default:
					break;
			}
		}

		switch (key)
		{
			case KEY_Up:
				g_selectedInput->onUpArrowKey(isShift(mods));
				return ListenerResult::Stop;

			case KEY_Down:
				g_selectedInput->onDownArrowKey(isShift(mods));
				return ListenerResult::Stop;

			case KEY_Right:
				g_selectedInput->onRightArrowKey(isCtrl(mods), isShift(mods));
				return ListenerResult::Stop;

			case KEY_Left:
				g_selectedInput->onLeftArrowKey(isCtrl(mods), isShift(mods));
				return ListenerResult::Stop;

			default:
				break;
		}

		if (data.action == KeyboardInputData::Action::Press && isCtrl(mods))
		{
			switch (key)
			{
				case KEY_A:
					g_selectedInput->highlightFromToPos(0, -1);
					return ListenerResult::Stop;

				case KEY_Insert:
				case KEY_C:
					g_selectedInput->onCopy();
					return ListenerResult::Stop;

				case KEY_V:
					g_selectedInput->onPaste();
					return ListenerResult::Stop;

				case KEY_X:
					g_selectedInput->onCut();
					return ListenerResult::Stop;

				case KEY_Backspace:
				case KEY_Delete:
					g_selectedInput->onDelete(true, key == KEY_Delete);
					return ListenerResult::Stop;

				default:
					break;
			}

			// swallow ctrl/alt combos that weren't handled above so IME doesn't insert the key
			return ListenerResult::Stop;
		}

		if (data.action == KeyboardInputData::Action::Press)
		{
			if (!isCtrl(mods))
			{
				switch (key)
				{
					case KEY_Home:
						g_selectedInput->onHomeKey(isShift(mods));
						return ListenerResult::Stop;

					case KEY_End:
						g_selectedInput->onEndKey(isShift(mods));
						return ListenerResult::Stop;

					default:
						break;
				}
			}

			if (isShift(mods) && !isCtrl(mods))
			{
				switch (key)
				{
					case KEY_Insert:
						g_selectedInput->onPaste();
						return ListenerResult::Stop;

					default:
						break;
				}
			}

			if (hasShortcut(mods))
				return ListenerResult::Stop;
		}

		return ListenerResult::Propagate;
	}

	ListenerResult handleMouse(MouseInputData& data)
	{
		if (
			g_selectedInput &&
			data.button == MouseInputData::Button::Left &&
			data.action == MouseInputData::Action::Press
		) {
			if (!BI::cocos::isTouchOnInput(g_selectedInput, BI::cocos::getTouchLocation()))
				g_selectedInput->deselectInput();
		}

		return ListenerResult::Propagate;
	}
}

struct BetterCCEGLViewChar : Modify<BetterCCEGLViewChar, CCEGLView>
{
	void onGLFWCharCallback(GLFWwindow* window, unsigned int c)
	{
		if (g_selectedInput && BI::platform::hasShortcutModifier())
			return;

		CCEGLView::onGLFWCharCallback(window, c);
	}
};

$execute
{
	KeyboardInputEvent().listen(handleKey, -100).leak();
	MouseInputEvent().listen(handleMouse).leak();
}
