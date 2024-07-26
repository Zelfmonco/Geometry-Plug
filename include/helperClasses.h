#include <string>
#include <vector>

class DeviceCmdAttr {
public:
	std::string FeatureDescriptor = "";
	unsigned int StepCount = 0;
	std::string ActuatorType = "";
	std::string SensorType = "";
	std::vector<int> SensorRange; // every two steps
	//std::vector<std::string> Endpoints;
};

class DeviceCmd {
public:
	std::string CmdType = "";
	std::string StopDeviceCmd = "";
	std::vector<DeviceCmdAttr> DeviceCmdAttributes;
};

class Device {
public:
	std::string DeviceName;
	unsigned int DeviceIndex;
	unsigned int DeviceMessageTimingGap = 0;
	std::string DeviceDisplayName = "";
	std::vector<DeviceCmd> DeviceMessages;
};

class Scalar {
public:
	unsigned int Index;
	double ScalarVal;
	std::string ActuatorType;
};
