// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent()) 
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;	
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());

	} 
}

bool UGrabber::GetGrabbableInReach(FHitResult& HitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	return GetWorld()->SweepSingleByChannel(
		HitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,Sphere);
}

void UGrabber::Grab() 
{
	FHitResult HitResult;

	if (GetGrabbableInReach(HitResult))
	{
		UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
		if (PhysicsHandle == nullptr) 
		{
			UE_LOG(LogTemp, Fatal, TEXT("Handler Not Found!"));
			return;
		}

		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	
			HitComponent->WakeAllRigidBodies();
			HitComponent->SetSimulatePhysics(true);
		
			AActor* HitResultActor = HitResult.GetActor();
			HitResultActor->Tags.Add("Grabbed");
		
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComponent,
				NAME_None,
				HitResult.ImpactPoint,
				GetComponentRotation()
			);
	}
}


void UGrabber::Release() 
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent()) 
	{
		PhysicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");
		PhysicsHandle->ReleaseComponent();
	} 
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const 
{
	return GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();;
}
