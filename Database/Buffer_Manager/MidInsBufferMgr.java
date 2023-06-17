package simpledb.buffer;

import java.util.*;

import simpledb.file.*;
import simpledb.log.LogMgr;

public class MidInsBufferMgr implements BufferMgr {

	private Map<BlockId, Buffer> bufferpool;
	private Deque<Buffer> freeBufferList;
	private DoublyLinkedList LRU_list;
	private int numAvailable;
	private static final long MAX_TIME = 10000;
	private int reference_cnt = 0;
	private int buffer_hit_cnt = 0;
	
   /**
    * Constructor:  Creates a buffer manager having the specified 
    * number of buffer slots.
    * This constructor depends on a {@link FileMgr} and
    * {@link simpledb.log.LogMgr LogMgr} object.
    * @param numbuffs the number of buffer slots to allocate
    */
   public MidInsBufferMgr(FileMgr fm, LogMgr lm, int numbuffs) {
        /*Write your code */
	   bufferpool = new LinkedHashMap<>();
	   freeBufferList = new LinkedList<>();
	   LRU_list = new DoublyLinkedList();
	   this.numAvailable = numbuffs;
	   for(int i = 0; i < numbuffs; i++){
		Buffer buff = new Buffer(fm, lm, i);
		freeBufferList.add(buff);
	   }
   }

   public synchronized int available() {
      /* Write your code*/
	   int cnt = freeBufferList.size();
	   DoublyLinkedList.Node node = LRU_list.head;
	   while(node != null){
		if(!node.data.isPinned())
			cnt++;
		node = node.nextNode;
	   }

        return cnt;
   }

   public synchronized void flushAll(int txnum){
      /* Write your code*/
	   DoublyLinkedList.Node node = LRU_list.head;

	   while(node != null){
		if(node.data.modifyingTx() == txnum){
			if(node.data.isPinned())
				node.data.unpin();
			node.data.flush();
			freeBufferList.add(node.data);
		}
		node = node.nextNode;
	   }

	   LRU_list.clear();
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

   private boolean waitingTooLong(long starttime){
	return System.currentTimeMillis() - starttime > MAX_TIME;
   }

   public synchronized void unpin(Buffer buff) {
      /* Write your code */
	   buff.unpin();
	   if(!buff.isPinned()){   
		notifyAll();
	   }
   }

   private Buffer tryToPin(BlockId blk){

	   Buffer buff = bufferpool.get(blk);
	   this.reference_cnt++;

	   if(buff == null){
		   // miss -> get from freeBufferList
		   buff = chooseFreeBuffer();
		   if(buff == null){
			// if null, get from tail of the LRU_list
			buff = LRU_list.removeLast();
            buff.unpin();
			buff.flush();

            for(Map.Entry<BlockId, Buffer> entry : bufferpool.entrySet()){
                    if(entry.getValue().getID() == buff.getID()){
                            bufferpool.remove(entry.getKey());
                    }
            }
            bufferpool.put(blk, buff);
            buff.assignToBlock(blk);
		   }
		   else{
			// if in the freeBufferList   
		   	bufferpool.put(blk, buff);
			buff.assignToBlock(blk);
		   }
		   
		   if(LRU_list.size <= 3){
			   LRU_list.addFirst(buff);
		   }
		   else{
			int index = (int)(LRU_list.size * 5 / 8);
			LRU_list.add(index, buff);
		   }
	   }
	   else{
		   // hit
		   int index2 = -1;
		   for(int i = 0; i < LRU_list.size; i++){
			Buffer temp = LRU_list.getNode(i).data;
			if(temp.getID() == buff.getID()){
				index2 = i;
				break;
			}
		   }

		   LRU_list.remove(index2);
		   LRU_list.addFirst(buff);
		   this.buffer_hit_cnt++;
	   }

	   buff.pin();

	   return buff;
   }

   private Buffer chooseFreeBuffer(){
	if(!freeBufferList.isEmpty()){
		Buffer buff = freeBufferList.remove();
		if(buff != null)
			return buff;
	}
	return null;
   }

   public void printStatus(){
      /* Write your code */

	   System.out.println("LRU list:");
	   DoublyLinkedList.Node node = LRU_list.head;
	   while(node != null){
		StringBuilder result = new StringBuilder();
		result.append("Buffer ").append(node.data.getID()).append(": ");
		result.append(node.data.block().toString());
		if(node.data.isPinned()){
			result.append(" pinned");
		}
		else{
			result.append(" unpinned");
		}
		System.out.println(result.toString());
		node = node.nextNode;
	   }
   } 

   public float getHitRatio(){
        /* Write your code */
	   if(reference_cnt == 0)
		   return 0.00f;
	   System.out.println("reference_cnt : " + reference_cnt + " buffer_hit_cnt : " + buffer_hit_cnt);
	   float result = (buffer_hit_cnt /(float) reference_cnt * 100);

     	return Math.round(result * 100) / 100.0f;
    }
   
}
