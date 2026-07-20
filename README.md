# S ChaosSlash 진도 현황 (2026-07-14 기준)

## 주차별 계획 vs 실제 구현 비교

|주차|계획 목표|실제 구현 상태|완성도|
|-|-|-|-|
|**1주차**|C++ 기초 (클래스/함수/포인터/상속) + GDD + Git LFS|클래스 정의, 멤버 변수/함수, 생성자, 상속(`ACharacter`), `UPROPERTY`/`UFUNCTION` 매크로 실제 코드에 적용됨|✅ 완료|
|**2주차**|WASD 이동/점프 + 3인칭 카메라 + 첫 패키징|Enhanced Input으로 이동/시점 구현, SpringArm+Camera 세팅, IMC\_Default + IA\_Move/IA\_Look 에셋 존재. `FObjectFinder<UInputAction>` 템플릿 공백 버그 수정 완료|✅ 완료|
|**3주차**|idle/walk/run 애니메이션 + 공격 몽타주|AnimInstance(플레이어/적 모두), 몽타주 에셋 다수 존재, ABP\_PlayerCharacterBase 블루프린트, AttackAnimNotify 셸|✅ 완료|
|**4주차**|콜리전 + 트레이스 + 데미지 시스템|구형 SweepTrace 구현, 파티클 이펙트 재생까지 동작. `CurrentHP``MaxHP` 변수 추가됨, `TakeDamage` 활성화 → 실제 HP 감소 및 사망(Destroy) 동작|✅ 완료|
|**5주차**|몬스터 AI (탐지/추격/공격)|BehaviorTree + Blackboard 에셋, AIController 연결(`AIControllerClass`, `AutoPossessAI`) 활성화됨. BTTask에서 `UGameplayStatics::ApplyDamage` 실제 호출 + 쿨타임 관리 구현. 공격 시 `AttackMontage` 재생 연결 완료|✅ 완료|
|**6주차**|체력 UI + 사망/리스폰|`CurrentHP`/`MaxHP` + `TakeDamage` 구현. `HPBar`(`UProgressBar`) BeginPlay 바인딩 및 피격 시 실시간 갱신. 사망 시 `Destroy()` → PC 타이머 3초 → `RestartPlayer(PC)`로 리스폰|✅ 완료|
|**7주차**|회복 아이템 + 던전 맵|**회복 아이템** — 포션 개수(`CurrentPotion`/`MaxPotion=4`) 관리, `UsePotion()`으로 체력(`HealAmount=50`) 회복, HUD `PotionQuantity` 텍스트 연동, 사망 시 초기화, 리스폰 시 HUD 중복 제거 구현. 3D 포션 메시 에셋 존재. **월드 픽업 액터** — `APotionPickup` C++ 클래스 제작(`SphereComponent` 충돌 감지, `UStaticMeshComponent` 메시 표시), `AEnemyBase` 사망 시 확률(`DropChance=0.05`) 기반 포션 스폰. 포션 드랍 과다 문제 수정: `GetAllActorsOfClass`로 월드 내 픽업 수 체크, 동시 최대 3개 초과 시 드랍 차단. Blueprint 자식(`BP\\\_PotionPickup`)에서 포션 메시 연결 및 `BP\\\_EnemyBase`에서 드랍 설정. **던전 맵** — 맵 배치 작업 완료, 네비메시 교체 완료, 구조물 배치 + Blocking Volume으로 바닥 콜리전 처리, `AEnemyBaseSpawner` C++ 클래스 제작(`AEnemyBase` 전용, `ICombatActivatable` 체인 연결), `OnEnemyDied` 델리게이트로 스포너 소진 감지 → 다음 구역 자동 활성화. `TMap::operator\\\[]` → `Find()`로 교체해 스폰 시 크래시 수정. **적 HP바** — 타격 시 표시, 적 사망 시 숨김, 게임 시작 시 숨김 처리.|✅ 완료|
|**8주차**|메인메뉴/클리어/게임오버 + 전체 통합|**메인메뉴** — `BP\\\_MainMenuWidget` 제작, 게임 시작/종료 버튼 연결, 메인메뉴 레벨 전환 구현. **게임오버** — `BP\\\_GameOverWidget` 제작, 다시 시작/메인메뉴 버튼 연결. `PlayerCharacterBase::Die()`에서 자동 리스폰 제거 후 게임오버 위젯 표시로 교체. **클리어 UI** — `BP\\\_GameClearWidget` 제작. `EnemyBaseSpawner`에서 월드 전체 적 수 + 모든 스포너 소진 여부를 동시 체크해 `ShowGameClearUI()` 호출. **스폰 시 기본 무기 장착** — `BeginPlay()`에서 소검 자동 장착. 스폰/재시작 시 입력 모드 게임 전용으로 초기화.|✅ 완료|
|**9주차**|버그픽스 + SFX/VFX + 패키징|**버그픽스** — 적 콜리전 카메라 채널 Ignore 처리(블루프린트). 루트 모션 활성화로 애니메이션 문제 해결. **SFX** — 배경음 `SC\\\_Dungeon\\\_3\\\_\\\_from\\\_Deluxe\\\_Edition` 연결(`BeginPlay`에서 재생). 적 피격 사운드 `DamageVocalizations\\\_Male` 22종 랜덤 재생(`TakeDamage`에서 처리). 환경음은 장르 특성상 생략. **패키징** — 완료(`DefaultEngine.ini`의 GameMode 경로 리다이렉트 문제 수정)|✅ 완료|
|**10주차**|발표 자료 + 시연 영상|발표회 미참가로 생략|➖ 해당 없음|

\---

## 계획에 없었는데 추가 구현된 것

