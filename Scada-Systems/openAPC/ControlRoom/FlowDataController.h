#ifndef FLOWDATACONTROLLER_H
#define FLOWDATACONTROLLER_H

class FlowDataController
{
public:
   FlowDataController();
   virtual ~FlowDataController();

private:
   void *m_mutex;
};

#endif
