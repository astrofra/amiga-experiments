#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

#include "board.h"
#include "screen_size.h"
#include "voxel_routines.h"
#include "color_routines.h"
#include "bitmap_routines.h"
#include "cosine_table.h"

extern struct GfxBase *GfxBase;
extern struct ViewPort view_port1;
extern int dbuffer_offset_2;

short 	*matrix = NULL;
short	*point_list = NULL;
short	*t_point_list = NULL;
UWORD 	point_list_size = 0;
UWORD 	*list_size_by_slice = NULL;

int squareDistBetweenPoints(short x, short y, short z, short a, short b, short c)
{
	x -= a;
	y -= b;
	z -= c;

	return (x * x + y * y + z * z);
}

/*
	Create a static voxel cube.
	The purpose is only to store the data
	temporarily and buid a list of 3D points.
*/
void setupMatrix(void)
{
	matrix = (short *)AllocMem(sizeof(short) * VOXEL_SIZE * VOXEL_SIZE * VOXEL_SIZE, MEMF_CLEAR);
}

/*
	Fills the voxel cube
	with random values.
*/
void fillMatrixWithRandomData(void)
{
	short x,y,z,value;

	for(y = 0; y < VOXEL_SIZE; y++)
	{
		for(z = 0; z < VOXEL_SIZE; z++)
		{
			for(x = 0; x < VOXEL_SIZE; x++)
			{
				value = squareDistBetweenPoints(x, y, z, VOXEL_SIZE >> 1, VOXEL_SIZE >> 1, VOXEL_SIZE >> 1);
				if (value > (VOXEL_SIZE * 3) - 3 && value < VOXEL_SIZE * 3)
					writeVoxelToMatrix(x,y,z,1);
			}
		}
	}
}

/*
	Get a voxel from its 3D coordinates.
*/
short __inline readVoxelFromMatrix(short x, short y, short z)
{
	UWORD v_idx;
	v_idx = x + (y << VOXEL_SIZE_EXP) + (z << (VOXEL_SIZE_EXP + VOXEL_SIZE_EXP));
	return matrix[v_idx];
}

void __inline writeVoxelToMatrix(short x, short y, short z, short value)
{
	UWORD v_idx;
	v_idx = x + (y << VOXEL_SIZE_EXP) + (z << (VOXEL_SIZE_EXP + VOXEL_SIZE_EXP));
	matrix[v_idx] = value;
}

/*
	Build a list of points
	from the voxel cube,
	by skipping the empty voxels.
*/
void buildPointListFromMatrix(void)
{
	short loop, x, y, z, voxel, list_idx = 0;
	UWORD slice_size;

	printf("buildPointListFromMatrix()\n");

	/*
		Initialize an array that will store
		how many points there are in each slice
	*/
	list_size_by_slice = (UWORD *)AllocMem(sizeof(UWORD) * VOXEL_SIZE, MEMF_CLEAR);
	/*
		Creates a first list to store
		the list of points.
	*/
	point_list_size = 0;
	for(loop = 0; loop < VOXEL_SIZE * VOXEL_SIZE * VOXEL_SIZE; loop++)
		if (matrix[loop] > 0)
			point_list_size++;

	point_list = (short *)AllocMem(sizeof(short) * point_list_size * 4, MEMF_CLEAR);

	for(y = 0; y < VOXEL_SIZE; y++)
	{
		slice_size = 0;
		for(z = 0; z < VOXEL_SIZE; z++)
		{
			for(x = 0; x < VOXEL_SIZE; x++)
			{
				voxel = readVoxelFromMatrix(x,y,z);
				if (voxel > 0)
				{
					point_list[list_idx++] = x - (VOXEL_SIZE >> 1);
					point_list[list_idx++] = y - (VOXEL_SIZE >> 1);
					point_list[list_idx++] = z - (VOXEL_SIZE >> 1);
					point_list[list_idx++] = voxel;
					slice_size++;
				}
			}
		}

		list_size_by_slice[y] = (UWORD)slice_size;
		printf("list_size_by_slice[%d] = %d\n", y, slice_size);
	}

	/*
		Creates a second list, leave it empty,
		for further geometry transformations.
	*/
	t_point_list = (short *)AllocMem(sizeof(short) * point_list_size * 4, MEMF_CLEAR);

	printf("buildPointListFromMatrix() found %d points\n", (list_idx >> 2));
}

#define VOXEL_ADD_POINT(x,y,z,v_col)	point_list[list_idx++] = x - (VOXEL_SIZE >> 1); \
										point_list[list_idx++] = y - (VOXEL_SIZE >> 1); \
										point_list[list_idx++] = z - (VOXEL_SIZE >> 1); \
										point_list[list_idx++] = v_col;

