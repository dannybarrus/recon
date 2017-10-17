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
public class SinglyLinkedList
{

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        LinkedList thisList = new LinkedList();
        
        thisList.add("0");
        thisList.add("1");
        thisList.add("2");
        thisList.add("3");
        thisList.add("4");
        thisList.add("5");
        
        System.out.println("Print: crunchifyList: \t\t" + thisList);
        System.out.println(".size(): \t\t\t\t" + thisList.size());
        System.out.println(".get(3): \t\t\t\t" + thisList.get(3) + " (get element at index:3 - list starts from 0)");
        System.out.println(".remove(2): \t\t\t\t" + thisList.remove(2) + " (element removed)");
        System.out.println(".get(3): \t\t\t\t" + thisList.get(3) + " (get element at index:3 - list starts from 0)");
        System.out.println(".size(): \t\t\t\t" + thisList.size());
        System.out.println("Print again: crunchifyList: \t" + thisList);
    }
    
}
