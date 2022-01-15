#include "Button.hpp"

CST_Color Button::colors[2] = {
	{ 0x00, 0x00, 0x00, 0xff }, // light
	{ 0xff, 0xff, 0xff, 0xff }, // dark
};

Button::Button(const char* message, int button, bool dark, int size, int width)
	: physical(button)
	, dark(dark)
	, icon(getUnicode(button), (size / SCALER) * 1.25, &colors[dark], ICON)
	, text(message, (size / SCALER), &colors[dark])
{

	super::append(&text);
	super::append(&icon);

	fixedWidth = width;

	updateBounds();

	this->touchable = true;
	this->hasBackground = true;

	// protect "stack" children
	text.isProtected = icon.isProtected = true;

	if (dark)
	{
		backgroundColor = (rgb){ 0x67/255.0, 0x6a/255.0, 0x6d/255.0 };
#if defined(__WIIU__)
		backgroundColor = (rgb){ 0x3b/255.0, 0x3c/255.0, 0x4e/255.0 };
#endif
	}
	else
		backgroundColor = (rgb){ 0xee/255.0, 0xee/255.0, 0xee/255.0 };
}

void Button::updateBounds()
{
	int PADDING = 10 / SCALER;

	int bWidth = PADDING * 0.5 * (icon.width != 0); // gap space between button

	text.position(PADDING * 2 + bWidth + icon.width, PADDING);
	icon.position(PADDING * 1.7, PADDING + (text.height - icon.height) / 2);

	this->width = (fixedWidth > 0) ? fixedWidth : text.width + PADDING * 4 + bWidth + icon.width;
	this->height = text.height + PADDING * 2;
}

void Button::updateText(const char* inc_text)
{
	this->text.setText(inc_text);
	this->text.update();
	updateBounds();
}

const char* Button::getUnicode(int button)
{
	switch (button)
	{
		case A_BUTTON:
			return "\ue0a0";
		case B_BUTTON:
			return "\ue0a1";
		case Y_BUTTON:
			return "\ue0a2";
		case X_BUTTON:
			return "\ue0a3";
		case START_BUTTON:
			return "\ue0a4";
		case SELECT_BUTTON:
			return "\ue0a5";
		case L_BUTTON:
			return "\ue0a6";
		case R_BUTTON:
			return "\ue0a7";
		case ZL_BUTTON:
			return "\ue0a8";
		case ZR_BUTTON:
			return "\ue0a9";
		default:
			break;
	}
	return "";
}

bool Button::process(InputEvents* event)
{
	if (event->isKeyDown() && this->physical != 0 && event->held(this->physical))
	{
		// invoke our action, since we saw a physical button press that matches!
		this->action();
		return true;
	}

	return super::process(event);
}