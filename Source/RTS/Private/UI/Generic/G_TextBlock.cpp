#include "UI/Generic/G_TextBlock.h"
#include "Components/TextBlock.h"

void UG_TextBlock::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo font = TextBlock->GetFont();
	font.TypefaceFontName = FontType;
	font.Size = FontSize;
	TextBlock->SetFont(font);
	SetText(Text);
}

void UG_TextBlock::SetText(const FText& InText)
{
	if (TextBlock)
	{
		TextBlock->SetText(InText);
		Text = InText;
	}
}
