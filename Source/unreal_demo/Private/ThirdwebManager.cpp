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
	ServerUrl = "https://engine-express-bez4.chainsaw-dev.zeet.app/";
}

void AThirdwebManager::PerformLogin(const FString &Username, const FString &Password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(this->ServerUrl + "/user/login");
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
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

				if (FJsonSerializer::Deserialize(Reader, JsonObject))
				{
					AuthToken = JsonObject->GetStringField("authToken");
				}

				this->OnLoginResponse.Broadcast(true, Response->GetContentAsString());
				UE_LOG(LogTemp, Warning, TEXT("AuthToken: %s"), *AuthToken);
			}
            else
            {
                FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
                this->OnLoginResponse.Broadcast(false, ErrorMsg);
				UE_LOG(LogTemp, Warning, TEXT("ErrorMsg: %s"), *ErrorMsg);
            }
        }
        else
        {
            this->OnLoginResponse.Broadcast(false, TEXT("Failed to connect to the server."));
			UE_LOG(LogTemp, Warning, TEXT("Failed to connect to the server."));
        } });

	HttpRequest->ProcessRequest();
}

void AThirdwebManager::PerformClaim()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(this->ServerUrl + "/engine/claim-erc20");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("authToken", AuthToken);

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
				UE_LOG(LogTemp, Warning, TEXT("Claim response: %s"), *Response->GetContentAsString());
            }
            else
            {
                FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
                this->OnClaimResponse.Broadcast(false, ErrorMsg);
				UE_LOG(LogTemp, Warning, TEXT("ErrorMsg: %s"), *ErrorMsg);
            }
        }
        else
        {
            this->OnClaimResponse.Broadcast(false, TEXT("Failed to connect to the server."));
			UE_LOG(LogTemp, Warning, TEXT("Failed to connect to the server."));
        } });

	HttpRequest->ProcessRequest();
}

void AThirdwebManager::GetBalance()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(this->ServerUrl + "/engine/get-erc20-balance");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("authToken", AuthToken);

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
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
				{
					const TSharedPtr<FJsonObject> *ResultObject;
					if (JsonObject->TryGetObjectField("result", ResultObject))
					{
						FString DisplayValue;
						if ((*ResultObject)->TryGetStringField("displayValue", DisplayValue))
						{
							this->OnBalanceResponse.Broadcast(true, DisplayValue);
							return;
						}
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("Balance response: %s"), *Response->GetContentAsString());
			}
			else
			{
				FString ErrorMsg = FString::Printf(TEXT("HTTP Error: %d, Response: %s"), StatusCode, *(Response->GetContentAsString()));
				this->OnBalanceResponse.Broadcast(false, ErrorMsg);
				UE_LOG(LogTemp, Warning, TEXT("ErrorMsg: %s"), *ErrorMsg);
			}
		}
		else
		{
			this->OnBalanceResponse.Broadcast(false, TEXT("Failed to connect to the server."));
			UE_LOG(LogTemp, Warning, TEXT("Failed to connect to the server."));
		} });

	HttpRequest->ProcessRequest();
}
