package simpledb.buffer;

import java.util.*;

import simpledb.file.*;
import simpledb.log.LogMgr;


public class LRUBufferMgr implements BufferMgr {
	
	private Map<BlockId, Buffer> bufferpool;
	private Deque<Buffer> unpinnedList;
	private Deque<Buffer> bufferList;
	private int numAvailable;
	private static final long MAX_TIME = 10000;
	private int buffer_hit_cnt = 0;
	private int reference_cnt = 0;
   /**
    * Constructor:  Creates a buffer manager having the specified 
    * number of buffer slots.
    * This constructor depends on a {@link FileMgr} and
    * {@link simpledb.log.LogMgr LogMgr} object.
    * @param numbuffs the number of buffer slots to allocate
    */
    public LRUBufferMgr(FileMgr fm, LogMgr lm, int numbuffs) {
        /*Write your code */
	    bufferpool = new LinkedHashMap<>();
	    unpinnedList = new LinkedList<>();
	    bufferList = new LinkedList<>();
	    this.numAvailable = numbuffs;
	    for(int i = 0; i < numbuffs; i++){
		Buffer buff = new Buffer(fm, lm, i);
		unpinnedList.add(buff);
	    }
   }

   public synchronized int available() {
      /* Write your code*/
        return unpinnedList.size();
   }

   public synchronized void flushAll(int txnum){
      /* Write your code*/
	   for(Buffer buff : bufferpool.values()){
		if(buff.modifyingTx() == txnum)
			buff.flush();
	   }
   }



   public synchronized Buffer pin(BlockId blk) {
      /* Write your code*/
	   try{
		long timestamp = System.currentTimeMillis();
		Buffer buff = tryToPin(blk);
		while(buff == null && !waitingTooLong(timestamp)){
			wait(MAX_TIME);
			buff = tryToPin(blk);
		}
		if(buff == null)
			throw new BufferAbortException();
		return buff;
	   }
	   catch(InterruptedException e){
		throw new BufferAbortException();
	   }
   }
	
   private boolean waitingTooLong(long starttime) {
	return System.currentTimeMillis() - starttime > MAX_TIME;
   } 

   public synchronized void unpin(Buffer buff) {
      /* Write your code */
	   buff.unpin();
	   /* if buffer's pin count becomes 0, add it to the end of the list*/
	   if(!buff.isPinned()){
		unpinnedList.addLast(buff);
		notifyAll();
	   }
   }


  private Buffer tryToPin(BlockId blk){

	Buffer buff = bufferpool.get(blk);	
	this.reference_cnt++;

	if(buff == null){
		buff = chooseUnpinnedBuffer();
		if(buff == null)
			return null;

		// If miss, pick up from unpinnedList and remove the old one and add new
		// first one is old one and last one is new one
		Buffer temp = null;
		if(bufferList.size() >= this.numAvailable){
			temp = bufferList.removeFirst();
			if(temp != null && temp != buff){
				this.unpin(temp);
				temp.flush();
			}
			bufferList.add(buff);
		}
		else{
			bufferList.add(buff);
		}
		
		if(!bufferpool.containsKey(blk)){
			// if new block allocated to exist buffer, remove and put again
			for(Map.Entry<BlockId, Buffer> entry : bufferpool.entrySet()){
				if(entry.getValue().getID() == buff.getID()){
					bufferpool.remove(entry.getKey());
				}
			}

			bufferpool.put(blk, buff);
			buff.assignToBlock(blk);
		}
		else{
			bufferpool.put(blk, buff);
			buff.assignToBlock(blk);
		}
		
	}
	else{	
		// If hit, move the buffer to the first
		bufferList.remove(buff);
		bufferList.add(buff);
		this.buffer_hit_cnt++;
		
	}
	buff.pin();
	

	return buff;
  }

  private Buffer chooseUnpinnedBuffer(){
	if(!unpinnedList.isEmpty()){
		Buffer buff = unpinnedList.remove();
		if(buff != null && !buff.isPinned())
			return buff;
	}
	return null;
  }

   public void printStatus(){
      /* Write your code */ 
	   String temp = "";
	   System.out.println("Allocated Buffers:");
	   for(Buffer buff : bufferpool.values()){
		StringBuilder result = new StringBuilder();
		result.append("Buffer ").append(buff.getID()).append(": ");
		result.append(buff.block().toString());
		if(buff.isPinned()){
			result.append(" pinned");
		}
		else{
			result.append(" unpinned");
			temp += buff.getID();
			temp += " ";
		}
		System.out.println(result.toString());
	   }
	   System.out.println("Unpinned Buffers in LRU order: " + temp);
   }

  /* private String getUnpinnedListString(){
	StringBuilder result = new StringBuilder();
	for(Buffer buff : unpinnedList){
		if(buff != null)
			result.append(buff.getID()).append(" ");
	}
	return result.toString().trim();
   }*/

   public float getHitRatio(){
        /* Write your code */
	   if(reference_cnt == 0)
		   return 0;
	   System.out.println("reference_cnt : " + reference_cnt + " buffer_hit_cnt : " + buffer_hit_cnt);
	   float result = (buffer_hit_cnt /(float) reference_cnt * 100);

     	return Math.round(result * 100) / 100.0f;
    }

}

