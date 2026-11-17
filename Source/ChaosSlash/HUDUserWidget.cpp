#include "HUDUserWidget.h"
#include "Player/PlayerCharacterBase.h"
#include "Materials/MaterialInstanceDynamic.h"

void UHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SpecialGaugeBar)
	{
		ProgressMID = SpecialGaugeBar->GetDynamicMaterial();

		if (!SpecialGaugeBar)
		{
			UE_LOG(LogTemp, Warning, TEXT("Is not valid Material on Progress Image"));
		}
	}
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
		ProgressMID->SetScalarParameterValue(FName("Progress"), FMath::Clamp(GaugePercent, 0.0f, 1.0f));
		UE_LOG(LogTemp, Warning, TEXT("Progress Updated"));
		
	}
}
