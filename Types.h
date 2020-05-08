#ifndef TYPES_H
#define TYPES_H

#include <math.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <vector>
#include <string>

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
//typedef unsigned long long	uint64_t;
typedef signed char 		int8_t;
typedef signed short 		int16_t;
typedef signed int			int32_t;
typedef long int			int64_t;

// Physics constants
#define PHYS_C 299792458
#define INV_C 3.3356409519815204e-09

// Detector sizes [mm]
// Gaps are defined between two detector units. For example the distance between active
// detector part and the outer edge of SC16 casing would be (DS_SC1_GAP/2 + DS_SC16_GAP/2)
#define DS_SC1 123 // includes 0.5 mm gap on each side (according to specs SC1 has 122 mm)
#define DS_SC1_GAP 1
#define DS_SC16 500
#define DS_SC16_GAP 9
#define DS_SC16Z_PLASTIC 30
#define DS_SC16Z_CASE 6
#define SC1_DETECTABLE_MUON_DISTANCE 10

//Last positions in these arrays are always related to default values (in case of errors while reading)
//detectors
#define N_PLANKS 101
#define N_CHAMBERS 7
#define N_SIGNALS 3

#define N_SCMODULES 103
#define N_PIXELSX 4
#define N_PIXELSY 4
#define N_PIXELS (N_PIXELSX*N_PIXELSY)

//daq electronics
#define N_TDC_CHANNELS 129
#define N_GEO_ADDRESSES 33

#define N_LSTMODULES 62
#define N_LSTS_INCM 5
#define N_CMS_INPU 12
#define N_WORDS_INLST 16
#define N_BITS_INWORD 16
#define N_BITS_INLST (N_BITS_INWORD*N_WORDS_INLST)
#define N_LSTCHANNELS (N_CMS_INPU*N_LSTS_INCM*N_WORDS_INLST)

#define N_ABS_INLST 4
#define N_LSTPORTS_INAB 8
#define N_LSTCHANNELS_INPORT 8

#define N_LSTCHAMBERS 6
#define N_LSTANODES (N_LSTCHANNELS_INPORT*N_LSTCHAMBERS)
#define N_LSTPADBIT_OFFSET 64
#define N_LSTPADS (N_BITS_INLST-N_LSTPADBIT_OFFSET)
#define N_LSTPAD_ROWS (N_LSTPADS/N_LSTCHAMBERS)

// All dimensions are in mm
#define LST_N_TUBES_INCHAMBER 16
#define LST_TUBE_INNERSIZE 9
#define LST_CHAMBER_EXT_WALL_THICKNESS 2.2
#define LST_CHAMBER_INT_WALL_THICKNESS 0.61
#define LST_CHAMBER_WIDTH (LST_TUBE_INNERSIZE*LST_N_TUBES_INCHAMBER + 2*LST_CHAMBER_EXT_WALL_THICKNESS + (2*(LST_N_TUBES_INCHAMBER-1)*LST_CHAMBER_INT_WALL_THICKNESS))
#define LST_PAD_WIDTH LST_CHAMBER_WIDTH
#define LST_PAD_LENGTH 87.5
#define LST_WIDTH (N_LSTCHAMBERS*LST_CHAMBER_WIDTH)
#define LST_LENGTH (LST_PAD_LENGTH*N_LSTPADS/N_LSTCHAMBERS)

// half-difference between coordinates, which are still considered the same plane of detectors
#define SAME_PLANE_EPSILON 5
#define DOUBLE_EPSILON 0.000001

//analysis (by Pasi)
#define A_WIDTH 3150.0  // this is in channels, float because it is a divisor.
#define A_WIDTH_MAXDIFF ((A_WIDTH/100.0)*100)// this is in channels, Too small value may limit the angle. Multiplied value 25mm corresponds to angle cutoff of 45 degrees in anode direction.
#define MAX_posNA_posFA_DIFF 600 // this is in mm.
#define MAX_XY_DIFF          600 // this is in mm.

// enum types definitions
enum EDeviceType{v767, v830, v1190, v1495, v14950, v14951, NA};
enum ERawDataType_t{rdt_OLD, rdt_NEW_, rdt_NEW1};
enum EDetector{PLANK, SC, LST, EMPTY};

typedef std::vector<int32_t> vector1d;
typedef std::vector<vector1d> vector2d;
typedef std::vector<vector2d> vector3d;

template <class T1, class T2> class TNameVal{
public:
	std::string name;
	T1 val;
	T2* ptr;
	TNameVal();
};
template <class T1, class T2> TNameVal<T1,T2>::TNameVal(){
	name="";
	val=0;
	ptr=NULL;
}
template <class T1, class T2> class Tv1NameVal{
public:
	typedef std::vector<TNameVal<T1,T2> > v1;
};
template <class T1, class T2> class Tv2NameVal{
public:
	typedef std::vector<typename Tv1NameVal<T1,T2>::v1> v2;
};
template <class T1, class T2> class Tv3NameVal{
public:
	typedef std::vector<typename Tv2NameVal<T1,T2>::v2> v3;
};

template <class T1> class v1d{
public:
	typedef std::vector<T1> v;
};
template <class T1> class v2d{
public:
	typedef std::vector<typename v1d<T1>::v> v;
};
template <class T1> class v3d{
public:
	typedef std::vector<typename v2d<T1>::v> v;
};

#endif
