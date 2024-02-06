#pragma once

#include "./graphics.hpp"

class MayaGraphicsGUI::Button : public MayaGraphicsGUI::Component
{
public:

	Button(MayaGraphicsGUI& gui);

	virtual void Draw() override;

	void ReactEvent(MayaEvent& e) override;

	void SetEventCallback(MayaFunctionCR<void()> callback);

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	bool IsButtonTouched() const;

	bool IsButtonPressed() const;

private:

	MayaFunction<void()> callback;
	MayaString text;
	bool pressed;
};