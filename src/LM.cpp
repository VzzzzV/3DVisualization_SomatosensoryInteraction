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
	screenWidth = ::GetDeviceCaps(hdc, HORZRES);//HORZSIZEΪ��Ļˮƽ�ߴ磨��Ϊ�����ߴ磬��mm�ƣ�
	std::cout << screenHeight << ' ' << screenWidth;
	ReleaseDC(NULL, hdc);
}

void MouseListener::onConnect(const Controller& controller) {
	//controller.enableGesture(Gesture::TYPE_CIRCLE);//�������ơ�����Ȧ
	controller.enableGesture(Gesture::TYPE_KEY_TAP);//�������ơ������
	controller.enableGesture(Gesture::TYPE_SWIPE);//�������ơ�������
	//controller.enableGesture(Gesture::TYPE_SCREEN_TAP);//�������ơ��������Ļ
	controller.setPolicyFlags(Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);//��������ں�̨ʱ��Ȼ���Խ��յ�Leap Motion����Ϣ
}

void MouseListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();//��ȡ��ǰframe
	const Frame previous = controller.frame(1);//��ȡǰһ֡frame
	if (frame.hands().isEmpty())//�ж��Ƿ����ִ��ڣ�û���򷵻غ���
		return;
	/****************˫����ִ����깦��*********************/

	Hand leftHand;
	Hand rightHand;
	leftHand = frame.hands()[0];

	if (frame.hands().count() != 0)
	{
		for (int i = 1; i < frame.hands().count(); i++)//������ߵ����趨Ϊ����
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
		}//�趨������


		Leap::Vector stabilizedPosition = leftHand.fingers().frontmost().stabilizedTipPosition();//�õ�������ǰ��ָ�Ŀռ�λ��
		int x = stabilizedPosition.x / 310 * 1920 + (screenWidth) / 2;//����Ļ�Ŀ������Ϊԭ����1.2�������ٱ�Ե��λ�ѵ�����
		int y = screenHeight - stabilizedPosition.y / 175 * screenHeight + 400;//�����ײ��޷����

		if (Pattern == 0)//����깦��
			::SetCursorPos(x, y);
		//��ָ���Ƶ���ָ���ڵ�����λ��
		else if (Pattern == 1)//����ģʽ
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
				::SetCursorPos(x, y);//��ָ���Ƶ���ָ���ڵ�����λ��
		}

		else if (Pattern == 3)//������תģʽ
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
			::SetCursorPos(x, y);//��ָ���Ƶ���ָ���ڵ�����λ��
		}
	}

	const GestureList gestures = frame.gestures();//��ȡframe�е�����
	for (int g = 0; g < gestures.count(); g++)
	{
		Gesture gesture = gestures[g];

		for (int i = 0; i < gesture.hands().count(); i++)
		{
			if (gesture.hands()[i].id() == leftHand.id())
				return;
		}
		//�ж��Ƿ�Ϊ����
		if (gesture.type() == Gesture::TYPE_KEY_TAP)
		{
			::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
			Sleep(10);//����10����
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
		}
		//���Ϊ������
		if (gesture.type() == Gesture::TYPE_SCREEN_TAP)
		{
			::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
			Sleep(10);
			::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
			Sleep(10);
		}
		//����Ϊ����Ҽ�
		if (gesture.type() == Gesture::TYPE_SWIPE
			&& rightHand.palmNormal().y > -0.9)
			if (gesture.state() == Gesture::STATE_STOP)
				::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		//����Ϊ����������
	}
}