#pragma 
#include "MouseEvent.h"
#include <queue>

class MouseClass
{
public:
	void OnLeftPressed(int x, int y);
	void OnLeftRelesed(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);

	bool IsLeftDown();
	bool IsMiddleDown();
	bool IsRightDown();
	int GetPosX();
	int GetPosy();
	MousePoint GetPos();

	bool EventBufferIsEmoty();
	MouseEvent ReadEvent();

protected:
private:
	std::queue<MouseEvent>eventBuffer;
	bool leftIsDown = false;
	bool righIsDown = false;
	bool mbuttonDown = false;
	int x = 0;
	int y = 0;

};
