#pragma once

class Level;

class Pipe
{
public:
	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Level* levelPtr, bool canAccess = false);
	virtual ~Pipe();

	Pipe(const Pipe&) = delete;
	Pipe& operator=(const Pipe&) = delete;

	void AddContactListener(ContactListener* listener);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

private:
	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;

	// If true mario can enter this pipe
	bool m_CanAccess;
};
