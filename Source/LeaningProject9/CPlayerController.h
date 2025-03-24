// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LEANINGPROJECT9_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(Client, Reliable)
	void ClientCreateGameUI();
	
	UFUNCTION(Client, Reliable)
	void ClientStartTurn(bool bMyTurn);

	UFUNCTION(Client, Reliable)
	void ClientPrintResult(int32 PreStrike, int32 PreBall, int32 RemainingCount);

	UFUNCTION(Client, Reliable)
	void ClientCreateResultWindow(const FString& ResultText, const FColor& ResultTextColor);	
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSendAnswer(const FString& Answer);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSendOut();	

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRestart();
	
	UFUNCTION(BlueprintPure)
	float GetTurnTimeRatio() const { return TurnTime / MaxTurnTime; } 
	UFUNCTION(BlueprintPure)
	bool IsMyTurn() const { return bIsMyTurn; }

private:
	UFUNCTION(Server, Reliable)
	void ServerJoinPlayer();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> MainWidgetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ResultWidgetType;
	
private:
	const float MaxTurnTime = 10.f;

	TObjectPtr<UUserWidget> MainWidget;
	TObjectPtr<UUserWidget> ResultWidget;
	
	bool bIsMyTurn;
	FTimerHandle TurnTimer;
	float TurnTime;
	
};
