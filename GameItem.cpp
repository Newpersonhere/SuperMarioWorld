#pragma once

#include "stdafx.h"

#include "Game.h"
#include "GameItem.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "LevelData.h"
#include "Level.h"
#include "SoundManager.h"


// ___PLATFORM___
Platform::Platform(DOUBLE2 topLeft, DOUBLE2 bottomRight)
{
	m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
	m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width / 2, height / 2), 0, BodyType::STATIC);
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::PLATFORM));
}
Platform::~Platform()
{
	delete m_ActPtr;
}
void Platform::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}


// ___PIPE___
Pipe::Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, bool canAccess) : m_CanAccess(canAccess)
{
	m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
	m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width / 2, height / 2), 0, BodyType::STATIC);
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::PIPE));
}
Pipe::~Pipe()
{
	delete m_ActPtr;
}
void Pipe::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}


// ___ITEM___
Item::Item(DOUBLE2 topLeft,TYPE type, BodyType bodyType, int width, int height) :
	Entity(topLeft + DOUBLE2(width / 2, height / 2), 
		SpriteSheetManager::generalTiles, bodyType, this), m_Type(type), WIDTH(width), HEIGHT(height)
{
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::ITEM));
	m_ActPtr->SetFixedRotation(true);
}
void Item::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}
Item::TYPE Item::GetType()
{
	return m_Type;
}
void Item::TogglePaused(bool paused)
{
	m_ActPtr->SetActive(!paused);
}


// ___COIN___
// NOTE: There are two types of regular coins, those spawned at the start of the game,
// which have an infinite lifetime, and those which are spawned from prize blocks, which are only visible for 
// a few frames. If m_Life == -1, this coin is the former, otherwise it is the latter with LIFETIME ticks in its life
Coin::Coin(DOUBLE2 topLeft, int life, TYPE type, DOUBLE2 size) :
	Item(topLeft, type, BodyType::STATIC, int(size.x), int(size.y)), m_Life(life)
{
	m_ActPtr->SetFixedRotation(true);
	m_ActPtr->SetSensor(true);

	m_AnimInfo.msPerFrame = 0.15;

	if (life > -1)
	{
		// This coin shoots up, then falls down, then disapears
		m_ActPtr->SetBodyType(BodyType::DYNAMIC);
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -250));
	}
}
bool Coin::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;

	if (m_Life > -1)
	{
		if (m_Life == LIFETIME)
		{
			// NOTE: Give the player the coin right away so that the sound effect play now
			levelPtr->GiveItemToPlayer(this);
		}
		if (--m_Life <= 0)
		{
			m_Life = -1; // Not really necesary... right?
			return true;
		}
	}

	return false;
}
void Coin::Paint()
{
	double srcCol = 0 + m_AnimInfo.frameNumber;
	double srcRow = 0;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}

// ___DRAGON COIN___
// NOTE: Dragon coins always haev infinite life, therefore -1 as life param
DragonCoin::DragonCoin(DOUBLE2 centerPos) :
	Coin(centerPos, -1, TYPE::DRAGON_COIN, DOUBLE2(WIDTH, HEIGHT))
{
}
bool DragonCoin::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 6;

	return false;
}
void DragonCoin::Paint()
{
	double srcCol = 0 + m_AnimInfo.frameNumber;
	double srcRow = 2;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y - 9;
	// NOTE: Dragon coins are two tiles tall, we could draw both at once of course 
	// but drawing each half seperately is perhaps a bit simpler
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
	m_SpriteSheetPtr->Paint(left, top + Coin::HEIGHT, srcCol, srcRow + 1);
}


// ___P SWITCH___
PSwitch::PSwitch(DOUBLE2 topLeft, COLOUR colour) :
	Item(topLeft, Item::TYPE::P_SWITCH, BodyType::DYNAMIC), m_Colour(colour)
{
}
bool PSwitch::Tick(double deltaTime, Level* levelPtr)
{
	return false;
}
void PSwitch::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 4;
	double srcRow = 11;
	if (m_IsPressed)
	{
		srcCol += 1;
	}
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}

