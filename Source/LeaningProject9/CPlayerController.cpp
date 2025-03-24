// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"

#include "CGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//서버로 들어왔다고 메세지를 보냄
	if (IsLocalController())
		ServerJoinPlayer();
}

void ACPlayerController::ClientCreateResultWindow_Implementation(const FString& ResultText, const FColor& ResultTextColor)
{
	GetWorldTimerManager().ClearTimer(TurnTimer);

	ResultWidget = CreateWidget(GetWorld(), ResultWidgetType);
	if (ResultWidget)
		ResultWidget->AddToViewport();

	if (UWidget* ResultTextWidget = ResultWidget->GetWidgetFromName("ResultText"))
		if (UTextBlock* ResultTextBlock = Cast<UTextBlock>(ResultTextWidget))
		{
			ResultTextBlock->SetText(FText::FromString(ResultText));
			ResultTextBlock->SetColorAndOpacity(FSlateColor(ResultTextColor));
		}
}

void ACPlayerController::ClientCreateGameUI_Implementation()
{
	MainWidget = CreateWidget(GetWorld(), MainWidgetType);
	if (MainWidget)
		MainWidget->AddToViewport();
}

void ACPlayerController::ClientStartTurn_Implementation(bool bMyTurn)
{
	bIsMyTurn = bMyTurn;
	TurnTime = 0;

	//타이머 설정. 이후 1초마다 값 보정 해줄 예정
	GetWorldTimerManager().ClearTimer(TurnTimer);
	GetWorldTimerManager().SetTimer(TurnTimer, [&]
	{
		if (TurnTime >= MaxTurnTime)
		{
			GetWorldTimerManager().ClearTimer(TurnTimer);

			if (UKismetSystemLibrary::IsServer(GetWorld()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("시간초과"));
				//서버에서만 실행되며, 현재 턴 플레이어가 Out이라고 호출
				GetWorld()->GetAuthGameMode<ACGameMode>()->InputTurnPlayerAnswer("OUT");
			}
		}

		TurnTime += 0.02f;
	}, 0.02f, true);

	if (bIsMyTurn)
	{
		//턴 플레이어 Text 수정
		if (UWidget* TextWidget = MainWidget->GetWidgetFromName("TurnText"))
			if (UTextBlock* TextBlock = Cast<UTextBlock>(TextWidget))
			{
				TextBlock->SetText(FText::FromString(TEXT("당신의 턴입니다.")));
				TextBlock->SetColorAndOpacity(FSlateColor(FColor::White));
			}

		//입력 할 수 있음
		if (UWidget* InputWidget = MainWidget->GetWidgetFromName("Input"))
			if (UEditableTextBox* InputTextBox = Cast<UEditableTextBox>(InputWidget))
				InputTextBox->SetIsReadOnly(false);
	}
	else
	{
		//턴 플레이어 Text 수정
		if (UWidget* TextWidget = MainWidget->GetWidgetFromName("TurnText"))
			if (UTextBlock* TextBlock = Cast<UTextBlock>(TextWidget))
			{
				TextBlock->SetText(FText::FromString(TEXT("상대의 턴입니다.")));
				TextBlock->SetColorAndOpacity(FSlateColor(FColor::Red));
			}

		//입력 막음
		if (UWidget* InputWidget = MainWidget->GetWidgetFromName("Input"))
			if (UEditableTextBox* InputTextBox = Cast<UEditableTextBox>(InputWidget))
				InputTextBox->SetIsReadOnly(true);
	}
}

void ACPlayerController::ClientPrintResult_Implementation(int32 PreStrike, int32 PreBall, int32 RemainingCount)
{
	if (RemainingCount > 0)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		                                 FString::FromInt(RemainingCount) + TEXT(" 번 남았습니다"));

	if (UWidget* TextWidget = MainWidget->GetWidgetFromName("ResultText"))
		if (UTextBlock* TextBlock = Cast<UTextBlock>(TextWidget))
		{
			FString Str = "";
			if (RemainingCount == INDEX_NONE)
				Str = "OUT";
			else
			{
				Str.AppendInt(PreStrike);
				Str.AppendChar('S');
				Str.AppendInt(PreBall);
				Str.AppendChar('B');
			}

			TextBlock->SetText(FText::FromString(Str));
		}
}

void ACPlayerController::ServerSendAnswer_Implementation(const FString& Answer)
{
	GetWorldTimerManager().ClearTimer(TurnTimer);
	GetWorld()->GetAuthGameMode<ACGameMode>()->InputTurnPlayerAnswer(Answer);
}

void ACPlayerController::ServerSendOut_Implementation()
{
	GetWorldTimerManager().ClearTimer(TurnTimer);
	GetWorld()->GetAuthGameMode<ACGameMode>()->InputTurnPlayerAnswer("OUT");
}

void ACPlayerController::ServerJoinPlayer_Implementation()
{
	GetWorld()->GetAuthGameMode<ACGameMode>()->AddPlayer();
}

void ACPlayerController::ServerRestart_Implementation()
{
	//서버에 재시작 중이라고 메세지 띄우기
	if (ACPlayerController* Cont = Cast<ACPlayerController>(GetWorld()->GetFirstPlayerController()))
		if (UWidget* TextWidget = Cont->MainWidget->GetWidgetFromName("ResultText"))
			if (UTextBlock* TextBlock = Cast<UTextBlock>(TextWidget))
				TextBlock->SetText(FText::FromString(TEXT("게임을 다시 시작하는 중")));

	GetWorld()->GetAuthGameMode<ACGameMode>()->RestartGame();
}
