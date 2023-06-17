package simpledb.buffer;

import java.util.*;

public class DoublyLinkedList{
	
	public Node head;
	public Node tail;
	public int size;

	public DoublyLinkedList(){
		size = 0;
	}

	public class Node{
		public Node nextNode;
		public Node prevNode;
		public Buffer data;

		Node(Buffer data){
			this.data = data;
			this.nextNode = null;
			this.prevNode = null;
		}
	}

	public void addFirst(Buffer data){
		Node newNode = new Node(data);

		if(head != null){
			newNode.nextNode = head;
			head.prevNode = newNode;
		}

		head = newNode;
		if(head.nextNode == null){
			tail = head;
		}

		size++;
	}

	public void addLast(Buffer data){
		if(size == 0){
			addFirst(data);
		}
		else{
			Node newNode = new Node(data);
			tail.nextNode = newNode;
			newNode.prevNode = tail;
        		tail = newNode;
        		size++;
		}
	}

	public void add(int index, Buffer data){
		if(index == 0){
			addFirst(data);
		}
		else if(index == size - 1){
			addLast(data);
		}
		else{
			Node newNode = new Node(data);
			Node prevNode = getNode(index - 1);
			Node nextNode = prevNode.nextNode;

			newNode.prevNode = prevNode;
			newNode.nextNode = nextNode;

			prevNode.nextNode = newNode;

			nextNode.prevNode = newNode;

			size++;
		}
	}
	
	public Buffer removeFirst(){
		if(size == 0){
			return null;
		}

		Node removeNode = head;
		head = null;
		head = removeNode.nextNode;
		head.prevNode = null;
		size--;

		return removeNode.data;
	}

	public Buffer removeLast(){
		if(size == 0){
			return null;
		}

		Node removeNode = tail;
		tail = null;
		tail = removeNode.prevNode;
		tail.nextNode = null;
		size--;

		return removeNode.data;
	}

	public Buffer remove(int index){
		if(size == 0){
			return null;
		}

		if(index == 0){
			return removeFirst();
		}
		else if(index == size - 1){
			return removeLast();
		}
		else{
			Node removeNode = getNode(index);
			Node prevNode = removeNode.prevNode;
			Node nextNode = removeNode.nextNode;

			prevNode.nextNode = nextNode;

			nextNode.prevNode = prevNode;

			size--;

			return removeNode.data;
		}
	}

	public Node getNode(int index){
		if(index < size / 2){
			Node node = head;
			for(int i = 0; i < index; i++){
				node = node.nextNode;
			}

			return node;
		}
		else{
			Node node = tail;
			for(int i = size - 1; i > index; i--){
				node = node.prevNode;
			}
			return node;
		}
	}

	public String toString(){
		StringBuffer result = new StringBuffer("[");
		if(size != 0){
			Node node = head;
			result.append(node.data);
			while(node.nextNode != null){
				node = node.nextNode;
				result.append(",");
				result.append(node.data);
			}
		}
		result.append("]");
		return result.toString();
	}

	public void clear(){
		head = null;
		tail = null;
		size = 0;
	}

}
