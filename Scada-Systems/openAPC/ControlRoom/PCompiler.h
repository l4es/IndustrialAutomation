#ifndef PCOMPILER_H
#define PCOMPILER_H

class ObjectList;
class FlowConnection;
class flowObject;

class PCompiler
{
public:
   PCompiler();
   ~PCompiler();

   void compile(ObjectList *olist);

protected:

private:
   FlowConnection *findValidTarget(FlowConnection *startConnection,flowObject *nextObject,FlowConnection *resetConnection);
   void            shortenConnections(flowObject *srcObject,FlowConnection *srcConnection,FlowConnection *parentConnection);
   void            removeIOElementDefinitions(ObjectList *olist);
   void            removeOrphanFlowConnections(ObjectList *olist);
   void            resolveGroupStructure(ObjectList *olist);
   void            recurseStartingObjects(ObjectList *olist);

   ObjectList     *m_objectList;
};

#endif
