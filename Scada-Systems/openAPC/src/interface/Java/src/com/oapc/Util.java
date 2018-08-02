package com.oapc;

public class Util
{

   public static double numBlockToDbl(int numValue,short numDivider)
   {
      if ((numDivider==0) || (numDivider==1)) return (double)numValue;
      return numValue/Math.pow(10.0,numDivider);
   }
   
   
   public static void dblToNumBlock(double inValue,int[] numValue,short[] numDivider)
   {
      if ((Math.abs(inValue)<1000000000.0) && ((inValue-(int)inValue)!=0))
      {
         numDivider[0]=0;
         while ((Math.abs(inValue)<1000000000.0) && ((inValue-(int)inValue)!=0) && (numDivider[0]<20)) // slow but necessary to avoid problems with incorrect floating point calculations
         {
            numDivider[0]++;
            inValue*=10;
         }
         numValue[0]=(int)inValue;
      }
      else
      {
         numDivider[0]=0;
         numValue[0]=(int)inValue;
      }
   }


}