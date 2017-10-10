
        
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package anagrams;

import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 *
 * @author danny
 */
public class Anagrams
{
    public static int NUMBER_LETTERS = 26;
    public static int SHOW_DETAIL = 1;
    
    public static int getDelta(int[] array1, int[] array2)
    {
        if(array1.length != array2.length)
        {
            // dbtodo -- more error handling
            if(SHOW_DETAIL == 1)
            {
               System.out.println("Arrays do not match!!!");
            }
            return -1;
        }
        int delta = 0;
        for(int i=0;i < array1.length; i++)
        {
            int difference = Math.abs(array1[i] - array2[i]);
            if(SHOW_DETAIL == 1)
            {
                int offset = (int) 'a';
                char alphaLetter = (char) (offset + i);
                System.out.println("Letter (" + alphaLetter + ") -- The difference between " + array1[i] + " and " + array2[i] + " is " + difference);
                        
            }
            delta += difference;
        }
        
        return delta;
    
    }
    
    public static int [] getCharCounts(String s)
    {
        int[] charCounts = new int[NUMBER_LETTERS];

        for(int i=0; i < s.length(); i++)
        {
            int offset = (int) 'a';
            char c = s.charAt(i);
            int code = c - offset;
            charCounts[code]++;
        }
        if(SHOW_DETAIL == 1)
        {
           System.out.println("String is:  " + s);
           for(int i : charCounts)
           {
               System.out.print(i);
           }
           System.out.println();
        }
        return charCounts;
    }

    public static int numberNeeded(String first, String second)
    {
        int[] charCount1 = getCharCounts(first);
        int[] charCount2 = getCharCounts(second);

        return getDelta(charCount1, charCount2);
    }

    /**
     * Given two string (lowercase a->z), how many characters do we need to remove (from either)
     * to make them anagrams?
     * Example:  hello, billion --> Answer: 6 (he from hello and biin from billion)
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        
        String first = "";
        String second = "";
        
        try
        {
            first = br.readLine();
            second = br.readLine();
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
        }
                
        System.out.println(numberNeeded(first, second));
        
    }
    
}
