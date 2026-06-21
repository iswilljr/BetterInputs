#include <Geode/modify/CCIMEDispatcher.hpp>

#include "BetterTextInputNode.hpp"

// Backspace and delete are handled in dispatchDeleteBackward/dispatchDeleteForward, then in
// CCEGLView::onGLFWKeyCallback/[EAGLView keyDownExec:]/KeyboardInputEvent on Windows.
// Same goes for navigation keys and ctrl shortcuts in those platform handlers.
struct BetterCCIMEDispatcher : geode::Modify<BetterCCIMEDispatcher, cocos2d::CCIMEDispatcher>
{
	void dispatchDeleteBackward()
	{
		if (g_selectedInput)
		{
			g_selectedInput->onDelete(false, false);
			return;
		}

		CCIMEDispatcher::dispatchDeleteBackward();
	}

	void dispatchDeleteForward()
	{
		if (g_selectedInput)
		{
			g_selectedInput->onDelete(false, true);
			return;
		}

		CCIMEDispatcher::dispatchDeleteForward();
	}

	void dispatchInsertText(const char* text, int len, cocos2d::enumKeyCodes keyCode)
	{
		if (g_selectedInput)
		{
#ifdef GEODE_IS_WINDOWS
			if (keyCode == cocos2d::enumKeyCodes::KEY_Left)
			{
				g_selectedInput->onLeftArrowKey(
					BI::platform::keyDown(BI::PlatformKey::LEFT_CONTROL),
					BI::platform::keyDown(BI::PlatformKey::LEFT_SHIFT)
				);
				return;
			}

			if (keyCode == cocos2d::enumKeyCodes::KEY_Right)
			{
				g_selectedInput->onRightArrowKey(
					BI::platform::keyDown(BI::PlatformKey::LEFT_CONTROL),
					BI::platform::keyDown(BI::PlatformKey::LEFT_SHIFT)
				);
				return;
			}
#endif

			g_selectedInput->insertCharAtPos(g_selectedInput->getCursorPos(), text[0]);
			return;
		}

		CCIMEDispatcher::dispatchInsertText(text, len, keyCode);
	}
};
