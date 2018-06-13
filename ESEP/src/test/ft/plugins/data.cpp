#include "test/ft/plugins/analyser.h"

namespace esep { namespace test { namespace functional { namespace plugins {

namespace
{
	const uint16_t ITEM_101_1[] = {
		0, 5665,
		2, 9820,
		3, 14164,
		5, 20916,
		6, 26110,
		8, 30359,
		9, 35222,
		11, 40227,
		12, 52173,
		14, 54156,
		15, 54486,
		17, 54250,
		18, 54014,
		20, 53683,
		21, 53495,
		23, 53306,
		24, 53495,
		27, 53636,
		29, 53589,
		30, 53353,
		32, 53164,
		33, 53306,
		35, 53778,
		36, 53920,
		38, 54014,
		39, 53731,
		41, 53495,
		42, 52597,
		44, 50567,
		45, 47970,
		47, 46176,
		48, 45468,
		50, 45279,
		51, 45232,
		53, 45279,
		54, 45137,
		56, 45043,
		57, 44996,
		60, 45043,
		62, 45090,
		63, 45043,
		66, 44996,
		68, 45090,
		69, 45751,
		71, 50331,
		72, 53589,
		74, 53872,
		75, 53967,
		78, 54061,
		80, 53967,
		81, 53920,
		83, 53967,
		84, 53920,
		86, 54156,
		89, 53967,
		90, 53731,
		92, 53683,
		93, 53542,
		95, 53495,
		98, 52928,
		99, 51559,
		101, 47404,
		102, 40746,
		104, 38480,
		105, 37347,
		107, 36686,
		108, 36450,
		110, 36497,
		111, 36591,
		113, 36497,
		114, 36591,
		116, 36686,
		117, 36780,
		119, 36733,
		120, 36497,
		122, 35742,
		123, 35033,
		125, 34750,
		126, 48443,
		128, 54156,
		129, 54014,
		131, 53589,
		132, 53542,
		134, 53400,
		135, 53353,
		137, 53070,
		138, 52928,
		140, 53117,
		143, 53258,
		144, 53542,
		146, 53683,
		147, 53495,
		149, 53258,
		150, 52975,
		152, 52786,
		155, 52220,
		156, 51134,
		158, 49765,
		159, 48537,
		161, 47829,
		162, 47026,
		164, 46223,
		165, 45610,
		167, 45090,
		168, 45043,
		170, 44996,
		171, 44949,
		173, 44996,
		174, 44949,
		176, 44901,
		177, 44760,
		179, 44618,
		182, 44524,
		183, 44571,
		185, 44713,
		186, 44618,
		189, 44713,
		191, 44854,
		194, 45043,
		195, 45185,
		197, 45468,
		198, 45704,
		200, 45562,
		201, 45137,
		203, 44996,
		204, 44901,
		206, 44854,
		207, 45090,
		209, 45232,
		210, 45279,
		212, 45185,
		213, 45043,
		216, 44996,
		218, 44949,
		219, 44996,
		221, 44949,
		222, 44854,
		224, 44949,
		225, 45043,
		228, 44996,
		230, 45043,
		231, 45090,
		233, 45468,
		234, 46035,
		236, 46837,
		237, 47829,
		239, 48962,
		240, 50048,
		242, 51323,
		243, 52409,
		245, 53400,
		246, 54014,
		248, 54250,
		249, 53778,
		251, 53117,
		252, 52975,
		254, 53117,
		255, 53164,
		257, 53495,
		258, 53731,
		260, 53825,
		261, 53636,
		263, 53306,
		264, 53070,
		266, 53211,
		267, 53778,
		269, 54156,
		270, 42729,
		272, 36072,
		273, 36403,
		275, 36497,
		276, 36733,
		278, 36780,
		279, 36969,
		281, 37158,
		282, 37300,
		285, 37347,
		287, 37064,
		290, 37300,
		291, 37725,
		293, 38574,
		294, 40510,
		296, 45374,
		297, 49528,
		299, 51606,
		300, 52125,
		302, 52361,
		303, 52456,
		305, 52739,
		306, 53164,
		308, 53542,
		309, 53778,
		311, 53636,
		314, 53778,
		315, 53589,
		317, 53542,
		318, 53636,
		320, 53778,
		321, 53731,
		324, 53542,
		326, 52361,
		327, 48537,
		329, 45893,
		330, 45562,
		332, 45326,
		333, 45279,
		335, 45374,
		336, 45279,
		338, 45326,
		339, 45232,
		341, 45326,
		345, 45515,
		347, 45421,
		348, 45468,
		350, 45751,
		351, 46365,
		353, 47215,
		354, 49151,
		356, 51559,
		357, 52881,
		359, 53353,
		360, 53683,
		362, 53778,
		363, 53683,
		365, 53542,
		366, 53683,
		369, 53731,
		371, 53683,
		372, 53495,
		374, 53353,
		375, 53164,
		377, 53353,
		378, 53542,
		380, 53825,
		381, 54297,
		383, 33145
	};

