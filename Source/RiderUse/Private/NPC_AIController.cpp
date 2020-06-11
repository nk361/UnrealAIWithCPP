// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "MyCharacter.h"//this name seems off
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "blackboard_keys.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameFramework/Character.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& object_initializer)
{
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/NPC_BT.NPC_BT'"));
    if(obj.Succeeded())
    {
        btree = obj.Object;
    }
    behavior_tree_component = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
    blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

    //setup_perception_system();
}

void ANPC_AIController::BeginPlay()
{
    Super::BeginPlay();
    RunBehaviorTree(btree);
    behavior_tree_component->StartTree(*btree);
}

void ANPC_AIController::OnPossess(APawn * const pawn)
{
    Super::OnPossess(pawn);
    if(blackboard)
    {
        blackboard->InitializeBlackboard(*btree->BlackboardAsset);
    }
}

UBlackboardComponent * ANPC_AIController::get_blackboard() const
{
    return blackboard;
}

void ANPC_AIController::on_target_detected(AActor * actor, FAIStimulus const stimulus)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Found target"));
    
    //if(AMyCharacter * const ch = Cast<AMyCharacter>(actor))//TODO currently this condition is never true
    //{
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Changing can see player key"));
        get_blackboard()->SetValueAsBool(bb_keys::can_see_player, stimulus.WasSuccessfullySensed());
    //}
}

void ANPC_AIController::setup_perception_system()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Setting up perception system"));
    
    //Create and initialize sight configuration object
    sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
    sight_config->SightRadius = 500.0f;
    sight_config->LoseSightRadius = sight_config->SightRadius + 50.0f;
    sight_config->PeripheralVisionAngleDegrees = 90.0f;//360 for full rotation view
    sight_config->SetMaxAge(5.0f);//Time until perceived stimulus is forgotten
    sight_config->AutoSuccessRangeFromLastSeenLocation = 900.0f;//Allows the AI to continue to see the stimulus source if it is within this distance of the location it was last seen
    sight_config->DetectionByAffiliation.bDetectEnemies = true;//Determines what kind of stimuli can be detected
    sight_config->DetectionByAffiliation.bDetectFriendlies = true;
    sight_config->DetectionByAffiliation.bDetectNeutrals = true;

    //Add sight configuration component to perception component
    GetPerceptionComponent()->SetDominantSense(*sight_config->GetSenseImplementation());
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::on_target_detected);//Call this function when something is perceived
    GetPerceptionComponent()->ConfigureSense(*sight_config);
}