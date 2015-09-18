/*
    Misc bitmap routines 
*/

#include "includes.prl"
#include "tinfl.c"

extern struct DosLibrary *DOSBase;
extern struct GfxBase *GfxBase;

extern size_t tinfl_decompress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);

/*
  Image loading 
*/

PLANEPTR load_getchipmem(UBYTE *name, ULONG size)
{
  BPTR fileHandle;
  PLANEPTR mem;

  if (!(fileHandle = Open(name, MODE_OLDFILE)))
    return (NULL);

  if (!(mem = AllocMem(size, MEMF_CHIP)))
    return (NULL);

  Read(fileHandle, mem, size);
  Close(fileHandle);

  return (mem);
}

PLANEPTR load_zlib_getchipmem(UBYTE *name, ULONG input_size, ULONG output_size)
{
  BPTR fileHandle;
  PLANEPTR temp_mem, unzip_mem, tinfl_return_value;

  if (!(fileHandle = Open(name, MODE_OLDFILE)))
    return (NULL);

  if (!(temp_mem = AllocMem(input_size, MEMF_PUBLIC)))
    return (NULL);

  Read(fileHandle, temp_mem, input_size);

  if (!(unzip_mem = AllocMem(output_size, MEMF_CHIP)))
  {
    FreeMem(temp_mem, input_size);
    return (NULL);  
  }

  Close(fileHandle);

  tinfl_return_value = tinfl_decompress_mem_to_mem(unzip_mem, output_size, temp_mem, input_size, 1);
  // printf("tinfl_decompress_mem_to_mem() returned %d\n", tinfl_return_value);
  FreeMem(temp_mem, input_size);

  return (unzip_mem);
}

PLANEPTR load_getmem(UBYTE *name, ULONG size)
{
  BPTR fileHandle;
  PLANEPTR mem;

  if (!(fileHandle = Open(name, MODE_OLDFILE)))
    return (NULL);

  if (!(mem = AllocMem(size, 0L)))
    return (NULL);

  Read(fileHandle, mem, size);
  Close(fileHandle);

  return (mem);
}

struct BitMap *load_file_as_bitmap(UBYTE *name, ULONG byte_size, UWORD width, UWORD height, UWORD depth)
{
  BPTR fileHandle;
  struct BitMap *new_bitmap;
  // PLANEPTR new_plane_ptr;
  UWORD i;

  if (!(fileHandle = Open(name, MODE_OLDFILE)))
    return (NULL);

  new_bitmap = (struct BitMap *)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
  InitBitMap(new_bitmap, depth, width, height);

  for (i = 0; i < depth; i++)
    (*new_bitmap).Planes[i] = (PLANEPTR)AllocMem(RASSIZE(width, height), MEMF_CHIP);

  for (i = 0; i < depth; i++)
    Read(fileHandle, (*new_bitmap).Planes[i], byte_size / depth);
  // Read(fileHandle, (*new_bitmap).Planes[0], byte_size);

  Close(fileHandle);

  return new_bitmap;
}

struct BitMap *load_zlib_file_as_bitmap(UBYTE *name, ULONG input_size, ULONG output_size, UWORD width, UWORD height, UWORD depth)
{
  BPTR fileHandle;
  struct BitMap *new_bitmap;
  UWORD i, j;
  PLANEPTR temp_mem, output_temp_mem;

  if (!(fileHandle = Open(name, MODE_OLDFILE)))
    return (NULL);

  new_bitmap = (struct BitMap *)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
  InitBitMap(new_bitmap, depth, width, height);

  for (i = 0; i < depth; i++)
    (*new_bitmap).Planes[i] = (PLANEPTR)AllocMem(RASSIZE(width, height), MEMF_CHIP);

  if (!(temp_mem = AllocMem(input_size, MEMF_PUBLIC)))
    return (NULL);

  Read(fileHandle, temp_mem, input_size);
  Close(fileHandle);

  tinfl_decompress_mem_to_mem((*new_bitmap).Planes[0], output_size, temp_mem, input_size, 1);
  FreeMem(temp_mem, input_size);
  temp_mem = NULL;

