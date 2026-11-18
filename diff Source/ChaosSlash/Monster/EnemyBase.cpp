@@
 AEnemyBase::AEnemyBase()
 {
+	// 적 액터의 기본 설정을 초기화합니다.
+	// 실제 적 종류별 메시와 능력치는 BeginPlay()에서 결정됩니다.
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
@@
 void AEnemyBase::BeginPlay()
 {
    Super::BeginPlay();

-	// 메시, 애니메이션, 크기, 체력, 공격력 초기화
+	// 스포너가 설정한 CurrentEnemyType을 기준으로
+	// 메시, 애니메이션, 크기, 체력, 공격력을 초기화합니다.
    InitializeMeshes();
    InitializeAnimInstance();
    InitializeSize();
    InitializeHp();
    InitializeDamage();

+	// 적이 생성되면 플레이어 HUD의 적 HP 바를 표시하고
+	// 현재 적의 최대 HP 비율로 초기화합니다.
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
@@
 void AEnemyBase::InitializeMeshes()
 {
+	// 적 종류에 대응하는 스켈레탈 메시를 적용합니다.
    if (USkeletalMesh** FoundMesh = EnemyMeshes.Find(CurrentEnemyType))
@@
+	// 메시의 기본 위치와 회전을 캐릭터 기준에 맞게 보정합니다.
    GetMesh()->SetRelativeLocationAndRotation(
@@
 void AEnemyBase::InitializeAnimInstance()
 {
+	// 적 종류에 맞는 애니메이션 블루프린트를 적용합니다.
    if (TSubclassOf<UAnimInstance>* FoundAnim =
@@
 void AEnemyBase::InitializeSize()
 {
+	// 적 종류에 따라 액터의 크기를 설정합니다.
+	// MAGICENEMY는 일반 적보다 큰 크기를 사용합니다.
    if (CurrentEnemyType == EEnemyType::NORMALENEMY)
@@
 void AEnemyBase::InitializeHp()
 {
+	// 적 종류별 최대 HP를 설정합니다.
    if (CurrentEnemyType == EEnemyType::NORMALENEMY)
@@
 void AEnemyBase::Attack()
 {
+	// 현재 재생 중인 공격 몽타주가 없을 때만 공격 애니메이션을 실행합니다.
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
@@
 float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
 {
-	//부모(AActor)의 TakeDamage를 먼저 실행해 엔진 내부 처리를 보장
+	// 부모 클래스의 데미지 처리를 먼저 실행합니다.
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

-	//CurrentEnemyHp에서 데미지를 빼되 0~MaxEnemyHp 범위를 벗어나지 않게 고정
+	// HP가 0보다 작아지거나 최대 HP를 초과하지 않도록 제한합니다.
    CurrentEnemyHp = FMath::Clamp(CurrentEnemyHp - ActualDamage, 0.f, MaxEnemyHp);

-	// 피격 사운드 랜덤 재생
+	// 등록된 피격 사운드 중 하나를 무작위로 재생합니다.
@@
+	// 변경된 HP를 플레이어 HUD에 반영합니다.
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
@@
    if (CurrentEnemyHp <= 0.f)
    {
+		// 적이 사망하면 HUD를 숨기고,
+		// 처치 수 증가, 아이템 드롭, 사망 이벤트 전파를 처리합니다.
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("[Enemy] 사망 → Destroy"));
@@
        APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(DamageCauser);
        if (PlayerChar)
        {
+			// 최종 공격자가 플레이어인 경우 처치 수를 증가시킵니다.
            PlayerChar->AddKill();
        }

+		// 현재 월드에 존재하는 포션 수가 3개 미만이고
+		// 드롭 확률에 당첨된 경우 포션을 생성합니다.
        TArray<AActor*> ExistingPotions;
@@
+		// 스포너에 사망 사실을 알린 후 적 액터를 제거합니다.
        OnEnemyDied.Broadcast();
        Destroy();
@@
 void AEnemyBase::InitializeDamage()
 {
+	// 적 종류별 공격력을 설정합니다.
    if (CurrentEnemyType == EEnemyType::NORMALENEMY)
@@
 void AEnemyBase::InitializeAIController()
 {
+	// 월드에 배치되었거나 스폰된 즉시 AI 컨트롤러가 소유하도록 설정합니다.
    AIControllerClass = AEnemyAIController::StaticClass();
@@
 FString AEnemyBase::GetHUDMessage()
 {
+	// HUD에 표시할 적 이름을 적 종류에 따라 반환합니다.
    switch (CurrentEnemyType)