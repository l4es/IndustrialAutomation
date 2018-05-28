#include "machine.h"
#include <scicos_block4.h>


struct TempFuncDev{
	unsigned int var1
	double var3;
	int var2;
};

static int init(scicos_block *block)
{
  struct FlagTestDev * comdev = (struct FlagTestDev *) malloc(sizeof(struct FlagTestDev));

  comdev->var1=block->ipar[0];
  comdev->var2=block->ipar[1];
  comdev->var3=block->ipar[2];
  *block->work=(void *)comdev;
 
  return 0;
}
 


static int state(scicos_block *block)
{
  struct FlagTestDev * comdev = (struct FlagTestDev *) (*block->work);
     if (get_scicos_time()==comdev->time){
	comdev->step++;
    } else {
	comdev->time=get_scicos_time();
	comdev->step=1;
    }
     printf("Flag 0: update state, step=%d, t=%f\n",comdev->step,comdev->time);

  return 0;
}
static int in(scicos_block *block)
{
struct FlagTestDev * comdev = (struct FlagTestDev *) (*block->work);
     printf(" Flag 2 ");
}

static int inout(scicos_block *block)
{
  struct FlagTestDev * comdev = (struct FlagTestDev *) (*block->work);
  int i;    
     if (get_scicos_time()==comdev->time){
	comdev->step++;
    } else {
	comdev->time=get_scicos_time();
	comdev->step=1;
    }

     printf("Flag 1: update output, step=%d, t=%f\n",comdev->step,comdev->time);
if (GetNin(block)>0){
 double *u1 = GetRealInPortPtrs(block,1);
 comdev->data=u1[0];
}
if (GetNout(block)>0){
  double *y1 = GetRealOutPortPtrs(block,1);
  y1[0]=comdev->data;
}

  return 0;
}
static int eventout(scicos_block *block)
{
  struct FlagTestDev * comdev = (struct FlagTestDev *) (*block->work);
     if (get_scicos_time()==comdev->time){
	comdev->step++;
    } else {
	comdev->time=get_scicos_time();
	comdev->step=1;
    }
     printf("Flag 3: update event output, step=%d, t=%f\n",comdev->step,comdev->time);

  return 0;
}

static int end(scicos_block *block)
{
  struct FlagTestDev * comdev = (struct FlagTestDev *) (*block->work);

  free(comdev);
  return 0;
}

void rt_tempfunc(scicos_block *block,int flag)
{

  if (flag==0){    
    state(block);     /* update stats */
  }
  else if (flag==1){    
    inout(block);     /* update output */
  }
  else if (flag==2){    
    in(block);     /* ??? */
  }
  else if (flag==3){    
    eventout(block);     /* update event output */
  }
  else if (flag==5){     /* termination */ 
    end(block);
  }
  else if (flag==4){     /* initialisation */
    init(block);
  }
}

