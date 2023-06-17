package simpledb.buffer;

import simpledb.file.*;


public interface BufferMgr {

   /**
    * Returns the number of available (i.e. unpinned) buffers.
    * @return the number of available buffers
    */
   public int available();

   /*
    * Flushes the dirty buffers modified by the specified transaction.
     @param txnum the transaction's id number
    */
    public void flushAll(int txnum);

   /**
    * Pins a buffer to the specified block, potentially
    * waiting until a buffer becomes available.
    * If no buffer becomes available within a fixed 
    * time period, then a {@link BufferAbortException} is thrown.
    * @param blk a reference to a disk block
    * @return the buffer pinned to that block
    */
   public Buffer pin(BlockId blk);

   /**
    * Unpins the specified data buffer. If its pin count
    * goes to zero, then notify any waiting threads.
    * @param buff the buffer to be unpinned
    */
   public void unpin(Buffer buff);

   /*
    * Displays current status.
    * The status consists of ID, block, and pinned status of each buffer in the allocated map.
    * Also, IDs of each buffer in the unpinned list must be shown.
    */
   public void printStatus();

   /*
    * Returns the hit ratio of buffer manager
    * @return hit ratio of buffer manager (ex. 98.77%)
    */
    public float getHitRatio(); 
   
}