  /*
    Realign the bitplanes
    in case there is a padding needed.
    /!\ This should not work!
  */
  output_temp_mem = (*new_bitmap).Planes[0] + output_size;

  if (output_temp_mem != (*new_bitmap).Planes[depth - 1] + RASSIZE(width, height))
    for (i = 0; i < depth; i++)
      for(j = 0; j < RASSIZE(width, height); j++)
        *((*new_bitmap).Planes[depth - i - 1] + RASSIZE(width, height) - j) = *(output_temp_mem--);

  // printf("output_size = %d, RASSIZE() = %d, tinfl = %d\n", output_size, RASSIZE(width, height) * depth, ret_val);

  return new_bitmap;
}

struct BitMap *load_array_as_bitmap(UWORD *bitmap_array, ULONG array_size, UWORD width, UWORD height, UWORD depth)
{
  struct BitMap *new_bitmap;
  // PLANEPTR new_plane_ptr;
  UWORD i;
  UBYTE *read_ptr;

  new_bitmap = (struct BitMap *)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
  InitBitMap(new_bitmap, depth, width, height);

  for (i = 0; i < depth; i++)
    (*new_bitmap).Planes[i] = (PLANEPTR)AllocMem(RASSIZE(width, height), MEMF_CHIP);

  for (i = 0, read_ptr = (UBYTE *)bitmap_array; i < depth; i++, read_ptr += (array_size / depth))
    memcpy((UBYTE *)(*new_bitmap).Planes[i], read_ptr, array_size / depth);

  return new_bitmap;
}

void free_allocated_bitmap(struct BitMap *allocated_bitmap)
{
  USHORT i;
  ULONG block_len;

  if (allocated_bitmap)
  {
    // printf("free_allocated_bitmap() allocated_bitmap = %x\n", allocated_bitmap);
    // printf("allocated_bitmap, BytesPerRow = %d, Rows = %d, Depth = %d, pad = %d\n",
    //       (*allocated_bitmap).BytesPerRow,
    //       (*allocated_bitmap).Rows,
    //       (*allocated_bitmap).Depth,
    //       (int)(*allocated_bitmap).pad);

    block_len = RASSIZE((*allocated_bitmap).BytesPerRow * 8, (*allocated_bitmap).Rows);
    for (i = 0; i < (*allocated_bitmap).Depth; i++)
    {
      // printf("FreeMem() plane[%i], block_len = %i\n", i, block_len);
      FreeMem((*allocated_bitmap).Planes[i], block_len); // (*allocated_bitmap).BytesPerRow * (*allocated_bitmap).Rows);
    }

    block_len = (LONG)sizeof(struct BitMap);
    // printf("FreeMem() struct BitMap, block_len = %i\n", block_len);
    FreeMem(allocated_bitmap, block_len);
  }
}

void load_file_into_existing_bitmap(struct BitMap *new_bitmap, BYTE *name, ULONG byte_size, UWORD depth)
{
  BPTR fileHandle;
  UWORD i;

  if (fileHandle = Open(name, MODE_OLDFILE))
  {
    for (i = 0; i < depth; i++)
      Read(fileHandle, (*new_bitmap).Planes[i], byte_size / depth);
    Close(fileHandle);
  }
}

void disp_interleaved_st_format(PLANEPTR data, struct BitMap *dest_BitMap, UWORD width, UWORD height, UWORD src_y, UWORD x, UWORD y, UWORD depth)
{
  PLANEPTR src, dest;
  UWORD i, j, k;
  UWORD x_byte, width_byte;

  while(width != ((width >> 4) << 4))
    width++;

  x_byte = x >> 3;
  width_byte = width >> 3;

  for (i = 0; i < height; i ++)
  {
    for (k = 0; k < depth; k ++)
    {
      for (j = 0; j < width_byte; j ++)
      {
        src = data + (j + (i + src_y) * 40 * depth) + (k * 40);
        dest = (*dest_BitMap).Planes[k] + j + x_byte + (48 * i) + 48 * y;

        *dest = *src;
      }
    }
  }
}