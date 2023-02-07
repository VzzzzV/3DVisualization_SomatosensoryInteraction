#include "stdafx.h"
#include "LM.h"


class MouseListener : public Listener {
public:
	virtual void onInit(const Controller&);
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
private:
	int screenWidth, screenHeight;
};

void MouseListener::onInit(const Controller& controller) {
	HDC hdc = GetDC(NULL);
	screenHeight = ::GetDeviceCaps(hdc, VERTRES);
	screenWidth = ::GetDeviceCaps(hdc, HORZRES);//HORZSIZE为屏幕水平尺寸（定为度量尺寸，以mm计）
	std::cout << screenHeight << ' ' << screenWidth;
	ReleaseDC(NULL, hdc);
}

void MouseListener::onConnect(const Controller& controller) {
	//controller.enableGesture(Gesture::TYPE_CIRCLE);//解锁手势——画圈
	controller.enableGesture(Gesture::TYPE_KEY_TAP);//解锁手势——点击
	controller.enableGesture(Gesture::TYPE_SWIPE);//解锁手势——挥手
	//controller.enableGesture(Gesture::TYPE_SCREEN_TAP);//解锁手势——点击屏幕
	controller.setPolicyFlags(Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);//允许程序在后台时依然可以接收到Leap Motion的消息
}

void MouseListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();//获取当前frame
	const Frame previous = controller.frame(1);//获取前一帧frame
	if (frame.hands().isEmpty())//判断是否有手存在，没有则返回函数
		return;
	/****************双手则执行鼠标功能*********************/

	Hand leftHand;
	Hand rightHand;
	leftHand = frame.hands()[0];

	if (frame.hands().count() != 0)
	{
		for (int i = 1; i < frame.hands().count(); i++)//将最左边的手设定为左手
		{
			if (leftHand.fingers().frontmost().stabilizedTipPosition().x > frame.hands()[i].fingers().frontmost().stabilizedTipPosition().x)
				leftHand = frame.hands()[i];
		}
		for (int i = 0; i < frame.hands().count(); i++)
		{
			if (frame.hands()[i].id() == leftHand.id())
				continue;
			else
				rightHand = frame.hands()[i];
		}//设定左右手


		Leap::Vector stabilizedPosition = leftHand.fingers().frontmost().stabilizedTipPosition();//得到左手最前手指的空间位置
		int x = stabilizedPosition.x / 310 * 1920 + (screenWidth) / 2;//将屏幕的宽高扩大为原来的1.2倍，减少边缘定位难的问题
		int y = screenHeight - stabilizedPosition.y / 175 * screenHeight + 400;//调整底部无法检测

		if (Pattern == 0)//仅鼠标功能
			::SetCursorPos(x, y);
		//将指针移到手指所在的坐标位置
		else if (Pattern == 1)//缩放模式
		{
			if (leftHand.palmNormal().y<0.1
				&&leftHand.palmNormal().y>-0.4
				&&rightHand.palmNormal().y<0.1
				&&rightHand.palmNormal().y>-0.4
				&& frame.hands().count() == 2)
			{
				int Rightpalmx = rightHand.translation(previous).x * 10;
				int Leftplamx = leftHand.translation(previous).x * 10;
				//pView->zoomx = pView->zoomx + Rightpalmx - Leftplamx;
			//	pView->zoomy = 1.0*pView->zoomx*pView->Dib->Height / pView->Dib->Width;

			}
			else
				::SetCursorPos(x, y);//将指针移到手指所在的坐标位置
		}

		else if (Pattern == 3)//进入旋转模式
		{
			if (leftHand.palmNormal().y<0.15
				&&leftHand.palmNormal().y>-0.5
				&&rightHand.palmNormal().y < -0.95
				&& frame.hands().count() == 2)
			{
			}
			else if (frame.hands().count() == 1)
			{
				Hand Hand = frame.hands()[0];
				int tranz = Hand.translation(previous).y * 10;
			}
			::SetCursorPos(x, y);//将指针移到手指所在的坐标位置
		}
	}

	const GestureList gestures = frame.gestures();//获取frame中的手势
	for (int g = 0; g < gestures.count(); g++)
	{
		Gesture gesture = gestures[g];

		for (int i = 0; i < gesture.hands().count(); i++)
		{
			if (gesture.hands()[i].id() == leftHand.id())
				return;
		}
		//判断是否为右手
		if (gesture.type() == Gesture::TYPE_KEY_TAP)
		{
			::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
			Sleep(10);//缓冲10毫秒
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
		}
		//点击为鼠标左键
		if (gesture.type() == Gesture::TYPE_SCREEN_TAP)
		{
			::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
			Sleep(10);
			::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
			Sleep(10);
		}
		//触屏为鼠标右键
		if (gesture.type() == Gesture::TYPE_SWIPE
			&& rightHand.palmNormal().y > -0.9)
			if (gesture.state() == Gesture::STATE_STOP)
				::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		//挥手为按下鼠标左键
	}
}