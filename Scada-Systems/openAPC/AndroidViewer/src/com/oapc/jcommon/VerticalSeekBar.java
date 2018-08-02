package com.oapc.jcommon;

import android.content.*;
import android.graphics.*;
import android.util.*;
import android.view.*;
import android.widget.*;


public class VerticalSeekBar extends SeekBar 
{
   private int progress=0;

    public VerticalSeekBar(Context context) 
    {
        super(context);
    }

    
    
    protected void onSizeChanged(int w, int h, int oldw, int oldh) 
    {
        super.onSizeChanged(h, w, oldh, oldw);
    }

    
    
    @Override
    protected synchronized void onMeasure(int widthMeasureSpec, int heightMeasureSpec) 
    {
        super.onMeasure(heightMeasureSpec, widthMeasureSpec);
        setMeasuredDimension(getMeasuredHeight(), getMeasuredWidth());
    }

    
    
    protected void onDraw(Canvas c) 
    {
        c.rotate(-90);
        c.translate(-getHeight(), 0);

        super.onDraw(c);
    }

    
    
    public void setProgress(int progress)
    {
       this.progress=progress;
       super.setProgress(progress);
    }
    
    
    public int getProgress()
    {

       return progress;
    }
    
    
    
    @Override
    public boolean onTouchEvent(MotionEvent event) 
    {
       super.onTouchEvent(event);
       if (!isEnabled()) 
       {
          return false;
       }

       switch (event.getAction()) 
       {
          case MotionEvent.ACTION_DOWN:
          case MotionEvent.ACTION_MOVE:
          case MotionEvent.ACTION_UP:
          {
             progress=getMax() - (int) (getMax() * event.getY() / getHeight());
             setProgress(progress);
             onSizeChanged(getWidth(), getHeight(), 0, 0);
             break;
          }

          case MotionEvent.ACTION_CANCEL:
             break;
       }
       return true;
   }
}

