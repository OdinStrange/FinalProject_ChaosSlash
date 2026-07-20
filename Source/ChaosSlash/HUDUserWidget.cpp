#include "HUDUserWidget.h"
#include "Player/PlayerCharacterBase.h"

void UHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHUDUserWidget::SetupBindings(APlayerCharacterBase* PlayerCharacter)
{
	if (!PlayerCharacter) return;

	PlayerCharacter->OnPotionChanged.AddDynamic(this, &UHUDUserWidget::UpdatePotionTextBox);
	PlayerCharacter->OnKillCountChanged.AddDynamic(this, &UHUDUserWidget::UpdateKillCount);
	PlayerCharacter->OnComboChanged.AddDynamic(this, &UHUDUserWidget::UpdateCombo);
	PlayerCharacter->OnSpecialGaugeChanged.AddDynamic(this, &UHUDUserWidget::UpdateSpecialGauge);
}

void UHUDUserWidget::UpdatePotionTextBox(int32 NewCurrentPotion)
{
	if (PotionQuantity)
	{
		PotionQuantity->SetText(FText::AsNumber(NewCurrentPotion));
	}
}

void UHUDUserWidget::UpdateKillCount(int32 NewKillCount)
{
	if (KillCountText)
	{
		KillCountText->SetText(
			FText::Format(FText::FromString(TEXT("{0} Kill")), FText::AsNumber(NewKillCount))
		);
	}
}

void UHUDUserWidget::UpdateCombo(int32 NewComboCount)
{
	if (ComboText)
	{
		if (NewComboCount <= 0)
		{
			ComboText->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ComboText->SetVisibility(ESlateVisibility::Visible);
			ComboText->SetText(
				FText::Format(FText::FromString(TEXT("{0} Combo")), FText::AsNumber(NewComboCount))
			);
		}
	}
}

void UHUDUserWidget::UpdateSpecialGauge(float GaugePercent)
{
	if (SpecialGaugeBar)
	{
		SpecialGaugeBar->SetPercent(GaugePercent);

		// 게이지 MAX이면 보라색, 아니면 파란색
		if (GaugePercent >= 1.f)
		{
			SpecialGaugeBar->SetFillColorAndOpacity(FLinearColor(0.6f, 0.0f, 1.0f, 1.0f));
		}
		else
		{
			SpecialGaugeBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.4f, 1.0f, 1.0f));
		}
	}
}
