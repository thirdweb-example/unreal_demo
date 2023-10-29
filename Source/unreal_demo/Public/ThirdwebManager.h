// ThirdwebManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThirdwebManager.generated.h"

// Declare a new multicast delegate type.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResponse, bool, bWasSuccessful, const FString &, ResponseString);

UCLASS()
class UNREAL_DEMO_API AThirdwebManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AThirdwebManager();

	// Function to perform the login operation
	UFUNCTION(BlueprintCallable, Category = "Thirdweb")
	void PerformLogin(const FString &GameServerUrl, const FString &Username, const FString &Password);

	// Function to perform the claim operation
	UFUNCTION(BlueprintCallable, Category = "Thirdweb")
	void PerformClaim();

	// This delegate is triggered in C++, and Blueprints can bind to it.
	UPROPERTY(BlueprintAssignable, Category = "Thirdweb", meta = (DisplayName = "OnLoginResponse"))
	FOnLoginResponse OnLoginResponse;

	// This delegate is triggered in C++, and Blueprints can bind to it.
	UPROPERTY(BlueprintAssignable, Category = "Thirdweb", meta = (DisplayName = "OnClaimResponse"))
	FOnLoginResponse OnClaimResponse;

private:
	FString ServerUrl;
};