	const uint16_t ITEM_110_2[] = {
			0, 11709,
			1, 15156,
			3, 15675,
			4, 16383,
			6, 16903,
			7, 18272,
			9, 20585,
			10, 23985,
			12, 27951,
			13, 31114,
			15, 34656,
			16, 37016,
			18, 39849,
			19, 43532,
			21, 48584,
			22, 52881,
			24, 53872,
			25, 53967,
			27, 54061,
			28, 54014,
			30, 53920,
			31, 53825,
			33, 53731,
			34, 53683,
			36, 53495,
			37, 53589,
			39, 53731,
			40, 53778,
			42, 53683,
			43, 53589,
			45, 53353,
			46, 53211,
			48, 52692,
			49, 51228,
			51, 48867,
			52, 46648,
			54, 45657,
			55, 45326,
			57, 45279,
			58, 45090,
			60, 45185,
			61, 45137,
			63, 45043,
			64, 44996,
			66, 44854,
			67, 44760,
			70, 44618,
			72, 44476,
			73, 44665,
			75, 45515,
			76, 47168,
			78, 49623,
			79, 52361,
			81, 53164,
			82, 53258,
			84, 53117,
			85, 53070,
			88, 53022,
			90, 52928,
			91, 53022,
			93, 52881,
			94, 52834,
			96, 52928,
			97, 52834,
			99, 52692,
			100, 52597,
			102, 52361,
			103, 52125,
			105, 51417,
			106, 49812,
			108, 47451,
			109, 45846,
			111, 44760,
			112, 44240,
			114, 44099,
			115, 44051,
			120, 44004,
			121, 43957,
			123, 43815,
			124, 43768,
			126, 43627,
			127, 43579,
			129, 43438,
			130, 43863,
			132, 44713,
			133, 47262,
			135, 50709,
			136, 52314,
			138, 52692,
			139, 52550,
			141, 52456,
			142, 52409,
			144, 52503,
			145, 52409,
			147, 52503,
			150, 52409,
			151, 52361,
			154, 52220,
			157, 52031,
			159, 51795,
			160, 51275,
			162, 50237,
			163, 48631,
			165, 46318,
			166, 43060,
			168, 40416,
			169, 38669,
			171, 37347,
			172, 36308,
			174, 35600,
			175, 35411,
			177, 35269,
			178, 35364,
			180, 35317,
			181, 35128,
			184, 35033,
			186, 34986,
			187, 34939,
			189, 35128,
			190, 35317,
			192, 35081,
			193, 34797,
			195, 34750,
			196, 34703,
			198, 34750,
			199, 34656,
			201, 34844,
			202, 34797,
			204, 34986,
			205, 34703,
			207, 34561,
			208, 34467,
			210, 34420,
			213, 34467,
			214, 34608,
			216, 34844,
			217, 34797,
			220, 34939,
			222, 34656,
			223, 34844,
			225, 34892,
			226, 35033,
			228, 34986,
			229, 35081,
			231, 34986,
			232, 35081,
			234, 35128,
			235, 35222,
			237, 35553,
			238, 36025,
			240, 36969,
			241, 38244,
			243, 39660,
			244, 42777,
			246, 46979,
			247, 48018,
			249, 48443,
			250, 49340,
			252, 50142,
			253, 50709,
			255, 50992,
			256, 51181,
			258, 51134,
			259, 51039,
			261, 50992,
			262, 51087,
			264, 51181,
			265, 51039,
			267, 50945,
			268, 50898,
			270, 50992,
			271, 51039,
			273, 51134,
			274, 51039,
			276, 50898,
			277, 49576,
			279, 46412,
			280, 43296,
			282, 42777,
			283, 42729,
			285, 42588,
			288, 42635,
			291, 42588,
			292, 42682,
			294, 42635,
			295, 42588,
			297, 42824,
			298, 43154,
			300, 43532,
			301, 44193,
			303, 45515,
			304, 47073,
			306, 48206,
			307, 49481,
			309, 50331,
			310, 50756,
			312, 50992,
			318, 51087,
			319, 51228,
			321, 51370,
			322, 51464,
			324, 51228,
			325, 51181,
			327, 50945,
			330, 50756,
			331, 50709,
			333, 49906,
			334, 47970,
			336, 44524,
			337, 42966,
			339, 42682,
			340, 42541,
			342, 42446,
			343, 42305,
			345, 42352,
			346, 42446,
			349, 42541,
			352, 42588,
			355, 42918,
			357, 43579,
			358, 45137,
			360, 46743,
			361, 48301,
			363, 49670,
			364, 50378,
			366, 50567,
			367, 50803,
			369, 50992,
			370, 50945,
			372, 50803,
			373, 50614,
			375, 50520,
			376, 50567,
			378, 50614,
			381, 50473,
			382, 50237,
			384, 50142,
			385, 50331,
			387, 50662,
			388, 38386
	};

