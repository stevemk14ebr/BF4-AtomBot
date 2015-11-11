#pragma once

class LabelElement :public MenuElement
{
public:
	virtual ~LabelElement() = default;
	struct Context
	{
		//Required
		std::string m_Text;
		Vector2f m_Position;
		Vector2f m_Size;

		//Optional
		Color m_TextColor;
		Context()
		{
			m_TextColor = Color::Black();
		}
	};
	LabelElement(const Context& Ctx);

	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;
private:
	Context m_Ctx;
};

LabelElement::LabelElement(const Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
}

void LabelElement::Draw(RenderInterface& Renderer)
{
	Vector2f BtnTxtSize = Renderer.MeasureString("%s", m_Ctx.m_Text.c_str());
	Vector2f DeltaSize = m_Size - BtnTxtSize;
	DeltaSize /= 2;
	Renderer.BeginText();
	Renderer.RenderText(Vector2f(m_Position.x + DeltaSize.x, m_Position.y + DeltaSize.y), m_Ctx.m_TextColor, "%s", m_Ctx.m_Text.c_str());
	Renderer.EndText();
}

ElementType LabelElement::GetType()
{
	return ElementType::Label;
}