void buildLinesListAsCube(void)
{
	short list_idx = 0;
	point_list_size = 9;

	point_list = (short *)AllocMem(sizeof(short) * point_list_size * 4, MEMF_CLEAR);
	VOXEL_ADD_POINT(0,0,0,1);
	VOXEL_ADD_POINT(0,0,VOXEL_SIZE,1);
	VOXEL_ADD_POINT(VOXEL_SIZE,0,VOXEL_SIZE,1);
	VOXEL_ADD_POINT(VOXEL_SIZE,0,0,1);
	VOXEL_ADD_POINT(VOXEL_SIZE,VOXEL_SIZE,0,1);
	VOXEL_ADD_POINT(VOXEL_SIZE,VOXEL_SIZE,VOXEL_SIZE,1);
	VOXEL_ADD_POINT(0,VOXEL_SIZE,VOXEL_SIZE,1);
	VOXEL_ADD_POINT(0,VOXEL_SIZE,0,1);
	VOXEL_ADD_POINT(0,0,0,1);	

	t_point_list = (short *)AllocMem(sizeof(short) * point_list_size * 4, MEMF_CLEAR);
}

void deleteMatrix(void)
{
	if (matrix != NULL)
		FreeMem(matrix, sizeof(short) * VOXEL_SIZE * VOXEL_SIZE * VOXEL_SIZE);
}

void deletePointList(void)
{
	if (point_list != NULL)
		FreeMem(point_list, sizeof(short) * point_list_size * 4);

	if (list_size_by_slice != NULL)
		FreeMem(list_size_by_slice, sizeof(UWORD) * VOXEL_SIZE);

	if (t_point_list != NULL)
		FreeMem(t_point_list, sizeof(short) * point_list_size * 4);
}


/*
	Rotate the point list
	on its Z axis.
*/
void __inline rotatePointsOnAxisY(UWORD angle)
{
	short loop;

	UWORD list_idx = 0;
	for(loop = 0; loop < point_list_size << 2; loop += 4)
	{
		//	x' = x cos f - y sin f
		t_point_list[list_idx++] = (point_list[loop] * tcos[angle] - point_list[loop + 2] * tsin[angle]) >> 6;
		t_point_list[list_idx++] = point_list[loop + 1] << 3;
		//	y' = y cos f + x sin f
		t_point_list[list_idx++] = (point_list[loop] * tsin[angle] + point_list[loop + 2] * tcos[angle]) >> 6;
		t_point_list[list_idx++] = point_list[loop + 3];
	}
}

void __inline drawPointListToViewport(struct RastPort *rp, UWORD x_offset)
{
	short loop;

	SetAPen(rp, 1);
	for(loop = 0; loop < point_list_size << 2; loop += 4)
	{
		WritePixel(rp, t_point_list[loop] + (DEFAULT_WIDTH >> 1) + x_offset, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1));

		// RectFill(rp, t_point_list[loop] + (DISPL_WIDTH1 >> 1) - VOXEL_PX_SIZE, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1) - VOXEL_PX_SIZE + dbuffer_offset_2,
		// 			t_point_list[loop] + (DEFAULT_WIDTH >> 1) + VOXEL_PX_SIZE, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1) + VOXEL_PX_SIZE + dbuffer_offset_2);

		// RectFill(rp, t_point_list[loop] + (DISPL_WIDTH1 >> 1) - VOXEL_PX_SIZE + dbuffer_offset_2, t_point_list[loop + 1] + (DISPL_HEIGHT1 >> 1) - VOXEL_PX_SIZE,
		// 			t_point_list[loop] + (DEFAULT_WIDTH >> 1) + VOXEL_PX_SIZE + dbuffer_offset_2, t_point_list[loop + 1] + (DISPL_HEIGHT1 >> 1) + VOXEL_PX_SIZE);		

		// printf("(%d,%d), ", t_point_list[loop], t_point_list[loop + 1]);
	}
}

void __inline drawLinesListToViewport(struct RastPort *rp, UWORD x_offset)
{
	short loop = 0;

	SetAPen(rp, 1);
	Move(rp, t_point_list[loop] + (DEFAULT_WIDTH >> 1) + x_offset, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1));
	for(loop = 4; loop < point_list_size << 2; loop += 4)
	{
		Draw(rp, t_point_list[loop] + (DEFAULT_WIDTH >> 1) + x_offset, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1));

		// RectFill(rp, t_point_list[loop] + (DISPL_WIDTH1 >> 1) - VOXEL_PX_SIZE, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1) - VOXEL_PX_SIZE + dbuffer_offset_2,
		// 			t_point_list[loop] + (DEFAULT_WIDTH >> 1) + VOXEL_PX_SIZE, t_point_list[loop + 1] + (t_point_list[loop + 2] >> 2) + (DISPL_HEIGHT1 >> 1) + VOXEL_PX_SIZE + dbuffer_offset_2);

		// RectFill(rp, t_point_list[loop] + (DISPL_WIDTH1 >> 1) - VOXEL_PX_SIZE + dbuffer_offset_2, t_point_list[loop + 1] + (DISPL_HEIGHT1 >> 1) - VOXEL_PX_SIZE,
		// 			t_point_list[loop] + (DEFAULT_WIDTH >> 1) + VOXEL_PX_SIZE + dbuffer_offset_2, t_point_list[loop + 1] + (DISPL_HEIGHT1 >> 1) + VOXEL_PX_SIZE);		

		// printf("(%d,%d), ", t_point_list[loop], t_point_list[loop + 1]);
	}
}
