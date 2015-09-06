#ifndef VOXEL_ROUTINES
#define VOXEL_ROUTINES

#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>


#define VOXEL_SIZE_EXP	3
#define VOXEL_SIZE (1 << VOXEL_SIZE_EXP)
#define VOXEL_PX_SIZE 4

void setupMatrix(void);
void fillMatrixWithRandomData(void);
short readVoxelFromMatrix(short x, short y, short z);
void writeVoxelToMatrix(short x, short y, short z, short value);
void buildPointListFromMatrix(void);
void deleteMatrix(void);
void deletePointList(void);
void rotatePointsOnAxisY(UWORD angle);
void drawPointListToViewport(struct RastPort *rp, UWORD x_offset);

void buildLinesListAsCube(UBYTE figure_index);
void drawLinesListToViewport(struct RastPort *rp, UWORD x_offset);

#endif // #ifndef VOXEL_ROUTINES