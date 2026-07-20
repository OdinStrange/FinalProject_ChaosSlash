#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "HUDUserWidget.h"
#include "PlayerCharacterBase.generated.h"

UENUM(BlueprintType)//언리얼 리플렉션 시스템에 등록하겠다
enum class EPlayerStates : uint8//enum class 자료형의 EPlayerStates를 1바이트 정수 크기로 저장하여
{
	NONE UMETA(DisplayName = "NONE"),//코드용 이름 NONE/에디터에 표시되는 이름 NONE
	ATTACK UMETA(DisplayName = "ATTACK"),
	RUN UMETA(DisplayName = "RUN"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	SWORD UMETA(DisplayName = "SWORD"),
	GREATSWORD UMETA(DisplayName = "GREATSWORD"),
	BLUNT UMETA(DisplayName = "BLUNT"),
	KATANA UMETA(DisplayName = "KATANA"),
	
};

// 1. 포션 개수가 바뀐 것을 UI에 알리기 위한 델리게이트 선언 (매개변수 1개: 새로운 포션 개수)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPotionChanged, int32, NewCurrentPotion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillCountChanged, int32, NewKillCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboChanged, int32, NewComboCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialGaugeChanged, float, GaugePercent);

UCLASS()
class CHAOSSLASH_API APlayerCharacterBase : public ACharacter
	//언리얼의 APlayerCharacterBase를 상속받아서 ACharacter라는 새 클래스를 만든다
{
	GENERATED_BODY()//필요한 숨은 코드들 넣어주는 매크로

public://클래스 공개 기능
	APlayerCharacterBase();//생성자

protected://부모와 자식 사이에만 열어둠
	virtual void BeginPlay() override;
	//부모 클래스에 있는 BeginPlay 함수를 이 클래스에서 다시 정의해서 쓰겠다

public:	
	virtual void Tick(float DeltaTime) override;
	//부모 클래스에 있는 Tick(float DeltaTime) 함수를 이 클래스에서 다시 정의해서 쓰겠다

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//부모 클래스에 있는 SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) 함수를 이 클래스에서 다시 정의해서 쓰겠다

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//AActor::TakeDamage를 재정의 — ApplyDamage 호출 시 엔진이 이 함수를 실행해 CurrentHP를 깎는다

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPotionChanged OnPotionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKillCountChanged OnKillCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnComboChanged OnComboChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSpecialGaugeChanged OnSpecialGaugeChanged;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void UsePotion();

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool AddPotion(int32 Amount = 1);

	// 적 처치 시 EnemyBase에서 호출
	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddKill();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void UseSpecialAttack();

private://그 클래스 안에서만 사용 가능
	// 숫자키 액션 바인딩용 콜백(Enhanced Input 시그니처 고정)
	UFUNCTION()//이 함수를 언리얼 리플렉션 시스템에 등록하겠다
	void OnSelectSword(const FInputActionValue& Value);
	//OnSelectSword 함수에 const FInputActionValue& Value라는 매개변수를 넣어서 실행한다.

	UFUNCTION()
	void OnSelectGreatSword(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectBlunt(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectKatana(const FInputActionValue& Value);

	// 실제 무기 변경 로직 (입력 바인딩에 직접 연결하지 않음)
	void ChangeWeaponTo(EWeaponType NewType);

protected:
	UPROPERTY(EditAnywhere)//이 변수를 어디서든 수정 가능하게끔 리플렉션 시스템에 등록한다.
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	// 속도에 따른 팔 길이 조절을 위한 설정값
	UPROPERTY(EditAnywhere, Category = "Camera")
	//디테일 패널에서 이 변수를 Camera 묶음으로 보여주고 어디서든 수정 가능하게끔 리플렉션 시스템에 등록한다.
	float MinArmLength = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxArmLength = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomInterpSpeed = 2.0f; // 변화 속도

private:
	void PlayerMeshInitialization();
	void WeaponMeshInitialization();
	void CameraInitialization();
	void InputInitialization();
	void EffectInitialization();

	void Move(const FInputActionValue& Value);
	//Move 함수 안에서 참조로 받는 Enhanced Input이 넘겨주는 입력 데이터와 입력값 Value를 수정하지 않고 처리하는 함수 생성자
	void Look(const FInputActionValue& Value);
	void Attack();

	void Die();

	bool bCanAttack = true;
	FTimerHandle AttackCooldownHandle;

	bool bCanUsePotion = true;
	FTimerHandle PotionCooldownHandle;

	int32 KillCount = 0;
	int32 ComboCount = 0;
	FTimerHandle ComboResetHandle;

	float SpecialGauge = 0.f;

	// 에디터에서 조정 가능: 기본 5킬 = 필살기 1회
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxSpecialGauge = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float GaugePerKill = 20.f;

	// 필살기 광역 반경 (cm)
	UPROPERTY(EditAnywhere, Category = "Combat")
	float SpecialAttackRadius = 600.f;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTrace();

private:
	UPROPERTY(VisibleAnywhere, Category = "Input")
	//디테일 패널에서 이 변수를 Input 묶음으로 보여주고 어디서든 보기만 가능하게끔 리플렉션 시스템에 등록한다.
	class UInputMappingContext* DefaultContext;
	//Enhanced Input에서 쓰는 입력 설정 포인터 DefaultContext

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AttackAction;

	// 3. 포션 데이터 변수들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	int32 CurrentPotion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	int32 MaxPotion;

public:
	UPROPERTY(VisibleAnywhere)
	bool bIsChangeWeaponTo;//bool 자료형 변수 bIsChangeWeaponTo

	UPROPERTY(EditAnywhere)
	float mouseSpeed = 30.0f;//30.0의 값을 대입한 실수형 자료형 변수 mouseSpeed

	UPROPERTY(EditAnywhere)
	float PlayerMoveSpeed = 30.0f;//30.0의 값을 대입한 실수형 자료형 변수 PlayerMoveSpeed

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* WeaponSkeletalMesh;
	//무기 스켈레탈 메시 컴포넌트를 가리키는 포인터 변수

	// 타입 -> 메쉬 매핑 (에디터에서 채우는 방식 권장)
	UPROPERTY(EditDefaultsOnly)//클래스의 기본값만 수정할 수 있다
	TMap<EWeaponType, TObjectPtr<USkeletalMesh>> WeaponMeshMap;//맵 자료구조/키/값/변수 이름

protected:
	// 숫자키 입력 액션들(에디터/블루프린트에서 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SWORDAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* GREATSWORDAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* BLUNTAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* KATANAAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* PotionAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SpecialAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHP = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float RespawnDelay = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float CurrentHP = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HealAmount = 500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackSpeed = 1.f; //무기별 공격 속도 배율 (몽타주 재생 속도 + 쿨타임에 적용)

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 1.0f; // 공격 쿨타임(초)

public:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

public:
	EPlayerStates playerState = EPlayerStates::NONE;
	EWeaponType WeaponType = EWeaponType::NONE;

	int32 SwordDamage = 25;
	int32 GreatSwordDamage = 200;
	int32 BluntDamage = 100;
	int32 KatanaDamage = 50;

public:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* WeaponHitSound;

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* FireEffect;
	

public:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundBase* BackgroundMusic;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> GameClearWidgetClass;

	void ShowGameClearUI();

	UPROPERTY(EditAnywhere, Category = "Widget")
	UUserWidget* HUDWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UImage* Weapon_Icon = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UTextBlock* WeaponTextBlock;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UProgressBar* HPBar = nullptr;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> WeaponCamerashakeClass;
};