	const uint16_t ITEM_010_1[] = {
			0, 3635,
			1, 15108,
			3, 21955,
			4, 4579,
			6, 15533,
			7, 49812,
			9, 50142,
			10, 49576,
			12, 49340,
			13, 49198,
			15, 48962,
			16, 48915,
			18, 49009,
			19, 49292,
			21, 49151,
			22, 49104,
			24, 49245,
			25, 49434,
			30, 49387,
			31, 49056,
			33, 48726,
			34, 48395,
			36, 48159,
			37, 47404,
			39, 45846,
			40, 40133,
			42, 33711,
			43, 32956,
			45, 32767,
			48, 32956,
			54, 32861,
			58, 32814,
			60, 32625,
			61, 32531,
			63, 32767,
			64, 33711,
			66, 44618,
			67, 48726,
			69, 49245,
			72, 49009,
			73, 49056,
			75, 49104,
			76, 49340,
			78, 49292,
			79, 49528,
			82, 49387,
			84, 49198,
			85, 49056,
			87, 49387,
			88, 49151,
			90, 48915,
			91, 48726,
			93, 48348,
			94, 48065,
			96, 46837,
			97, 44760,
			99, 42871,
			100, 41974,
			102, 41738,
			103, 41643,
			105, 41407,
			106, 41549,
			108, 41502,
			111, 41549,
			112, 41360,
			114, 41596,
			115, 41643,
			117, 41785,
			118, 41691,
			120, 41313,
			121, 41030,
			123, 41266,
			124, 45515,
			126, 49292,
			127, 49528,
			129, 49434,
			130, 49198,
			132, 49056,
			133, 48962,
			135, 49198,
			136, 49292,
			138, 49340,
			139, 49434,
			141, 49670,
			142, 49717,
			144, 49906,
			145, 49859,
			147, 50237,
			148, 50331,
			150, 49953,
			151, 49387,
			153, 48301,
			154, 46554,
			156, 43910,
			157, 40227,
			159, 37725,
			160, 35978,
			162, 34656,
			163, 33853,
			165, 33475,
			166, 33334,
			168, 33286,
			171, 33192,
			172, 33003,
			174, 32909,
			177, 33050,
			178, 33239,
			180, 33334,
			181, 33239,
			183, 33428,
			189, 33381,
			190, 33239,
			192, 33286,
			198, 33239,
			199, 33050,
			202, 33239,
			204, 33145,
			207, 33098,
			208, 33334,
			210, 33475,
			211, 33522,
			213, 33475,
			214, 33570,
			216, 33759,
			217, 33806,
			220, 34231,
			222, 34514,
			223, 34608,
			225, 34514,
			226, 33947,
			228, 33570,
			229, 33050,
			231, 32673,
			232, 32578,
			234, 32909,
			235, 33711,
			237, 35930,
			238, 37961,
			240, 40180,
			241, 42635,
			243, 45279,
			244, 47451,
			246, 48065,
			247, 48820,
			249, 49151,
			250, 49434,
			252, 49481,
			253, 49623,
			255, 49812,
			256, 50048,
			259, 50095,
			261, 50048,
			262, 49906,
			264, 49953,
			265, 50001,
			267, 49859,
			268, 49670,
			271, 48962,
			273, 47120,
			274, 43249,
			276, 41974,
			277, 42068,
			279, 42021,
			280, 42116,
			282, 42210,
			285, 42257,
			288, 42068,
			289, 42021,
			292, 42116,
			295, 42305,
			297, 42918,
			298, 44004,
			300, 46082,
			301, 48159,
			303, 49340,
			304, 49859,
			306, 50189,
			307, 50284,
			309, 50142,
			310, 50284,
			312, 50237,
			315, 50284,
			316, 50520,
			318, 50567,
			321, 50614,
			322, 50756,
			325, 50426,
			328, 49953,
			330, 39566,
			331, 33664,
			333, 33617,
			334, 33428,
			337, 33711,
			339, 34136,
			340, 34514,
			342, 34608,
			343, 34844,
			345, 34986,
			346, 35269,
			348, 35505,
			349, 35836,
			351, 35978,
			352, 36025,
			354, 37111,
			355, 40085,
			357, 45468,
			358, 48773,
			360, 50142,
			361, 50567,
			363, 50756,
			364, 50614,
			366, 50473,
			367, 50426,
			369, 50614,
			370, 50803,
			373, 50709,
			375, 50662,
			376, 50756,
			378, 50803,
			379, 50709,
			381, 50756,
			382, 50851,
			384, 51323,
			385, 27432
	};

#define CURRENT ITEM_110_2
}

uint16_t Analyser::RAW_DATA(size_t i) { return CURRENT[i]; }
size_t Analyser::N_DATA(void) { static const size_t r = (sizeof(CURRENT) / (2 * sizeof(*CURRENT))); return r; }

}}}}