// ___1-UP MUSHROOM___
OneUpMushroom::OneUpMushroom(DOUBLE2 topLeft) :
	Item(topLeft, TYPE::ONE_UP_MUSHROOM)
{
}
bool OneUpMushroom::Tick(double deltaTime, Level* levelPtr)
{
	return false;
}
void OneUpMushroom::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 2;
	double srcRow = 12;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
// ___3-UP MOON___
ThreeUpMoon::ThreeUpMoon(DOUBLE2 topLeft) :
	Item(topLeft, TYPE::THREE_UP_MOON)
{
}
bool ThreeUpMoon::Tick(double deltaTime, Level* levelPtr)
{
	return false;
}
void ThreeUpMoon::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 3;
	double srcRow = 7;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
// ___SUPER MUSHROOM___
SuperMushroom::SuperMushroom(DOUBLE2 topLeft, int horizontalDir, bool isStatic) :
	Item(topLeft, TYPE::SUPER_MUSHROOM, BodyType::DYNAMIC), m_IsStatic(isStatic)
{
	assert(horizontalDir == 1 || horizontalDir == -1);

	if (m_IsStatic)
	{
		m_ActPtr->SetActive(false);
	}
	else
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(m_HorizontalSpeed * horizontalDir, 0));
	}
}
bool SuperMushroom::Tick(double deltaTime, Level* levelPtr)
{
	if (m_IsStatic)
	{
		double xPos = levelPtr->GetCameraOffset().x + Game::WIDTH / 2;
		double yPos = levelPtr->GetCameraOffset().y + 25;
		m_ActPtr->SetPosition(DOUBLE2(xPos, yPos));
	}
	else
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(m_HorizontalSpeed, m_ActPtr->GetLinearVelocity().y));
	}
	return false;
}
void SuperMushroom::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 2;
	double srcRow = 12;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
// ___FIRE FLOWER___
FireFlower::FireFlower(DOUBLE2 topLeft) :
	Item(topLeft, TYPE::FIRE_FLOWER)
{
}
bool FireFlower::Tick(double deltaTime, Level* levelPtr)
{
	return false;
}
void FireFlower::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 4 + m_AnimInfo.frameNumber;
	double srcRow = 13;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
// ___CAPE FEATHER___
CapeFeather::CapeFeather(DOUBLE2 topLeft) :
	Item(topLeft, TYPE::CAPE_FEATHER)
{
}
bool CapeFeather::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;

	return false;
}
void CapeFeather::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 3;
	double srcRow = 8;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}

// ___BLOCK___
Block::Block(DOUBLE2 topLeft, TYPE type) :
	Item(topLeft, type, BodyType::STATIC, WIDTH, HEIGHT)
{
}
Block::~Block() {}

// ___PRIZE BLOCK___
PrizeBlock::PrizeBlock(DOUBLE2 topLeft) :
	Block(topLeft, TYPE::PRIZE_BLOCK)
{
}
bool PrizeBlock::Tick(double deltaTime, Level* levelPtr)
{
	if (m_ShouldSpawnCoin)
	{
		Item* newItem = new Coin(DOUBLE2(m_ActPtr->GetPosition().x - WIDTH / 2, m_ActPtr->GetPosition().y - HEIGHT), Coin::LIFETIME);
		levelPtr->AddItem(newItem);

		m_ShouldSpawnCoin = false;
	}

	if (m_CurrentFrameOfBumpAnimation > -1)
	{
		// NOTE: I *think* what happens during the bump animation is
		//		 the block renders the normal question mark (not spinning)
		//       and moves up then down around 3 or 5 pixels.
		//		 Then the block is "used" and renders the brown used block texture

		m_yo = m_CurrentFrameOfBumpAnimation;
		if (m_yo > m_FramesOfBumpAnimation / 2)
		{
			m_yo = (m_FramesOfBumpAnimation / 2) - (m_yo - (m_FramesOfBumpAnimation / 2));
		}
		m_yo = int(m_yo * -0.5);

		m_CurrentFrameOfBumpAnimation++;
		if (m_CurrentFrameOfBumpAnimation > m_FramesOfBumpAnimation)
		{
			m_CurrentFrameOfBumpAnimation = -1;
			m_yo = 0;
		}
	}
	else
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
	}
	return false;
}
void PrizeBlock::Paint()
{
	double srcCol = 0 + m_AnimInfo.frameNumber;
	double srcRow = 4;

	if (m_IsUsed)
	{
		srcCol = 4;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
void PrizeBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_CurrentFrameOfBumpAnimation = 2;
		m_yo = 0;

		m_ShouldSpawnCoin = true;
	}
}

