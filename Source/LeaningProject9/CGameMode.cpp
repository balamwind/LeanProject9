// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameMode.h"

#include "CPlayerController.h"
#include "Kismet/GameplayStatics.h"

void ACGameMode::AddPlayer()
{
	PlayerCount++;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("플레이어가 입장했습니다."));

	//게임 시작
	if (PlayerCount == 2)
		StartGame();
}

void ACGameMode::InputTurnPlayerAnswer(const FString& Answer)
{
	int Strike = 0;
	int Ball = 0;

	for (int i = 0; i < Answer.Len(); i++)
	{
		if (Problem[i] == Answer[i])
		{
			Strike++;
		}
		else
		{
			int Temp = INDEX_NONE;
			if (Problem.FindChar(Answer[i], Temp))
				Ball++;
		}
	}
	
	ProblemInputCount--;
	if (Answer == "OUT")
		NowTurnPlayer->ClientPrintResult(0, 0, INDEX_NONE);
	else
	{
		//1턴에 몇개의 문제를 풀 수 있는 횟수로 할거면 ProblemInputCount를 RemainingCount에
		//현재 1턴에 한번씩 문제를 풀 수 있으니 (MaxAnswerCount - AnswerCount - 1)를 보내줌
		NowTurnPlayer->ClientPrintResult(Strike, Ball, MaxAnswerCount - AnswerCount - 1);
	}

	//정답 검사
	if (Strike == 3)
	{
		//정답
		Win(NowTurnPlayer);
	}
	//다음 플레이어 턴
	else if (ProblemInputCount == 0)
	{
		AnswerCount++;
		if (AnswerCount == MaxAnswerCount)
		{
			//양쪽 다 풀지 못했음
			Draw();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
				TEXT("남은 정답 횟수 : ") + FString::FromInt(MaxAnswerCount - AnswerCount));
			
			TArray<AActor*> Controllers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayerController::StaticClass(), Controllers);

			AActor* NextTurnPlayer = *Controllers.FindByPredicate([&](AActor* Ele) { return Ele != NowTurnPlayer; });
			StartTurn(Cast<ACPlayerController>(NextTurnPlayer), Controllers);
		}
	}
}

void ACGameMode::StartGame()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("게임이 시작되었습니다."));

	AnswerCount = 0;
	Problem = "";

	//양쪽에 UI를 붙여줌
	TArray<AActor*> Controllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayerController::StaticClass(), Controllers);

	for (AActor* ControllerActor : Controllers)
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(ControllerActor))
			CPlayerController->ClientCreateGameUI();

	//문제 결정
	TArray<int> ProblemCandidates = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	for (int i = 0; i < 3; i++)
	{
		int RandomIndex = FMath::RandRange(0, ProblemCandidates.Num() - 1);

		Problem += FString::FromInt(ProblemCandidates[RandomIndex]);
		ProblemCandidates.RemoveAt(RandomIndex);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("정답 : ") + Problem);

	//시작 플레이어는 클라이언트
	ACPlayerController* ClientController = Cast<ACPlayerController>(Controllers[1]);
	StartTurn(ClientController, Controllers);
}

void ACGameMode::StartTurn(ACPlayerController* TurnPlayer, TArray<AActor*>& Controllers)
{
	if (!TurnPlayer)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("턴이 바뀌었습니다."));

	NowTurnPlayer = TurnPlayer;
	ProblemInputCount = MaxProblemInputCount;

	for (auto& ControllerActor : Controllers)
		Cast<ACPlayerController>(ControllerActor)->ClientStartTurn(TurnPlayer == ControllerActor);
}

void ACGameMode::Draw()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Draw");

	TArray<AActor*> Controllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayerController::StaticClass(), Controllers);

	for (auto& ControllerActor : Controllers)
		Cast<ACPlayerController>(ControllerActor)->ClientCreateResultWindow("Draw", FColor::Green);
}

void ACGameMode::Win(ACPlayerController* Winner)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("게임이 끝났습니다."));

	TArray<AActor*> Controllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayerController::StaticClass(), Controllers);

	for (auto& ControllerActor : Controllers)
		if (ControllerActor == NowTurnPlayer)
			Cast<ACPlayerController>(ControllerActor)->ClientCreateResultWindow("Win!!!", FColor::Yellow);
		else
			Cast<ACPlayerController>(ControllerActor)->ClientCreateResultWindow("Lose...", FColor::Red);
}