|항목|내용|
|-|-|
|무기 4종 전환 시스템|숫자키로 소검/대검/둔기/카타나 전환, UI 텍스트로 데미지 수치 표시|
|카메라 줌 연동|앞으로 달릴 때 카메라가 자동으로 멀어지는 동적 팔 길이 조절|
|카메라 셰이크|공격 시 `BP\\\_WeaponLegacyCameraShake` 재생|
|몬스터 2종 구조|계획은 1종이었으나 Zombie + Juggernaut 두 타입 베이스 구성|
|포션 UI 시스템|`PotionQuantity` TextBlock에 현재 포션 개수 실시간 반영, 게임 시작 시 초기값(3) 즉시 표시|
|포션 입력 바인딩|`IA\\\_DrinkPotion` + `PotionAction`으로 키 입력 시 `UsePotion()` 호출|
|월드 포션 픽업 액터|`APotionPickup` C++ 클래스, `AEnemyBase` 사망 시 확률 드랍, 월드 캡(동시 최대 3개) 초과 시 드랍 차단|
|네비메시 교체|기존 NavMesh를 교체하여 AI 이동 경로 갱신|
|맵 배치 업데이트|던전 맵 오브젝트 재배치 작업 완료|
|`AEnemyBase` 전용 스포너|`AEnemyBaseSpawner` C++ 클래스, `ICombatActivatable` 체인으로 구역별 순차 활성화|
|적 HP바 가시성 제어|게임 시작 시 숨김, 타격 시 표시, 적 사망 시 숨김|
|사망·리스폰 HUD 정리|사망 시 기존 HUD 제거 → 리스폰 후 새 HUD 중복 없이 생성|
|무기별 개별 오프셋|`ChangeWeaponTo()` switch에서 무기마다 `SetRelativeLocation`/`SetRelativeRotation` 독립 적용|
|공격 범위 시각화 개선|`AttackTrace()` 에서 항상 구체 표시 (명중 빨강·빗나감 초록)|
|스폰 시 기본 무기 장착|`BeginPlay()` 마지막에 `ChangeWeaponTo(SWORD)` 호출로 소검 자동 장착|
|메인메뉴 UI|`BP\\\_MainMenuWidget` + 전용 레벨, 게임 시작/종료 버튼|
|게임오버 UI|`BP\\\_GameOverWidget`, 사망 시 자동 표시, 다시 시작/메인메뉴 버튼|
|클리어 UI|`BP\\\_GameClearWidget`, 모든 적 처치 + 모든 스포너 소진 시 자동 표시|

\---

## 지금 당장 우선순위로 수정해야 할 것

1. **전체 통합 테스트** — 메인메뉴 → 게임 → 클리어/게임오버 → 재시작 전체 흐름 검증
2. **9주차** — 버그픽스, SFX/VFX 연결, 패키징

<b>~~완료된 항목 (이전 우선순위)~~</b>

* ~~플레이어 HP 변수 추가 + TakeDamage 연결~~ → ✅ `CurrentHP`/`MaxHP` 추가, `TakeDamage` 활성화 완료
* ~~AIController 연결 주석 해제~~ → ✅ `AIControllerClass` + `AutoPossessAI` 활성화 완료
* ~~BTTask 공격 로직 구현~~ → ✅ `ApplyDamage` 실제 호출 + 쿨타임 관리 완료
* ~~몬스터 공격 애니메이션 연결~~ → ✅ `BTTask\\\_EnemyAttack`에서 `Attack()` 호출로 몽타주 재생 완료
* ~~기본 HUD에 체력바 연결~~ — `BP\\\_HUDUserWidget` 블루프린트에 ProgressBar 추가 후 `CurrentHP / MaxHP`로 바인딩 완료



## 추후 고쳐야 할 것들

1\. ~~플레이어 캐릭터의 애니메이션이 정상적으로 리타게팅 되지 않아서, 애니메이션이 정상적으로 플레이되고 있지 않아요. 전반적인 캐릭터 애니메이션이 어색합니다.~~ → ✅ 루트 모션 활성화로 해결

2\. ~~플레어의 마우스 왼쪽 클릭을 통한 공격시 다시 처음부터 몽타주가 재생이 되고 있습니다. 몽타주 재생시에는 공격 이벤트를 return 시키거나 쿨타임을 적용 시키는 것을 고려해 보세요.~~ → ✅ `Montage\\\_IsPlaying` 체크로 재생 중 연타 차단 완료

3\. ~~플레이어의 무기들의 소켓 위치가 손에 정확히 위치 하지 않습니다.~~ → ✅ 무기 위치 수정 완료

4\. ~~플레이어 무기 히트 사운드가, 애니메이션에 설정되어 있어 히트가 되지 않음에도 재생 되고 있습니다. 실제로 타격이 들어간 시점에 사운드가 플레이 되도록 수정해 보세요.~~ → ✅ `AttackTrace()`에서 `AEnemyBase` 적중 확인 후 `PlaySoundAtLocation`으로 `WeaponHitSound` 재생, AnimNotify 의존 제거 완료

5\. ~~플레이어와 카메라 사이에 몬스터가 지나갈때 CameraRag이 반응 하고 있어서 카메라의 위치가 갑자기 변경이 되고 있습니다.~~ → ✅ 블루프린트에서 적 콜리전 채널 Ignore 처리 완료

7\. ~~이 외에도, 완성도가 떨어져 보일만한 요소들 중에 가장 먼저 수정 해야 하는 부분으로 캐릭터의 texture 가 미 적용 상태인 부분도 함께 살펴 보세요!~~ → ✅ 기본 금속 텍스처 적용 완료

