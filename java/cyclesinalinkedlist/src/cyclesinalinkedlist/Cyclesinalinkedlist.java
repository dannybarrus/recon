/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package cyclesinalinkedlist;

/**
 *
 * @author danny
 */



public class Cyclesinalinkedlist
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


        
        System.out.println("Print list: \t\t" + thisList);
        
        boolean isCyclic = thisList.hasCycle(); 
        
        System.out.println("Has cycle:  " + isCyclic + "     (Expected:  false)");
        
        thisList.createCylclicLink(5, 3);
        
        System.out.println("Has cycle:  " + isCyclic + "     (Expected:  true)");
        
    }
    
    
    
}