// ___EXCLAMATION MARK BLOCK___
ExclamationMarkBlock::ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour, bool isSolid) :
	Block(topLeft, TYPE::EXCLAMATION_MARK_BLOCK), m_Colour(colour)
{
	m_AnimInfo.frameNumber = 0;
	SetSolid(isSolid);
}
bool ExclamationMarkBlock::Tick(double deltaTime, Level* levelPtr)
{
	if (m_ShouldSpawnSuperMushroom)
	{
		Item* newItem = new SuperMushroom(m_ActPtr->GetPosition() - DOUBLE2(WIDTH/2, 1.5*HEIGHT));
		newItem->AddContactListener(levelPtr);
		levelPtr->AddItem(newItem);

		m_ShouldSpawnSuperMushroom = false;
	}

	if (m_CurrentFrameOfBumpAnimation > -1)
	{
		// NOTE: I *think* what happens during the bump animation is
		//		 the block renders the normal question mark (not spinning)
		//       and moves up then down around 3 or 5 pixels.
		//		 Then the block is "used" and renders the brown used block texture

		m_yo = m_CurrentFrameOfBumpAnimation;
		if (m_yo > m_FramesOfBumpAnimation / 2)
		{
			m_yo = (m_FramesOfBumpAnimation / 2) - (m_yo - (m_FramesOfBumpAnimation / 2));
		}
		m_yo = int(m_yo * -0.5);

		m_CurrentFrameOfBumpAnimation++;
		if (m_CurrentFrameOfBumpAnimation > m_FramesOfBumpAnimation)
		{
			m_CurrentFrameOfBumpAnimation = -1;
			m_yo = 0;
		}
	}

	return false;
}
void ExclamationMarkBlock::Paint()
{
	double srcCol = 1;
	double srcRow = 10;
	if (m_IsSolid == false)
	{
		srcRow -= 1;
	}

	if (m_IsUsed)
	{
		srcRow = 4;
		srcCol = 4;
	}
	
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
void ExclamationMarkBlock::SetSolid(bool solid)
{
	m_IsSolid = solid;
	m_ActPtr->SetSensor(!solid);
}
void ExclamationMarkBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_CurrentFrameOfBumpAnimation = 2;
		m_yo = 0;

		m_ShouldSpawnSuperMushroom = true;
	}
}

// ___ROTATING BLOCK___
RotatingBlock::RotatingBlock(DOUBLE2 topLeft) :
	Block(topLeft, TYPE::ROTATING_BLOCK)
{
}
bool RotatingBlock::Tick(double deltaTime, Level* levelPtr)
{
	if (m_IsRotating)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
		if (m_AnimInfo.frameNumber == 3)
		{
			m_Rotations++;
			if (m_Rotations > MAX_ROTATIONS)
			{
				m_IsRotating = false;
				m_Rotations = 0;
				m_AnimInfo.frameNumber = 0;
			}
		}
	}

	return false;
}
void RotatingBlock::Paint()
{
	double srcCol = 0 + m_AnimInfo.frameNumber;
	double srcRow = 5;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
void RotatingBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);
	m_IsRotating = true;
}
bool RotatingBlock::IsRotating()
{
	return m_IsRotating;
}

