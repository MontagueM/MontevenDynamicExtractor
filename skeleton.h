#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include <fbxsdk.h>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "helpers.h"

struct DefaultObjectSpaceTransform
{
	std::vector<float> rotation;
	std::vector<float> location;
	float scale;
};


struct DefaultInverseObjectSpaceTransform : public DefaultObjectSpaceTransform
{
};


class Node
{
private:
public:
	uint32_t hash;
	std::string name;
	int32_t parentNodeIndex;
	int32_t firstChildNodeIndex;
	int32_t nextSiblingNodeIndex;
	DefaultObjectSpaceTransform* dost;
	DefaultInverseObjectSpaceTransform* diost;
	FbxNode* fbxNode = nullptr;
};

class Skeleton : public File
{
private:
	//std::unordered_map<uint32_t, std::string> boneNames;

	//void getSkeletonNames();
	std::vector<Node*> parseSkeleton();
public:
	Skeleton(std::string x, std::string pkgsPath) : File(x, pkgsPath) {};

	std::vector<Node*> get();

private:
	std::unordered_map<uint32_t, std::string> boneNames =
	{
		{3848821786, "Pedestal"},
		{1857732807, "Pelvis"},
		{162487657, "Thigh.L"},
		{458076469, "Calf.L"},
		{1565559567, "Foot.L"},
		{988249757, "Toe.L"},
		{2362576799, "Thigh.R"},
		{1061868683, "Calf.R"},
		{1575008697, "Foot.R"},
		{847516523, "Toe.R"},
		{2728809121, "Spine_1"},
		{2728809122, "Spine_2"},
		{2728809123, "Spine_3"},
		{976064003, "Clav.L"},
		{2720736209, "UpperArm.L"},
		{68516489, "ForeArm.L"},
		{1741390732, "Hand.L"},
		{262875985, "Index_1.L"},
		{262875986, "Index_2.L"},
		{262875987, "Index_3.L"},
		{2608135088, "Middle_1.L"},
		{2608135091, "Middle_2.L"},
		{2608135090, "Middle_3.L"},
		{2072385340, "Pinky_1.L"},
		{2072385343, "Pinky_2.L"},
		{2072385342, "Pinky_3.L"},
		{2592422609, "Ring_1.L"},
		{2592422610, "Ring_2.L"},
		{2592422611, "Ring_3.L"},
		{4256326025, "Thumb_1.L"},
		{4256326026, "Thumb_2.L"},
		{4256326027, "Thumb_3.L"},
		{3940624003, "Wrist_Twist_Fixup.L"},
		{3886930732, "Shoulder_Twist_Fixup.L"},
		{280710669, "Neck_1"},
		{280710670, "Neck_2"},
		{1087804030, "Head"},
		{375707561, "Clav.R"},
		{1348403735, "UpperArm.R"},
		{3441260707, "ForeArm.R"},
		{3932921310, "Hand.R"},
		{4056396387, "Index_1.R"},
		{4056396384, "Index_2.R"},
		{4056396385, "Index_3.R"},
		{2527141218, "Middle_1.R"},
		{2527141217, "Middle_2.R"},
		{2527141216, "Middle_3.R"},
		{523186114, "Pinky_1.R"},
		{523186113, "Pinky_2.R"},
		{523186112, "Pinky_3.R"},
		{238325807, "Ring_1.R"},
		{238325804, "Ring_2.R"},
		{238325805, "Ring_3.R"},
		{2453682435, "Thumb_1.R"},
		{2453682432, "Thumb_2.R"},
		{2453682433, "Thumb_3.R"},
		{3470599597, "Wrist_Twist_Fixup.R"},
		{2133354418, "Shoulder_Twist_Fixup.R"},
		{36783236, "Jaw"},
		{1287540002, "Eye.L"},
		{542709204, "Eye.R"},
		{2554408643, "SecondaryClav.R"},
		{57347777, "SecondaryUpperArm.R"},
		{288356485, "SecondaryForeArm.R"},
		{3857698172, "SecondaryHand.R"},
		{2912171333, "SecondaryIndex_1.R"},
		{2707704166, "SecondaryIndex_2.R"},
		{1122474635, "SecondaryIndex_3.R"},
		{4043201640, "SecondaryPinky_1.R"},
		{1826672811, "SecondaryPinky_2.R"},
		{283722886, "SecondaryPinky_3.R"},
		{979034917, "SecondaryThumb_1.L"},
		{3666389190, "SecondaryThumb_2.R"},
		{1380302827, "SecondaryThumb_3.R"},
		{3202869035, "Accessory_1"},
		{3615189242, "Accessory_2"},
		{2569216089, "Accessory_3"},
		{1248286271, "Accessory_4"},
		{2091074864, "Accessory_5"},
		{3325399077, "SecondaryClav.L"},
		{2952484955, "SecondaryUpperArm.L"},
		{1341214947, "SecondaryForeArm.L"},
		{2623089450, "SecondaryHand.L"},
		{910107611, "SecondaryIndex_1.L"},
		{2517625560, "SecondaryIndex_2.L"},
		{556222677, "SecondaryIndex_3.L"},
		{2632325498, "SecondaryPinky_1.L"},
		{1484970841, "SecondaryPinky_2.L"},
		{967401372, "SecondaryPinky_3.L"},
		{795803619, "SecondaryThumb_3.L"},
		{1900961856, "SecondaryThumb_2.L"},
		{3092150907, "Nose"},
		{862887812, "UpperLip_3"},
		{2996271826, "Jaw"},
		{3707180062, "Chin"},
		{265445679, "LowerLip_3"},
		{4280798516, "LowerLip_4"},
		{2384109844, "LowerLip_5"},
		{3342328385, "Jawline_2.L"},
		{2698715374, "LowerLip_2"},
		{1462169378, "LowerLip_1"},
		{3377068855, "Jawline_2.R"},
		{2137193665, "Brow_1.L"},
		{2137193666, "Brow_2.L"},
		{2137193667, "Brow_3.L"},
		{1620158747, "Cheek_1.L"},
		{1620158744, "Jawline_1.L"},
		{2405916421, "UnderEye.L"},
		{2405916422, "Cheekbone.L"},
		{1472908839, "LowerEye.L"},
		{4064364946, "Nostril_1.L"},
		{839315438, "UpperEyelid.L"},
		{2715264387, "UpperLip_4"},
		{1101907617, "UpperLip_5"},
		{2219496000, "Cheek_2.L"},
		{3999434879, "Nostril_2.L"},
		{4107497571, "Brow_1.R"},
		{4107497568, "Brow_2.R"},
		{4107497569, "Brow_3.R"},
		{2998991081, "Cheek_1.R"},
		{2998991082, "Jawline_1.R"},
		{2506856279, "UnderEye.R"},
		{2506856276, "Cheekbone.R"},
		{869754765, "LowerEyelid.R"},
		{3097505828, "Nostril_1.R"},
		{3597451020, "UpperEyelid.R"},
		{1040619693, "UpperLip_2"},
		{3184076039, "UpperLip_1"},
		{1855182198, "Cheek_2.R"},
		{3628633877, "Nostril_2.R"},
		{2657650232, "Tongue_1"},
		{2657650235, "Tongue_2"},
		{3289385034, "GunBase"},
		{670643234, "Hammer"},
		{2621268890, "MagRelease"},
		{2801973004, "Trigger"},
		{1065563967, "CraneRotate"},
		{1065563964, "CraneExtend"},
		{3172630974, "Cylinder"},
		{179750274, "Magazine"},
	};
};
