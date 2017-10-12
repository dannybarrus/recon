/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package lonelyinteger;

/**
 *
 * @author danny
 */
public class LonelyInteger
{

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        int[] array = {9,1,2,3,2,9,1,7,7};
        int found = lonelyInteger(array);
        
        System.out.println("Found unique element:  " + found);
        
        System.out.println();
        System.out.println();
        printExample();
    }
    
    /**
     * Given an array of n integers, where all but one of the integers occur in pairs. 
     * In other words, every element in the array occurs exactly twice except for one.
     * Find and print the unique element. 
     * @param array the array to search
     * @return result the unique element
     */ 
    public static int lonelyInteger(int[] array)
    {
        int result = 0;
        
        for (int val : array)
        {
            // because there is only 1, xor will produce the unique result. 
            result ^= val;
        }
        
        return result;
    }
    
    public static void printExample()
    {
        System.out.println("  9   1001  ");
        System.out.println("  1   0001  ");
        System.out.println("  1   0001  ");
        System.out.println("  3   0011  ");
        System.out.println("  2   0010  ");
        System.out.println("  9   1001  ");
        System.out.println("  1   0001  ");
        System.out.println("  7   0111  ");
        System.out.println("  7   0111  ");
        System.out.println("  7   0111  ");
        System.out.println("------------  xor the bits above");
        System.out.println("  3   0011  ");
        
    }
    
}
