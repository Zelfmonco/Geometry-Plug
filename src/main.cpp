//Geometry Plug main file

#include <future>
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/cocos/actions/CCActionInterval.h>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <string>

#include "../include/buttplugCpp.h"

using namespace geode::prelude;

std::string url = "ws://127.0.0.1";
Client client(url, 12345, "test.txt");
std::vector<DeviceClass> myDevices;

bool IsVibePercent = Mod::get()->getSettingValue<bool>("percentage-vibration");
bool IsDeathVibe = Mod::get()->getSettingValue<bool>("death-vibration");
bool IsVibeComplete = Mod::get()->getSettingValue<bool>("complete-vibration");
bool isVibeShake = Mod::get()->getSettingValue<bool>("shake-vibration");
bool isConnectedToServer = false;
bool vibe = false;
bool isLevelComplete = false;

float deathVibeStrength = Mod::get()->getSettingValue<int64_t>("death-vibration-strength");
float deathVibeLength = Mod::get()->getSettingValue<double>("death-vibration-length");
float completeVibeStrength = Mod::get()->getSettingValue<int64_t>("complete-vibration-strength");

int percentage;

$execute 
{
	listenForSettingChanges("death-vibration-strength", +[](int64_t p0) {deathVibeStrength = p0;});

	listenForSettingChanges("shake-vibration", +[](bool p0) {isVibeShake = p0;});
	
	listenForSettingChanges("death-vibration", +[](bool p0) {IsDeathVibe = p0;});

	listenForSettingChanges("percentage-vibration", +[](bool p0) {IsVibePercent = p0;});

	listenForSettingChanges("complete-vibration", +[](bool p0) {IsVibeComplete = p0;});

	listenForSettingChanges("death-vibration-length", +[](double p0) {deathVibeLength = p0;});

	listenForSettingChanges("complete-vibration-strength", +[](double p0) {completeVibeStrength = p0;});
}

using namespace std;

void callbackFunction(const mhl::Messages msg) {
	if (msg.messageType == mhl::MessageTypes::DeviceList) {
		cout << "Device List callback" << endl;
	}
	if (msg.messageType == mhl::MessageTypes::DeviceAdded) {
		cout << "Device Added callback" << endl;
	}
	if (msg.messageType == mhl::MessageTypes::ServerInfo) {
		cout << "Server Info callback" << endl;
	}
	if (msg.messageType == mhl::MessageTypes::DeviceRemoved) {
		cout << "Device Removed callback" << endl;
	}
	if (msg.messageType == mhl::MessageTypes::SensorReading) {
		cout << "Sensor Reading callback" << endl;
	}
};

//connects to server
int main()
{
	client.connect(callbackFunction);
	client.requestDeviceList();
	client.startScan();
	client.stopScan();
	
	return 0;
}

//vibrate device at a specified strength
void VibratePercent(float per)
{
	myDevices = client.getDevices();
	if(myDevices.size() == 0)
		return;
	
	client.sendScalar(myDevices[0], per / 100);
	vibe = true;
};

//stop device vibrating
void StopVibrate()
{
	if(myDevices.size() == 0)
		return;

	client.stopDevice(myDevices[0]);
	vibe = false;
};

using namespace geode::prelude;


class $modify(MenuLayer) 
{
	//Start when the menu layer initializes
	bool init() 
	{
		if((!MenuLayer::init()))
			return false;

		if(!isConnectedToServer)
		{
			main();
			isConnectedToServer = true;
		}
		return true;
	}
};

class $modify(MyPlayerObject, PlayerObject)
{
	//MyPlayerObject Stop Vibrating
	void StopVibe()
	{
		if(myDevices.size() == 0)
		return;

		client.stopDevice(myDevices [0]);
		vibe = false;
	}
	
	void playDeathEffect()
	{
		PlayerObject::playDeathEffect();
		
		if(IsVibePercent)
		{
			MyPlayerObject::StopVibe();
		}
		if(IsDeathVibe)
		{
			auto pl = PlayLayer::get();
			VibratePercent(deathVibeStrength);
			vibe = true;
			auto action = pl->runAction(CCSequence::create(CCDelayTime::create(deathVibeLength), CCCallFunc::create(pl, callfunc_selector(MyPlayerObject::StopVibe)), nullptr));
		}

	}
};

class $modify(PlayLayer)
{
	void updateProgressbar()
	{
		PlayLayer::updateProgressbar();
		auto pl = PlayLayer::get();
		percentage = pl->getCurrentPercentInt();

		if(IsVibePercent && !isLevelComplete)
		{
			VibratePercent(percentage);
		}

		if((percentage == 100) && (!isLevelComplete))
		{
			isLevelComplete = true;
			if(IsVibeComplete)
			{
				VibratePercent(completeVibeStrength);
				vibe = true;
				auto action = pl->runAction(CCSequence::create(CCDelayTime::create(2), CCCallFunc::create(pl, callfunc_selector(MyPlayerObject::StopVibe)), nullptr));
			}
		}
	}

	bool init(GJGameLevel * p0, bool p1, bool p2)
	{
		if(!PlayLayer::init(p0, p1, p2))
			return false;
		
		isLevelComplete = false;
		return true;
	}

	void resetLevelFromStart()
	{
		PlayLayer::resetLevelFromStart();
		isLevelComplete = false;
	}

	void resume()
	{
		PlayLayer::resume();

		if(!IsVibePercent)
		{
			float vp = percentage;
			VibratePercent(vp);
		}
		
	}

	void onExit()
	{
		PlayLayer::onExit();

		if(vibe)
			StopVibrate();
	}
};


class $modify (GJBaseGameLayer)
{
	void shakeCamera(float p0,float p1, float p2)
	{	
		GJBaseGameLayer::shakeCamera(p0, p1, p2);

		if(isVibeShake)
		{
			auto pl = PlayLayer::get();
			float duration = p0;
			float strength = p1;
			float interval = p2;
		
			if(strength > 5)
				strength = 5;

			strength = strength / 5 * 100;

			VibratePercent(strength);
			vibe = true;
			auto action = pl->runAction(CCSequence::create(CCDelayTime::create(duration), CCCallFunc::create(pl, callfunc_selector(MyPlayerObject::StopVibe)), nullptr));
		}
	}
};


//1520 shake trigger ID