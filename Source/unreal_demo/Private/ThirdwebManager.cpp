// ThirdwebManager.cpp

#include "ThirdwebManager.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

AThirdwebManager::AThirdwebManager()
{
	PrimaryActorTick.bCanEverTick = true;
	ServerUrl = "";
}

void AThirdwebManager::PerformLogin(const FString &GameServerUrl, const FString &Username, const FString &Password)
{
	this->ServerUrl = GameServerUrl;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(GameServerUrl + "/login");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("username", Username);
	JsonObject->SetStringField("password", Password);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	UE_LOG(LogTemp, Warning, TEXT("OutputString: %s"), *OutputString);

	HttpRequest->SetContentAsString(OutputString);

	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
													   {
        if(bWasSuccessful && Response.IsValid())
        {
            int32 StatusCode = Response->GetResponseCode();

            if(StatusCode == 200)
            {
                this->OnLoginResponse.Broadcast(true, Response->GetContentAsString());
            }
            else
            {
                FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
                this->OnLoginResponse.Broadcast(false, ErrorMsg);
            }
        }
        else
        {
            this->OnLoginResponse.Broadcast(false, TEXT("Failed to connect to the server."));
        } });

	HttpRequest->ProcessRequest();
}

void AThirdwebManager::PerformClaim()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(this->ServerUrl + "/claim-erc20");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("walletAddress", "0x450b943729Ddba196Ab58b589Cea545551DF71CC"); // TODO: get this from the login response
	JsonObject->SetStringField("chain", "goerli");
	JsonObject->SetStringField("contractAddress", "0x450b943729Ddba196Ab58b589Cea545551DF71CC");
	JsonObject->SetStringField("amount", "1337");

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	UE_LOG(LogTemp, Warning, TEXT("OutputString: %s"), *OutputString);

	HttpRequest->SetContentAsString(OutputString);

	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
													   {
        if(bWasSuccessful && Response.IsValid())
        {
            int32 StatusCode = Response->GetResponseCode();

            if(StatusCode == 200)
            {
                this->OnClaimResponse.Broadcast(true, Response->GetContentAsString());
            }
            else
            {
                FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
                this->OnClaimResponse.Broadcast(false, ErrorMsg);
            }
        }
        else
        {
            this->OnClaimResponse.Broadcast(false, TEXT("Failed to connect to the server."));
        } });

	HttpRequest->ProcessRequest();
}
