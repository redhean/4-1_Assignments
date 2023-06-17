package simpledb.buffer;

import simpledb.file.*;
import simpledb.log.LogMgr;


public class VanillaBufferMgr implements BufferMgr {
   private Buffer[] bufferpool; /* buffer pool */ 
   private int numAvailable;   /* the number of available (unpinned) buffer slots */
   private static final long MAX_TIME = 10000; /* 10 seconds */
   private int buffer_hit_cnt = 0;
   private int reference_cnt = 0;

   /**
    * Constructor:  Creates a buffer manager having the specified 
    * number of buffer slots.
    * This constructor depends on a {@link FileMgr} and
    * {@link simpledb.log.LogMgr LogMgr} object.
    * @param numbuffs the number of buffer slots to allocate
    */
   public VanillaBufferMgr(FileMgr fm, LogMgr lm, int numbuffs) {
      bufferpool = new Buffer[numbuffs];
      numAvailable = numbuffs;
      for (int i=0; i<numbuffs; i++)
         bufferpool[i] = new Buffer(fm, lm, i);
   }
   
   /**
    * Returns the number of available (i.e. unpinned) buffers.
    * @return the number of available buffers
    */
   public synchronized int available() {
      return numAvailable;
   }
   
   /**
    * Flushes the dirty buffers modified by the specified transaction.
    * @param txnum the transaction's id number
    */
   public synchronized void flushAll(int txnum) {
      for (Buffer buff : bufferpool)
         if (buff.modifyingTx() == txnum)
         	buff.flush();
   }
   
   /**
    * Unpins the specified data buffer. If its pin count
    * goes to zero, then notify any waiting threads.
    * @param buff the buffer to be unpinned
    */
   public synchronized void unpin(Buffer buff) {
      buff.unpin();
      if (!buff.isPinned()) {
         numAvailable++;
         notifyAll();
      }
   }
   
   /**
    * Pins a buffer to the specified block, potentially
    * waiting until a buffer becomes available.
    * If no buffer becomes available within a fixed 
    * time period, then a {@link BufferAbortException} is thrown.
    * @param blk a reference to a disk block
    * @return the buffer pinned to that block
    */
   public synchronized Buffer pin(BlockId blk) {
      try {
         long timestamp = System.currentTimeMillis();
         Buffer buff = tryToPin(blk);
         while (buff == null && !waitingTooLong(timestamp)) {
            wait(MAX_TIME);
            buff = tryToPin(blk);
         }
         if (buff == null)
            throw new BufferAbortException();

         return buff;
      }
      catch(InterruptedException e) {
         throw new BufferAbortException();
      }
   }
   
   /**
    * Returns true if starttime is older than 10 seconds
    * @param starttime timestamp 
    * @return true if waited for more than 10 seconds
    */
   private boolean waitingTooLong(long starttime) {
      return System.currentTimeMillis() - starttime > MAX_TIME;
   }
   
   /**
    * Tries to pin a buffer to the specified block. 
    * If there is already a buffer assigned to that block
    * then that buffer is used;
    * otherwise, an unpinned buffer from the pool is chosen.
    * Returns a null value if there are no available buffers.
    * @param blk a reference to a disk block
    * @return the pinned buffer
    */
   private Buffer tryToPin(BlockId blk) {
      Buffer buff = findExistingBuffer(blk);
      this.reference_cnt++;
      if (buff == null) {
         buff = chooseUnpinnedBuffer();
         if (buff == null)
            return null;
         buff.assignToBlock(blk);
      }
      else
	      buffer_hit_cnt++;

      if (!buff.isPinned())
         numAvailable--;
      buff.pin();
      return buff;
   }
   
   /**
    * Find and return a buffer assigned to the specified block. 
    * @param blk a reference to a disk block
    * @return the found buffer       
    */
   private Buffer findExistingBuffer(BlockId blk) {
      for (Buffer buff : bufferpool) {
         BlockId b = buff.block();
         if (b != null && b.equals(blk))
            return buff;
      }
      return null;
   }
   
   /**
    * Find and return an unpinned buffer     . 
    * @return the unpinned buffer       
    */
   private Buffer chooseUnpinnedBuffer() {
      for (Buffer buff : bufferpool)
         if (!buff.isPinned())
         return buff;
      return null;
   }

   /*print current status of buffers*/
   public void printStatus(){
    /* Write your code */
	 System.out.println("Allocated Buffers:");
           for(Buffer buff : bufferpool){
                StringBuilder result = new StringBuilder();
                result.append("Buffer ").append(buff.getID()).append(": ");
                result.append(buff.block().toString());
                if(buff.isPinned()){
                        result.append(" pinned");
                }
                else{
                        result.append(" unpinned");
                }
                System.out.println(result.toString());
           }
   }

    /*Return the hit ratio of buffer manager 
     * @return percentage of hit ratio (). Round to two decimal places ex) 98.75
    */
   public float getHitRatio() {
    /*Write your code */
	if(reference_cnt == 0)
		return 0.0f;
	
	System.out.println("reference_cnt : " + reference_cnt + " buffer_hit_cnt : " + buffer_hit_cnt);
	float result = ((this.buffer_hit_cnt /(float) this.reference_cnt) * 100);
   	return Math.round(result * 100) / 100.0f;
   }
}
