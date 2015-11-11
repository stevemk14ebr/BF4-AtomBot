#pragma once
class CheckBoxElement :public MenuElement
{
public:
	typedef EventDispatcher<void(const bool)> eValueChanged;
	virtual ~CheckBoxElement() = default;
	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;

	bool IsChecked();

	eValueChanged& EventValueChanged();

	struct Context
	{
		//Required
		Color m_FillColor;
		Color m_FillColorMouseOver;
		Color m_FillColorChecked;
		Vector2f m_Position;
		Vector2f m_Size;

		//Optional
		bool m_DefaultState;
		uint32_t m_BorderFillGap;
		Color m_DepthHighlightColor;
		Color m_DepthShadeColor;
		Context()
		{
			m_DefaultState = false;
			m_BorderFillGap = 2;
			m_DepthShadeColor = Color::Black();
			m_DepthHighlightColor = Color::White();
		}
	};
	CheckBoxElement(const Context& Ctx);
protected:
	virtual void OnMouseDown(const MouseMessage& Msg) override;
private:
	Context m_Ctx;
	bool m_IsChecked;
	eValueChanged m_eValueChanged;
};

CheckBoxElement::CheckBoxElement(const Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
	m_IsChecked = m_Ctx.m_DefaultState;
}

void CheckBoxElement::Draw(RenderInterface& Renderer)
{
	Color CheckColor = m_Ctx.m_FillColor;
	if (m_IsChecked)
		CheckColor = m_Ctx.m_FillColorChecked;
	else if (m_CursorInElement)
		CheckColor = m_Ctx.m_FillColorMouseOver;

	Renderer.BeginLine();
	Renderer.DrawFilledBox(m_Position + Vector2f(m_Ctx.m_BorderFillGap, m_Ctx.m_BorderFillGap),
		m_Size - Vector2f(m_Ctx.m_BorderFillGap * 2, m_Ctx.m_BorderFillGap * 2), CheckColor);
	Renderer.DrawLineBox(m_Position, m_Size + Vector2f(1, 1), Color::Black());
	Renderer.EndLine();
}

void CheckBoxElement::OnMouseDown(const MouseMessage& Msg)
{
	MenuElement::OnMouseDown(Msg);
	m_IsChecked = !m_IsChecked;
	m_eValueChanged.Invoke(m_IsChecked);
}

bool CheckBoxElement::IsChecked()
{
	return m_IsChecked;
}

CheckBoxElement::eValueChanged& CheckBoxElement::EventValueChanged()
{
	return m_eValueChanged;
}

ElementType CheckBoxElement::GetType()
{
	return ElementType::Checkbox;
}