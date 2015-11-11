#pragma once

class ColorPickerElement : public MenuElement
{
public:
	typedef EventDispatcher<void(Color&, const uint32_t)> eColorChanged;
	virtual ~ColorPickerElement();
	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;

	struct Context
	{
		Color m_FillColor;
		Color m_FillColorMouseOver;
		Vector2f m_Position;
		Vector2f m_Size;

		struct SelectorWindowCtx
		{
			//Required
			Color m_TitleFillColor;
			Color m_FillColor;
			Vector2f m_Size;

			//Optional
			float m_TitleBarHeight;
			float m_BorderWidth;
			Color m_TextColor;
			SelectorWindowCtx()
			{
				m_TitleBarHeight = 25;
				m_BorderWidth = 5;
				m_TextColor = Color::Black();
			}
		};
		SelectorWindowCtx m_ColorSelectorInfo;
		Context()
		{

		}
	};
	ColorPickerElement(const Context& Ctx);
	eColorChanged& EventColorChanged();
protected:
	virtual void OnMouseDown(const MouseMessage& Msg) override;
	virtual void OnMouseUp(const MouseMessage& Msg) override;
	virtual void OnMouseMove(const MouseMessage& Msg) override;
	virtual bool IsPointInControl(const Vector2f& Point) override;
	virtual bool IsPointInMouseDownZone(const Vector2f& Point) override;
	virtual bool IsCursorInElement() override;
	virtual void AddPosition(const Vector2f& NewAmount) override;
	virtual void SetPosition(const Vector2f& NewPos) override;
	virtual void OnColorChanged(const int Value, uint32_t SliderId);
private:
	Context m_Ctx;
	bool m_IsMouseDown;
	bool m_IsColorSelectorOpen;

	WindowElement* m_ColorSelectorWindow;
	uint32_t m_AlphaSliderId;
	uint32_t m_RedSliderId;
	uint32_t m_GreenSliderId;
	uint32_t m_BlueSliderId;
	eColorChanged m_eColorChanged;
};

ColorPickerElement::ColorPickerElement(const Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
	m_IsMouseDown = false;
	m_IsColorSelectorOpen = false;

	WindowElement::Context WindowCtx;
	WindowCtx.m_WindowName = "Select Color";
	WindowCtx.m_FillColor = m_Ctx.m_ColorSelectorInfo.m_FillColor;
	WindowCtx.m_TitleFillColor = m_Ctx.m_ColorSelectorInfo.m_TitleFillColor;
	WindowCtx.m_TitleBarHeight = m_Ctx.m_ColorSelectorInfo.m_TitleBarHeight;
	WindowCtx.m_BorderWidth = m_Ctx.m_ColorSelectorInfo.m_BorderWidth;
	WindowCtx.m_TextColor = m_Ctx.m_ColorSelectorInfo.m_TextColor;
	WindowCtx.m_Position = Vector2f(m_Position.x, m_Position.y + m_Size.y);
	WindowCtx.m_Size = m_Ctx.m_ColorSelectorInfo.m_Size;
	m_ColorSelectorWindow = new WindowElement(WindowCtx);
	m_ColorSelectorWindow->EventClosePressed() +=
		[&](const uint32_t Id) {
		m_IsColorSelectorOpen = false;
	};

	SliderElement<int>::Context SliderCtx;
	SliderCtx.m_Color = Color::Black();
	SliderCtx.m_Min = 0;
	SliderCtx.m_Max = 255;
	SliderCtx.m_DefaultValue = m_Ctx.m_FillColor.R * 255;
	SliderCtx.m_Position = Vector2f(0, 5);
	SliderCtx.m_Size = Vector2f(100, 20);

	//Add the RGBA sliders, set event to OnColorChanged
	SliderElement<int>* RedSlider = new SliderElement<int>(SliderCtx);
	RedSlider->EventValueChanged() += std::bind(&ColorPickerElement::OnColorChanged, this,
		std::placeholders::_1, RedSlider->GetId());
	m_RedSliderId = m_ColorSelectorWindow->AddSubElement(RedSlider);

	SliderCtx.m_Position = Vector2f(0, 45);
	SliderCtx.m_DefaultValue = m_Ctx.m_FillColor.G * 255;
	SliderElement<int>* GreenSlider = new SliderElement<int>(SliderCtx);
	GreenSlider->EventValueChanged() += std::bind(&ColorPickerElement::OnColorChanged, this,
		std::placeholders::_1, GreenSlider->GetId());
	m_GreenSliderId = m_ColorSelectorWindow->AddSubElement(GreenSlider);

	SliderCtx.m_Position = Vector2f(0, 85);
	SliderCtx.m_DefaultValue = m_Ctx.m_FillColor.B * 255;
	SliderElement<int>* BlueSlider = new SliderElement<int>(SliderCtx);
	BlueSlider->EventValueChanged() += std::bind(&ColorPickerElement::OnColorChanged, this,
		std::placeholders::_1, BlueSlider->GetId());
	m_BlueSliderId = m_ColorSelectorWindow->AddSubElement(BlueSlider);

	SliderCtx.m_Position = Vector2f(0, 125);
	SliderCtx.m_DefaultValue = m_Ctx.m_FillColor.A * 255;
	SliderElement<int>* AlphaSlider = new SliderElement<int>(SliderCtx);
	AlphaSlider->EventValueChanged() += std::bind(&ColorPickerElement::OnColorChanged, this,
		std::placeholders::_1, AlphaSlider->GetId());
	m_AlphaSliderId = m_ColorSelectorWindow->AddSubElement(AlphaSlider);
}

