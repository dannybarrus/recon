/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package singlylinkedlist;

/**
 *
 * @author danny
 */
public class LinkedList
{
    private static int counter;
    private Node head;
    
    public LinkedList()
    {
    }

       
    private static int getCounter()
    {
        return counter;
    }
    
    private static void incrementCounter()
    {
        counter++;
    }
    
    private void decrementCounter()
    {
        counter--;
    }
    
    public int size()
    {
        return getCounter();
    }
    
    @Override
    public String toString()
    {
        String output = "";
        
        if(head != null)
        {
            Node current = head.getNext();
            while(current != null)
            {
                output += "[" + current.getData().toString() + "]";
                current = current.getNext();
            }
        }
        
        return output;
    }
    
    // appends the specified element to the end of the list
    public void add(Object data)
    {
        // init node only in case of 1st element. 
        if(head == null)
        {
            head = new Node(data);
        }
        
        Node temp = new Node(data);
        Node current = head;
        
        // check for NPE before iterating over current
        if(current != null)
        {
            while(current.getNext() != null)
            {
                current = current.getNext();
            }
            
            current.setNext(temp);
    
        }
        
        incrementCounter();
    }
    
    @SuppressWarnings("null")
    public void add(Object data, int index)
    {
        Node temp = new Node(data);
        Node current = head;
        
        // check for NPE before iterating over current
        if(current != null)
        {
            for(int i = 0; i < index && current.getNext() != null; i++)
            {
                current = current.getNext();
            }
        }
        
        //set new node's next-node reference to this node's next-node
        temp.setNext(current.getNext());
        
        // now set this node's next-node to the new node
        current.setNext(temp);
        
        // increment counter to track new node
        incrementCounter();
        
    }
    
    public Object get(int index)
    {
        if(index < 0)
        {
            return null;
        }
        
        Node current = null;
        if(head != null)
        {
            current = head.getNext();
            for(int i = 0; i < index; i++)
            {
                if(current.getNext() == null)
                {
                    return null;
                }
                current = current.getNext();
            }
            return current.getData();
        }
        return current;
    }
    
    public boolean remove(int index)
    {
        if(index < 1 || index > size())
        {
            return false;
        }
        
        Node current = head;
        if(head != null)
        {
            
            for(int i = 0; i < index; i++)
            {
                if(current.getNext() == null)
                {
                    return false;
                }
                current = current.getNext();
            }
            
            current.setNext(current.getNext().getNext());
            
            decrementCounter();
            return true;
        }
        
        return false;
    }
}
