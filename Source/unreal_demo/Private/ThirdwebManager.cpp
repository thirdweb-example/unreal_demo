// ThirdwebManager.cpp

#include "ThirdwebManager.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

AThirdwebManager::AThirdwebManager()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; // Set this based on your needs
	ServerUrl = "";
}

void AThirdwebManager::PerformLogin(const FString &GameServerUrl, const FString &Username, const FString &Password)
{
	this->ServerUrl = GameServerUrl;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(GameServerUrl + "/login");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Construct the JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("username", Username);
	JsonObject->SetStringField("password", Password);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// log
	UE_LOG(LogTemp, Warning, TEXT("OutputString: %s"), *OutputString);

	// Now, OutputString contains the JSON data.
	HttpRequest->SetContentAsString(OutputString);

	// Define what happens when the HTTP request is completed using a lambda.
	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
													   {
        if(bWasSuccessful && Response.IsValid())
        {
            // HTTP status code (e.g., 200 for success)
            int32 StatusCode = Response->GetResponseCode();

            if(StatusCode == 200) // OK, request succeeded
            {
                // Trigger the delegate event
                this->OnLoginResponse.Broadcast(true, Response->GetContentAsString());
            }
            else
            {
                // If we reach here, it means there was a problem with the request.
                FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
                this->OnLoginResponse.Broadcast(false, ErrorMsg);
            }
        }
        else
        {
            // This covers the scenario where the HTTP request completely failed (e.g., the server is down).
            this->OnLoginResponse.Broadcast(false, TEXT("Failed to connect to the server."));
        } });

	// Execute the request
	HttpRequest->ProcessRequest();
}

void AThirdwebManager::PerformClaim()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(this->ServerUrl + "/claim-erc20");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Construct the JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("walletAddress", "0x450b943729Ddba196Ab58b589Cea545551DF71CC");
	JsonObject->SetStringField("chain", "goerli");
	JsonObject->SetStringField("contractAddress", "0x450b943729Ddba196Ab58b589Cea545551DF71CC");
	JsonObject->SetStringField("amount", "1337");

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// log
	UE_LOG(LogTemp, Warning, TEXT("OutputString: %s"), *OutputString);

	// Now, OutputString contains the JSON data.
	HttpRequest->SetContentAsString(OutputString);

	// Define what happens when the HTTP request is completed using a lambda.
	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
													   {
        if(bWasSuccessful && Response.IsValid())
        {
            // HTTP status code (e.g., 200 for success)
            int32 StatusCode = Response->GetResponseCode();

            if(StatusCode == 200) // OK, request succeeded
            {
                // Trigger the delegate event
                this->OnClaimResponse.Broadcast(true, Response->GetContentAsString());
            }
            else
            {
                // If we reach here, it means there was a problem with the request.
                FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
                this->OnClaimResponse.Broadcast(false, ErrorMsg);
            }
        }
        else
        {
            // This covers the scenario where the HTTP request completely failed (e.g., the server is down).
            this->OnClaimResponse.Broadcast(false, TEXT("Failed to connect to the server."));
        } });

	// Execute the request
	HttpRequest->ProcessRequest();
}