ColorPickerElement::~ColorPickerElement()
{
	delete m_ColorSelectorWindow;
	m_ColorSelectorWindow = nullptr;
}

void ColorPickerElement::Draw(RenderInterface& Renderer)
{
	//We want user to view the color while clicking and moving mouse in color selector window
	Color PreviewColor = m_Ctx.m_FillColor;
	if (m_CursorInElement && !m_IsColorSelectorOpen)
		PreviewColor = m_Ctx.m_FillColorMouseOver;

	Renderer.BeginLine();
	Renderer.DrawFilledBox(m_Position, m_Size, PreviewColor);
	Renderer.DrawLineBox(m_Position, m_Size, Color::Black());
	Renderer.EndLine();
	if (!m_IsColorSelectorOpen)
		return;

	m_ColorSelectorWindow->Draw(Renderer);
}

ElementType ColorPickerElement::GetType()
{
	return ElementType::ColorPicker;
}

void ColorPickerElement::OnMouseDown(const MouseMessage& Msg)
{
	if (!m_IsColorSelectorOpen)
		MenuElement::OnMouseDown(Msg);

	if (Msg.GetButton() == MouseMessage::MouseButton::Left)
	{
		m_IsMouseDown = true;
		m_IsColorSelectorOpen = true;
		MenuElement::OnZOrderChanged();
	}
	if (m_IsColorSelectorOpen && m_ColorSelectorWindow->IsPointInMouseDownZone(Msg.GetLocation()))
		m_ColorSelectorWindow->OnMouseDown(Msg);
}

void ColorPickerElement::OnMouseUp(const MouseMessage& Msg)
{
	MenuElement::OnMouseUp(Msg);
	if (Msg.GetButton() == MouseMessage::MouseButton::Left)
		m_IsMouseDown = false;

	if (m_IsColorSelectorOpen)
		m_ColorSelectorWindow->OnMouseUp(Msg);
}

void ColorPickerElement::OnMouseMove(const MouseMessage& Msg)
{
	if (MenuElement::IsCursorInElement())
		MenuElement::OnMouseMove(Msg);

	if (!m_IsColorSelectorOpen)
		return;

	if (!m_ColorSelectorWindow->IsPointInControl(Msg.GetLocation()) &&
		m_ColorSelectorWindow->IsCursorInElement())
	{
		m_ColorSelectorWindow->OnMouseLeave(Msg);
	}
	else if (m_ColorSelectorWindow->IsPointInControl(Msg.GetLocation()) &&
		!m_ColorSelectorWindow->IsCursorInElement())
	{
		m_ColorSelectorWindow->OnMouseEnter(Msg);
	}

	if (m_ColorSelectorWindow->IsCursorInElement())
	{
		m_ColorSelectorWindow->OnMouseMove(Msg);
	}
}

void ColorPickerElement::AddPosition(const Vector2f& NewAmount)
{
	MenuElement::AddPosition(NewAmount);
	m_ColorSelectorWindow->AddPosition(NewAmount);
}

void ColorPickerElement::SetPosition(const Vector2f& NewPos)
{
	MenuElement::SetPosition(NewPos);
	m_ColorSelectorWindow->SetPosition(NewPos);
}

bool ColorPickerElement::IsPointInControl(const Vector2f& Point)
{
	if (MenuElement::IsPointInControl(Point))
		return true;

	if (m_IsColorSelectorOpen && m_ColorSelectorWindow->IsPointInControl(Point))
		return true;

	return false;
}

bool ColorPickerElement::IsCursorInElement()
{
	if (MenuElement::IsCursorInElement())
		return true;

	if (m_IsColorSelectorOpen && m_ColorSelectorWindow->IsCursorInElement())
		return true;

	return false;
}

bool ColorPickerElement::IsPointInMouseDownZone(const Vector2f& Point)
{
	if (MenuElement::IsPointInMouseDownZone(Point))
		return true;

	if (!m_IsColorSelectorOpen)
		return false;

	if (m_ColorSelectorWindow->IsPointInMouseDownZone(Point))
		return true;

	return false;
}

ColorPickerElement::eColorChanged& ColorPickerElement::EventColorChanged()
{
	return m_eColorChanged;
}

void ColorPickerElement::OnColorChanged(const int Value, uint32_t SliderId)
{
	if (SliderId == m_RedSliderId)
	{
		m_Ctx.m_FillColor.R = Value / 255.0f;
	}
	else if (SliderId == m_GreenSliderId) {
		m_Ctx.m_FillColor.G = Value / 255.0f;
	}
	else if (SliderId == m_BlueSliderId) {
		m_Ctx.m_FillColor.B = Value / 255.0f;
	}
	else if (SliderId == m_AlphaSliderId) {
		m_Ctx.m_FillColor.A = Value / 255.0f;
	}
	m_eColorChanged.Invoke(m_Ctx.m_FillColor, GetId());
}