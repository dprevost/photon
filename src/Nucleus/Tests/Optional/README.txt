The tests in this directory are marked optional.

Tests:

InitBigShMem

    The goal of this test is to see what occurs when we force the bitmap
    of the allocator to be equal to a whole block (forcing the memory
    allocator itself to use two blocks instead of one).
    
    Since each bit of the bitmap represents a block, to get a bitmap of
    length one block requires 8 * PSON_BLOCK_SIZE blocks. 
    
    The total memory is obtained by multiplying the number of blocks by the
    length of each block so: 8 * PSON_BLOCK_SIZE * PSON_BLOCK_SIZE.
    
    The utility DisplayBigShMemSize can do this calculation for you if you 
    are unsure of the actual size of blocks you are using.
    
    For a block size of 8192 bytes, the shared memory would have to be 512 MB. 
    
    This is why this test is optional - it will often go beyond the amount
    of available RAM.