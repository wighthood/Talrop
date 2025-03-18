// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetBoxExtent(FVector(50.0f, 10.0f, 50.0f));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && !OtherActor->IsA(APortal::StaticClass()))
	{
		if (DestinationPortal != nullptr)
		{
			OtherActor->SetActorLocation(DestinationPortal->GetActorLocation()+DestinationPortal->GetActorForwardVector()*150);

			ACharacter* Character = Cast<ACharacter>(OtherActor);
			if (Character)
			{
				UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
				if (MovementComponent)
				{
					MovementComponent->AddImpulse(DestinationPortal->GetActorForwardVector() * OtherActor->GetVelocity(), true);
				}
			}
		}
	}
}

void APortal::PortalLink(APortal* OtherPortal)
{
	DestinationPortal = OtherPortal;
	OtherPortal->DestinationPortal = this;
}

