#pragma once

#include "Enumerations.h"
#include "SpriteSheet.h"
#include "Item.h"
#include "CountdownTimer.h"
#include "INT2.h"

class Level;
class Entity;
class KoopaShell;
class Yoshi;

class Player : public Entity
{
public:
	enum class POWERUP_STATE
	{
		NORMAL, SUPER, FIRE, CAPE, BALLOON, STAR, NONE
	};
	enum class ANIMATION_STATE
	{
		WAITING, WALKING, JUMPING, SPIN_JUMPING, FALLING, CLIMBING, NONE
	};

	Player(Level* levelPtr);
	virtual ~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void Tick(double deltaTime);
	void TickAnimations(double deltaTime);
	void Paint();
	void Reset();
	
	static POWERUP_STATE StringToPowerupState(std::string powerupStateStr);
	static std::string PowerupStateToString(POWERUP_STATE powerupState);

	int GetLives();
	int GetCoinsCollected();
	int GetDragonCoinsCollected();
	int GetStarsCollected();
	int GetScore();
	Item::TYPE GetExtraItemType();

	void AddItemToBeHeld(Item* itemPtr);
	bool IsHoldingItem();
	Item* GetHeldItemPtr();
	void DropHeldItem();

	DOUBLE2 GetLinearVelocity();
	void SetLinearVelocity(const DOUBLE2& newLinearVelRef);
	DOUBLE2 GetPosition();
	int GetDirectionFacing();

	POWERUP_STATE GetPowerupState();

	bool IsOnGround();

	bool IsRidingYoshi();
	void RideYoshi(Yoshi* yoshiPtr);
	void DismountYoshi(bool runWild = false);

	// Called when the player bounces off an enemy's head
	void Bounce();
	void Die();
	void TakeDamage();
	void KickShell(KoopaShell* koopaShellPtr, bool wasThrown);
	
	void ReleaseExtraItem(DOUBLE2 position);
	void OnItemPickup(Item* itemPtr, Level* levelPtr);

	void AddScore(int score, DOUBLE2 particlePosition);

	void MidwayGatePasshrough();
	void SetClimbingBeanstalk(bool climbing);
	ANIMATION_STATE GetAnimationState();
	int GetWidth();
	int GetHeight();
	bool IsDead();
	bool IsRunning();
	bool IsDucking();
	bool IsInvincible();
	bool IsAirborne();

	// NOTE: This should be called every time the player stomps on a KoopaTroopa's head
	//       Every frames m_FramesUntilEnemyHeadBounceEndSound is decremented, and plays the
	//		 sound when it reaches 0
	void ResetNumberOfFramesUntilEndStompSound();
	void SetOverlappingWithBeanstalk(bool overlapping);
	bool IsTransitioningPowerups();

private:
	void HandleKeyboardInput(double deltaTime);
	void HandleClimbingStateKeyboardInput(double deltaTime);
	void HandleYoshiKeyboardInput(double deltaTime);

	void AddCoin(Level* levelPtr, bool playSound = true);
	void AddDragonCoin(Level* levelPtr);
	void AddLife();
	void KickHeldItem(bool gently = false);
	void ChangePowerupState(POWERUP_STATE newPowerupState, bool isUpgrade = true);

	INT2 CalculateAnimationFrame();
	bool CalculateOnGround();
	SpriteSheet* GetSpriteSheetForPowerupState(POWERUP_STATE powerupState);
	String AnimationStateToString(ANIMATION_STATE state);
	
	SpriteSheet* m_SpriteSheetPtr = nullptr;

	static const double DEFAULT_GRAVITY;

	static const int JUMP_VEL;
	static const int WALK_BASE_VEL;
	static const int RUN_BASE_VEL;
	static const int YOSHI_DISMOUNT_XVEL;
	static const int BOUNCE_VEL;

	static const int YOSHI_TURN_AROUND_FRAMES = 15;
	static const double MARIO_SECONDS_PER_FRAME;

	bool m_IsOnGround;
	bool m_WasOnGround;
	int m_FramesSpentInAir;

	static const int STARTING_LIVES;
	int m_Lives;
	int m_Coins;
	int m_DragonCoins;
	int m_Stars;
	int m_Score;
	bool m_NeedsNewFixture;
	bool m_IsInvincible;

	CountdownTimer m_DeathAnimationTimer;
	CountdownTimer m_ChangingDirectionsTimer;
	CountdownTimer m_ShellKickAnimationTimer;
	CountdownTimer m_HeadStompSoundDelayTimer;
	CountdownTimer m_PowerupTransitionTimer;
	CountdownTimer m_InvincibilityTimer;
	CountdownTimer m_SpawnDustCloudTimer;

	POWERUP_STATE m_PrevPowerupState; // This is used to transition between states upon state change

	Item::TYPE m_ExtraItemToBeSpawnedType;
	Item* m_ExtraItemPtr; // This is the extra item slot mario has at the top of the screen

	Item* m_ItemHoldingPtr;

	int m_DirFacingLastFrame;
	int m_DirFacing;

	POWERUP_STATE m_PowerupState;
	
	ANIMATION_STATE m_AnimationState;
	bool m_IsDucking;
	bool m_IsLookingUp;
	bool m_IsRunning;
	bool m_IsHoldingItem;
	bool m_IsDead;
	bool m_IsRidingYoshi;

	bool m_IsOverlappingWithBeanstalk;
	static const int FRAMES_OF_CLIMBING_ANIMATION = 6;
	int m_FramesClimbingSinceLastFlip;
	int m_LastClimbingPose; // Flips between left and right as the player climbs

	Yoshi* m_RidingYoshiPtr;
};