// ___MESSAGE BLOCK___
int MessageBlock::m_BitmapWidth = -1;
int MessageBlock::m_BitmapHeight = -1;
const int MessageBlock::FRAMES_OF_ANIMATION = 360;
MessageBlock::MessageBlock(DOUBLE2 topLeft, String filePath) :
	Block(topLeft, TYPE::MESSAGE_BLOCK)
{
	m_BmpPtr = new Bitmap(filePath);
	m_BitmapWidth = m_BmpPtr->GetWidth();
	m_BitmapHeight = m_BmpPtr->GetHeight();
	m_AnimInfo.frameNumber = 0;
}
MessageBlock::~MessageBlock()
{
	delete m_BmpPtr;
}
bool MessageBlock::Tick(double deltaTime, Level* levelPtr)
{
	if (m_FramesUntilPause != -1 && --m_FramesUntilPause <= 0)
	{
		levelPtr->SetShowingMessage(true);
		m_FramesUntilPause = -1;
		m_FramesOfIntroAnimation = 0;
	}

	if (m_FramesOfIntroAnimation > -1)
	{
		if (levelPtr->IsShowingMessage() == false)
		{
			m_FramesOfIntroAnimation = -1;
			m_FramesOfOutroAnimation = 0;
		}
	}

	if (m_FramesOfOutroAnimation > FRAMES_OF_ANIMATION)
	{
		m_FramesOfOutroAnimation = -1;
	}

	return false;
}
void MessageBlock::Paint()
{
	double srcCol = 5;
	double srcRow = 9;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);

	int yo = 32;
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	MATRIX3X2 matViewPrevious = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	if (m_FramesOfIntroAnimation > -1)
	{
		m_FramesOfIntroAnimation++;

		// NOTE: Paints a growing rectangle for the first 'FRAMES_OF_ANIMATION' frames
		if (m_FramesOfIntroAnimation < FRAMES_OF_ANIMATION)
		{
			double width = double(m_FramesOfIntroAnimation) * (double(m_BitmapWidth) / double(FRAMES_OF_ANIMATION));
			double height = double(m_FramesOfIntroAnimation) * (double(m_BitmapHeight) / double(FRAMES_OF_ANIMATION));
			double x = Game::WIDTH / 2 - width / 2;
			double y = Game::HEIGHT / 2 - height / 2 - yo;
			GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
		}
		else
		{
			double x = Game::WIDTH / 2 - m_BmpPtr->GetWidth() / 2;
			double y = 25;
			GAME_ENGINE->DrawBitmap(m_BmpPtr, x, Game::HEIGHT / 2 - m_BmpPtr->GetHeight() / 2 - yo);
		}
	}
	else if (m_FramesOfOutroAnimation > -1)
	{
		m_FramesOfOutroAnimation++;

		// NOTE: Paints a shrinking rectangle for 'FRAMES_OF_ANIMATION' frames
		if (m_FramesOfOutroAnimation < FRAMES_OF_ANIMATION)
		{
			double width = m_BitmapWidth - double(m_FramesOfOutroAnimation) * (double(m_BitmapWidth) / double(FRAMES_OF_ANIMATION));
			double height = m_BitmapHeight - double(m_FramesOfOutroAnimation) * (double(m_BitmapHeight) / double(FRAMES_OF_ANIMATION));
			double x = Game::WIDTH / 2 - width / 2;
			double y = Game::HEIGHT / 2 - height / 2 - yo;
			GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
		}
	}

	GAME_ENGINE->SetViewMatrix(matViewPrevious);
}
void MessageBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	m_FramesUntilPause = FRAMES_TO_WAIT;

	SoundManager::PlaySoundEffect(SoundManager::SOUND::MESSAGE_BLOCK_HIT);
}
