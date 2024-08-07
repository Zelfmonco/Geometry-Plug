#include <string>
#include <functional>
#include <atomic>
#include <queue>
#include "ixwebsocket/IXWebSocket.h"
#include "messageHandler.h"
#include "log.h"

typedef msg::SensorReading SensorClass;

// Helper class to store devices and access them outside of the library.
class DeviceClass {
public:
	std::string deviceName;
	std::string displayName;
	std::vector<std::string> commandTypes;
	std::vector<std::string> sensorTypes;
	unsigned int deviceID;
};

// Main client class
class Client {
public:
	// Constructor which initialized websockets for Windows. Add an IFDEF depending on compilation OS for portability.
	Client(std::string url, unsigned int port) {
		lUrl = url;
		lPort = port;
	}
	Client(std::string url, unsigned int port, std::string logfile) {
		lUrl = url;
		lPort = port;
		logging = 1;
		if (!logfile.empty()) logInfo.init(logfile);
		else logInfo.init("log.txt");
	}
	~Client() {

	}

	void connect(void (*callFunc)(const mhl::Messages));
	void disconnect();
	
	// Atomic variables to store connection status. Can be accessed outside library too since atomic.
	std::atomic<bool> wsConnected{false};
	std::atomic<bool> isConnecting{false};
	std::atomic<bool> clientConnected{false};
	// Condition variables for the atomics, we want C++11 support
	std::condition_variable condClient;
	std::condition_variable condWs;

	// Public functions that send requests to server.
	void startScan();
	void stopScan();
	void requestDeviceList();
	void stopDevice(DeviceClass dev);
	void stopAllDevices();
	void sendScalar(DeviceClass dev, double str);
	void sensorRead(DeviceClass dev, int senIndex);
	void sensorSubscribe(DeviceClass dev, int senIndex);
	void sensorUnsubscribe(DeviceClass dev, int senIndex);

	// Setters
	void setUrl(std::string url) { lUrl = url; }
	void setPort(unsigned int port) { lPort = port; }

	// Mutex blocked function which grabs the currently connected devices and sensor reads.
	std::vector<DeviceClass> getDevices();
	SensorClass getSensors();
private:
	// URL variables for the websocket. 
	std::string lUrl;
	unsigned int lPort;

	int logging = 0;
	Logger logInfo;

	ix::WebSocket webSocket;

	// Message handler class, which takes messages, parses them and makes them to classes.
	mhl::Messages messageHandler;

	// Queue variable for passing received messages from server.
	std::queue<std::string> q;
	// Condition variabel to wait for received messages in the queue.
	std::condition_variable cond;
	// Mutex to ensure no race conditions.
	std::mutex msgMx;
	// Callback function for when a message is received and handled.
	std::function<void(const mhl::Messages&)> messageCallback;

	// Device and sensor class vector which is grabbed outside of the library.
	std::vector<DeviceClass> devices;
	SensorClass sensorData;

	void connectServer();
	void callbackFunction(const ix::WebSocketMessagePtr& msg);
	void messageHandling();
	void sendMessage(json msg, mhl::MessageTypes mType);
	void updateDevices();
	int findDevice(DeviceClass dev);
};
