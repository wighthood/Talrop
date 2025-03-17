// Copyright Epic Games, Inc. All Rights Reserved.


#include "talropWeaponComponent.h"
#include "talropCharacter.h"
#include "talropProjectile.h"
#include "Portal.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UtalropWeaponComponent::UtalropWeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
	for (int i = 0; i < MaxIndex; i++)
	{
		Portals.Add(nullptr);
	}
}


void UtalropWeaponComponent::SpawnPortal(int Index)
{
	if (PortalClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FHitResult OutHit;
			FCollisionQueryParams CollisionParam;
			CollisionParam.AddIgnoredActor(Character);
			UCameraComponent* Camera = Character->GetFirstPersonCameraComponent();

			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

			World->LineTraceSingleByChannel(OutHit, GetOwner()->GetActorLocation(),
				GetOwner()->GetActorLocation() + Camera->GetForwardVector() * 2000, ECC_MAX, CollisionParam);


			DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(),
				GetOwner()->GetActorLocation() + Camera->GetForwardVector() * 2000,
				FColor(255, 0, 0), false, .5, 0, 12.3f);

			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle positions
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at hit position
			if (OutHit.GetActor() != nullptr)
			{
				if (Portals[Index] != nullptr)
				{
					Portals[Index]->Destroy();
				}
				Portals[Index] = World->SpawnActor<APortal>(PortalClass, OutHit.ImpactPoint, OutHit.Normal.Rotation(), ActorSpawnParams);
				if (Index+1 == MaxIndex)
				{
					if (Portals[0] != nullptr)
					{
						Portals[Index]->PortalLink(Portals[0]);
					}
				}
				else
				{
					if (Portals[Index + 1] != nullptr)
					{
						Portals[Index]->PortalLink(Portals[Index + 1]);
					}
				}
			}
		}

		// Try and play the sound if specified
		/*if (FireSound != nullptr)
		{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
		}*/


		// Try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
}

void UtalropWeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	SpawnPortal(0);
}

void UtalropWeaponComponent::FirePortalRight()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	SpawnPortal(1);
}

bool UtalropWeaponComponent::AttachWeapon(AtalropCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UtalropWeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UtalropWeaponComponent::Fire);
			EnhancedInputComponent->BindAction(FireActionRight, ETriggerEvent::Triggered, this, &UtalropWeaponComponent::FirePortalRight);
		}
	}

	return true;
}

void UtalropWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}