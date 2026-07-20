#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUDUserWidget.generated.h"

class APlayerCharacterBase;

UCLASS()
class CHAOSSLASH_API UHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* EnemyHPBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PotionQuantity;

	// BP_HUDUserWidget에 같은 이름의 위젯을 추가해야 표시됨
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* KillCountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* ComboText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* SpecialGaugeBar;

public:
	void SetupBindings(APlayerCharacterBase* PlayerCharacter);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdatePotionTextBox(int32 NewCurrentPotion);

	UFUNCTION()
	void UpdateKillCount(int32 NewKillCount);

	UFUNCTION()
	void UpdateCombo(int32 NewComboCount);

	UFUNCTION()
	void UpdateSpecialGauge(float GaugePercent);
};
