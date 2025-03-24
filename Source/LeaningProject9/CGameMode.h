// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "CGameMode.generated.h"

class ACPlayerController;

UCLASS()
class LEANINGPROJECT9_API ACGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	void AddPlayer();
	void InputTurnPlayerAnswer(const FString& Answer);
	
private:
	void StartGame();
	void StartTurn(ACPlayerController* TurnPlayer, TArray<AActor*>& Controllers);
	void Draw();
	void Win(ACPlayerController* Winner);
	
private:
	const float MaxTurnTime = 10;
	const int32 MaxProblemInputCount = 1;
	const int32 MaxAnswerCount = 6;
	
	int32 PlayerCount = 0;
	ACPlayerController* NowTurnPlayer;
	FString Problem;
	int32 AnswerCount = 0;
	//문제를 오해해 1턴에 3번의 기회가 있고, 그 후 다음 턴 플레이어가 3번의 기회가 있는 줄 알아 제작했었음
	//어차피 만든 기능인거 확장성을 위해 남겨놓음
	int32 ProblemInputCount = MaxProblemInputCount;
};
