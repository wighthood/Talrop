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
			OtherActor->SetActorLocation(DestinationPortal->GetActorLocation()+DestinationPortal->GetActorForwardVector() * 200);

			ACharacter* Character = Cast<ACharacter>(OtherActor);
			if (Character)
			{
				FRotator newRotation = Character->GetActorRotation();
				newRotation.Yaw = RelativeLinkRotation(Character->GetControlRotation().Quaternion()).Rotator().Yaw;
				newRotation.Pitch = RelativeLinkRotation(Character->GetControlRotation().Quaternion()).Rotator().Pitch;
				Character->GetController()->SetControlRotation(newRotation);
				UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
				if (MovementComponent != nullptr)
				{
					FVector Velocity = MovementComponent->Velocity;

					MovementComponent->AddImpulse(-Velocity, true);
					MovementComponent->AddImpulse(RelativeLinkRotation(Velocity.ToOrientationQuat()).Vector()*1000, true);
				}
			}
		}
	}
}

FQuat APortal::RelativeLinkRotation(FQuat Rot) const
{
	if (!DestinationPortal)
		return FQuat::Identity;

	FQuat localIn = GetActorQuat().Inverse() * Rot;

	FQuat EndRotation = DestinationPortal->GetActorQuat() * (FQuat(0, 0, 1, 0) * localIn);

	return EndRotation;
}


void APortal::PortalLink(APortal* OtherPortal)
{
	DestinationPortal = OtherPortal;
	OtherPortal->DestinationPortal = this;
}

