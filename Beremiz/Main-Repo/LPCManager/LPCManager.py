#!/usr/bin/env python
# -*- coding: utf-8 -*-
import shutil
import socket

__version__ = "$Revision$"

import os, sys, getopt, wx, tempfile
import __builtin__
from types import TupleType, StringType, UnicodeType

_lpcmanager_path = os.path.split(__file__)[0]
_dist_folder = os.path.split(sys.path[0])[0]
_beremiz_folder = os.path.join(_dist_folder, "beremiz")
sys.path.append(_beremiz_folder)

def Bpath(*args):
    return os.path.join(CWD,*args)

if __name__ == '__main__':
    def usage():
        print "\nUsage of LPCManager.py :"
        print "\n   %s Projectpath Buildpath port [arch]\n"%sys.argv[0]
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
    except getopt.GetoptError:
        # print help information and exit:
        usage()
        sys.exit(2)
    
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
    
    if len(args) < 3 or len(args) > 4:
        usage()
        sys.exit()
    else:
        projectOpen = args[0]
        buildpath = args[1]
        try:
            port = int(args[2])
        except:
            usage()
            sys.exit()
        if len(args) > 3:
            arch = args[3]
        else:
            arch = "MC8"

    __builtin__.__dict__["BMZ_DBG"] = os.path.exists("LPC_DEBUG")

    app = wx.PySimpleApp(redirect=BMZ_DBG)
    app.SetAppName('beremiz')
    wx.InitAllImageHandlers()

    from util.misc import InstallLocalRessources
    InstallLocalRessources(_beremiz_folder)

_lpcmanager_path = os.path.split(__file__)[0]
import features
from POULibrary import POULibrary

class PLCLibrary(POULibrary):
    def GetLibraryPath(self):
        return os.path.join(_lpcmanager_path, "pous.xml")

features.libraries=[
    ('Native', 'NativeLib.NativeLibrary'),
    ('LPC', lambda: PLCLibrary)]


import connectors
from LPCconnector import LPC_connector_factory
connectors.connectors["LPC"]=lambda:LPC_connector_factory

import targets
from LPCtarget import LPC_target 
targets.targets["LPC"] = {"xsd": os.path.join(_lpcmanager_path, "LPCtarget", "XSD"),
                          "class": lambda: LPC_target,
                          "code": os.path.join(_lpcmanager_path,"LPCtarget","plc_LPC_main.c")} 
targets.toolchains["makefile"] = os.path.join(_lpcmanager_path, "LPCtarget", "XSD_toolchain_makefile")

from Beremiz import *
from ProjectController import ProjectController
from ConfigTreeNode import ConfigTreeNode
from editors.ProjectNodeEditor import ProjectNodeEditor

from plcopen.structures import LOCATIONDATATYPES
from PLCControler import PLCControler, LOCATION_CONFNODE, LOCATION_MODULE, LOCATION_GROUP,\
                         LOCATION_VAR_INPUT, LOCATION_VAR_OUTPUT, LOCATION_VAR_MEMORY
from IDEFrame import IDEFrame
from dialogs import ProjectDialog
from controls import TextCtrlAutoComplete

havecanfestival = False
try:
    from canfestival import RootClass as CanOpenRootClass
    from canfestival.canfestival import _SlaveCTN, _NodeListCTN, NodeManager
    from canfestival.NetworkEditor import NetworkEditor
    from canfestival.SlaveEditor import SlaveEditor
    havecanfestival = True
except:
    havecanfestival = False
    
SCROLLBAR_UNIT = 10
WINDOW_COLOUR = wx.Colour(240,240,240)
TITLE_COLOUR = wx.Colour(200,200,220)
CHANGED_TITLE_COLOUR = wx.Colour(220,200,220)
CHANGED_WINDOW_COLOUR = wx.Colour(255,240,240)

if wx.Platform == '__WXMSW__':
    faces = { 'times': 'Times New Roman',
              'mono' : 'Courier New',
              'helv' : 'Arial',
              'other': 'Comic Sans MS',
              'size' : 16,
             }
else:
    faces = { 'times': 'Times',
              'mono' : 'Courier',
              'helv' : 'Helvetica',
              'other': 'new century schoolbook',
              'size' : 18,
             }

from editors.ConfTreeNodeEditor import GenBitmapTextButton

#-------------------------------------------------------------------------------
#                          CANFESTIVAL CONFNODE HACK
#-------------------------------------------------------------------------------
# from canfestival import canfestival
# class LPC_canfestival_config:
#     def getCFLAGS(self, *args):
#         return ""
# 
#     def getLDFLAGS(self, *args):
#         return ""
#         
# canfestival.local_canfestival_config = LPC_canfestival_config() 
#-------------------------------------------------------------------------------
#                              LPCModule Class
#-------------------------------------------------------------------------------

LOCATION_TYPES = {"I": LOCATION_VAR_INPUT,
                  "Q": LOCATION_VAR_OUTPUT,
                  "M": LOCATION_VAR_MEMORY}

LOCATION_DIRS = dict([(dir, size) for size, dir in LOCATION_TYPES.iteritems()])

LOCATION_SIZES = {}
for size, types in LOCATIONDATATYPES.iteritems():
    for type in types:
        LOCATION_SIZES[type] = size

def _GetModuleChildren(module):
    children = []
    for child in module["children"]:
        if child["type"] == LOCATION_GROUP:
            children.extend(child["children"])
        else:
            children.append(child)
    return children

def _GetVariables(module):
    variables = []
    for child in module["children"]:
        if child["type"] in [LOCATION_GROUP, LOCATION_MODULE]:
            variables.extend(_GetVariables(child))
        else:
            variables.append(child)
    return variables

def _GetLastModuleGroup(module):
    group = module
    for child in module["children"]:
        if child["type"] == LOCATION_GROUP:
            group = child
    return group["children"]

def _GetModuleBySomething(module, something, toks):
    for child in _GetModuleChildren(module):
        if child.get(something) == toks[0]:
            if len(toks) > 1:
                return _GetModuleBySomething(child, something, toks[1:])
            return child
    return None

def _GetModuleVariable(module, location, direction):
    for child in _GetModuleChildren(module):
        if child["location"] == location and child["type"] == LOCATION_TYPES[direction]:
            return child
    return None

def _RemoveModuleChild(module, child):
    if child in module["children"]:
        module["children"].remove(child)
    else:
        for group in module["children"]:
            if group["type"] == LOCATION_GROUP and child in group["children"]:
                group["children"].remove(child)

BUS_TEXT = """/* Code generated by LPCBus confnode */

/* LPCBus confnode includes */
#include "app_glue.h"
#ifdef _WINDOWS_H
  #include "iec_types.h"
#else
  #include "iec_std_lib.h"
#endif

%(declare_code)s

/* LPCBus confnode user variables definition */
%(var_decl)s

/* LPCBus confnode functions */
int __init_%(location_str)s(int argc,char **argv)
{
%(init_code)s
  return 0;
}

void __cleanup_%(location_str)s(void)
{
}

void __retrieve_%(location_str)s(void)
{
%(retrieve_code)s
}
        
void __publish_%(location_str)s(void)
{
%(publish_code)s
}
"""

class LPCBus(object):
    
    def __init__(self):
        self.VariableLocationTree = []
        self.ResetUsedLocations()
        self.Icon = None
    
    def __getitem__(self, key):
        if key == "children":
            return self.VariableLocationTree
        raise KeyError, "Only 'children' key is available"
    
    def CTNEnabled(self):
        return None
    
    def SetIcon(self, icon):
        self.Icon = icon
    
    def _GetChildBySomething(self, something, toks):
        return _GetModuleBySomething({"children" : self.VariableLocationTree}, something, toks)
    
    def GetBaseTypes(self):
        return self.GetCTRoot().GetBaseTypes()

    def GetSizeOfType(self, type):
        return LOCATION_SIZES[self.GetCTRoot().GetBaseType(type)]
    
    def _GetVariableLocationTree(self, current_location, infos):
        if infos["type"] == LOCATION_MODULE:
            location = current_location + (infos["IEC_Channel"],)
            return {"name": infos["name"],
                    "type": infos["type"],
                    "location": ".".join(map(str, location + ("x",))), 
                    "icon": infos["icon"], 
                    "children": [self._GetVariableLocationTree(location, child) for child in infos["children"]]}
        elif infos["type"] == LOCATION_GROUP:
            return {"name": infos["name"],
                    "type": infos["type"],
                    "location": "", 
                    "icon": infos["icon"], 
                    "children": [self._GetVariableLocationTree(current_location, child) for child in infos["children"]]}
        else:
            size = self.GetSizeOfType(infos["IEC_type"])
            location = "%" + LOCATION_DIRS[infos["type"]] + size + ".".join(map(str, current_location + infos["location"]))
            return {"name": infos["name"],
                    "type": infos["type"],
                    "size": size,
                    "IEC_type": infos["IEC_type"],
                    "var_name": infos["name"],
                    "location": location,
                    "description": infos["description"],
                    "children": []}
    
    def GetVariableLocationTree(self):
        return {"name": self.BaseParams.getName(),
                "type": LOCATION_CONFNODE,
                "location": self.GetFullIEC_Channel(),
                "icon": self.Icon, 
                "children": [self._GetVariableLocationTree(self.GetCurrentLocation(), child) 
                             for child in self.VariableLocationTree]}
    
    def CTNTestModified(self):
        return False

    def CTNMakeDir(self):
        pass

    def CTNRequestSave(self, from_project_path=None):
        return None

    def ResetUsedLocations(self):
        self.UsedLocations = {}
    
    def _AddUsedLocation(self, parent, location):
        num = location.pop(0)
        if not parent.has_key(num):
            parent[num] = {"used": False, "children": {}}
        if len(location) > 0:
            self._AddUsedLocation(parent[num]["children"], location)
        else:
            parent[num]["used"] = True
        
    def AddUsedLocation(self, location):
        if len(location) > 0:
            self._AddUsedLocation(self.UsedLocations, list(location))

    def _CheckLocationConflicts(self, parent, location):
        num = location.pop(0)
        if not parent.has_key(num):
            return False
        if len(location) > 0:
            if parent[num]["used"]:
                return True
            return self._CheckLocationConflicts(parent[num]["children"], location)
        elif len(parent[num]["children"]) > 0:
            return True
        return False

    def CheckLocationConflicts(self, location):
        if len(location) > 0:
            return self._CheckLocationConflicts(self.UsedLocations, list(location))
        return False

    def CTNGenerate_C(self, buildpath, locations):
        """
        Generate C code
        @param current_location: Tupple containing confnode IEC location : %I0.0.4.5 => (0,0,4,5)
        @param locations: List of complete variables locations \
            [{"IEC_TYPE" : the IEC type (i.e. "INT", "STRING", ...)
            "NAME" : name of the variable (generally "__IW0_1_2" style)
            "DIR" : direction "Q","I" or "M"
            "SIZE" : size "X", "B", "W", "D", "L"
            "LOC" : tuple of interger for IEC location (0,1,2,...)
            }, ...]
        @return: [(C_file_name, CFLAGS),...] , LDFLAGS_TO_APPEND
        """
        current_location = self.GetCurrentLocation()
        # define a unique name for the generated C file
        location_str = "_".join(map(str, current_location))
        
        code_str = {"location_str": location_str,
                    "var_decl": "",
                    "declare_code": "",
                    "init_code": "",
                    "retrieve_code": "",
                    "publish_code": "",
                   }
        
        for module in _GetModuleChildren(self):
            if module["init"] != "":
                code_str["init_code"] += "  %s\n" % module["init"]
        
        # Adding variables
        vars = []
        self.ResetUsedLocations()
        for location in locations:
            loc = location["LOC"][len(current_location):]
            group = next = self
            i = 0
            while next is not None and i < len(loc):
                next = self._GetChildBySomething("IEC_Channel", loc[:i + 1])
                if next is not None:
                    i += 1
                    group = next
            var_loc = loc[i:]
            for variable in _GetModuleChildren(group):
                if variable["location"] == var_loc and location["DIR"] == LOCATION_DIRS[variable["type"]]:
#                    if location["DIR"] != LOCATION_DIRS[variable["type"]]:
#                        raise Exception, "Direction conflict in variable definition"
#                    if location["IEC_TYPE"] != variable["IEC_type"]:
#                        raise Exception, "Type conflict in variable definition"
                    if location["DIR"] == "Q":
                        if self.CheckLocationConflicts(location["LOC"]):
                            raise Exception, "BYTE and BIT from the same BYTE can't be used together"
                        self.AddUsedLocation(location["LOC"])
                    vars.append({"location": location["NAME"],
                                 "Type": variable["IEC_type"],
                                 "Retrieve": variable["retrieve"],
                                 "Publish": variable["publish"],
                                })
                    break
        base_types = self.GetCTRoot().GetBaseTypes()
        for var in vars:
            prefix = ""
            if var["Type"] in base_types:
                prefix = "IEC_"
            code_str["var_decl"] += "%s%s beremiz%s;\n"%(prefix, var["Type"], var["location"])
            code_str["var_decl"] += "%s%s *%s = &beremiz%s;\n"%(prefix, var["Type"], var["location"], var["location"])
            if var["Retrieve"] != "":
                code_str["retrieve_code"] += "  " + var["Retrieve"] % ("*" + var["location"]) + "\n"
            if var["Publish"] != "":
                code_str["publish_code"] += "  " + var["Publish"] % ("*" + var["location"]) + "\n"
        
        Gen_Module_path = os.path.join(buildpath, "Bus_%s.c"%location_str)
        module = open(Gen_Module_path,'w')
        module.write(BUS_TEXT % code_str)
        module.close()
        
        matiec_flags = '"-I%s" -Wno-unused-function'%os.path.abspath(self.GetCTRoot().GetIECLibPath())
        return [(Gen_Module_path, matiec_flags)],"",True

#-------------------------------------------------------------------------------
#                          LPC CanFestival ConfNode Class
#-------------------------------------------------------------------------------

if havecanfestival:

    DEFAULT_SETTINGS = {
        "CAN_Baudrate": "125K",
        "Slave_NodeId": 2,
        "Master_NodeId": 1,
    }
    
    class LPCSlaveEditor(SlaveEditor):
        SHOW_BASE_PARAMS = False
    
    class LPCCanOpenSlave(_SlaveCTN):
        XSD = """<?xml version="1.0" encoding="ISO-8859-1" ?>
        <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
          <xsd:element name="CanFestivalSlaveNode">
            <xsd:complexType>
              <xsd:attribute name="CAN_Baudrate" type="xsd:string" use="optional" default="%(CAN_Baudrate)s"/>
              <xsd:attribute name="NodeId" type="xsd:integer" use="optional" default="%(Slave_NodeId)d"/>
              <xsd:attribute name="Sync_Align" type="xsd:integer" use="optional" default="0"/>
              <xsd:attribute name="Sync_Align_Ratio" use="optional" default="50">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:integer">
                        <xsd:minInclusive value="1"/>
                        <xsd:maxInclusive value="99"/>
                    </xsd:restriction>
                </xsd:simpleType>
              </xsd:attribute>
            </xsd:complexType>
          </xsd:element>
        </xsd:schema>
        """ % DEFAULT_SETTINGS
        
        EditorType = LPCSlaveEditor
        
        def __init__(self):
            # TODO change netname when name change
            NodeManager.__init__(self)
            odfilepath = self.GetSlaveODPath()
            if(os.path.isfile(odfilepath)):
                self.OpenFileInCurrent(odfilepath)
            else:
                self.CreateNewNode("SlaveNode",  # Name - will be changed at build time
                                   0x00,         # NodeID - will be changed at build time
                                   "slave",      # Type
                                   "",           # description 
                                   "None",       # profile
                                   "", # prfile filepath
                                   "heartbeat",  # NMT
                                   [])           # options
                self.OnCTNSave()
        
        def GetCanDevice(self):
            return str(self.BaseParams.getIEC_Channel())
    
        ConfNodeMethods = [
            {"bitmap" : "NetworkEdit",
             "name" : _("Edit slave"), 
             "tooltip" : _("Edit CanOpen slave with ObjdictEdit"),
             "method" : "_OpenView"},
        ] + _SlaveCTN.ConfNodeMethods
    
    class LPCNetworkEditor(NetworkEditor):
        SHOW_BASE_PARAMS = False
        
    class LPCCanOpenMaster(_NodeListCTN):
        XSD = """<?xml version="1.0" encoding="ISO-8859-1" ?>
        <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
          <xsd:element name="CanFestivalNode">
            <xsd:complexType>
              <xsd:attribute name="CAN_Baudrate" type="xsd:string" use="optional" default="%(CAN_Baudrate)s"/>
              <xsd:attribute name="NodeId" type="xsd:integer" use="optional" default="%(Master_NodeId)d"/>
              <xsd:attribute name="Sync_TPDOs" type="xsd:boolean" use="optional" default="true"/>
            </xsd:complexType>
          </xsd:element>
        </xsd:schema>
        """ % DEFAULT_SETTINGS
        
        EditorType = LPCNetworkEditor
        
        def GetCanDevice(self):
            return str(self.BaseParams.getIEC_Channel())
    
        ConfNodeMethods = [
            {"bitmap" : "NetworkEdit",
             "name" : _("Edit network"), 
             "tooltip" : _("Edit CanOpen Network with NetworkEdit"),
             "method" : "_OpenView"},
        ] + _NodeListCTN.ConfNodeMethods
    
    class LPCCanOpen(CanOpenRootClass):
        XSD = None
        CTNChildrenTypes = [("CanOpenNode", LPCCanOpenMaster, "CanOpen Master"),
                            ("CanOpenSlave", LPCCanOpenSlave, "CanOpen Slave")]
        
        def GetCanDriver(self):
            return None
        
        def LoadChildren(self):
            ConfigTreeNode.LoadChildren(self)
            
            if self.GetChildByName("Master") is None:
                master = self.CTNAddChild("Master", "CanOpenNode", 0)
                #master.BaseParams.setEnabled(False)
                master.CTNRequestSave()
            
            if self.GetChildByName("Slave") is None:
                slave = self.CTNAddChild("Slave", "CanOpenSlave", 1)
                #slave.BaseParams.setEnabled(False)
                slave.CTNRequestSave()

#-------------------------------------------------------------------------------
#                              LPCProjectController Class
#-------------------------------------------------------------------------------

def mycopytree(src, dst):
    """
    Copy content of a directory to an other, omit hidden files
    @param src: source directory
    @param dst: destination directory
    """
    for i in os.listdir(src):
        if not i.startswith('.'):
            srcpath = os.path.join(src,i)
            dstpath = os.path.join(dst,i)
            if os.path.isdir(srcpath):
                if os.path.exists(dstpath):
                    shutil.rmtree(dstpath)
                os.makedirs(dstpath)
                mycopytree(srcpath, dstpath)
            elif os.path.isfile(srcpath):
                shutil.copy2(srcpath, dstpath)

[SIMULATION_MODE, TRANSFER_MODE] = range(2)

class LPCProjectNodeEditor(ProjectNodeEditor):
    SHOW_PARAMS = False
    ENABLE_REQUIRED = False

class LPCProjectController(ProjectController):

    StatusMethods = [
        {"bitmap" : "Debug",
         "name" : _("Simulate"),
         "tooltip" : _("Simulate PLC"),
         "method" : "_Simulate"},
        {"bitmap" : "Run",
         "name" : _("Run"),
         "shown" : False,
         "tooltip" : _("Start PLC"),
         "method" : "_Run"},
        {"bitmap" : "Stop",
         "name" : _("Stop"),
         "shown" : False,
         "tooltip" : _("Stop Running PLC"),
         "method" : "_Stop"},
        {"bitmap" : "Build",
         "name" : _("Build"),
         "tooltip" : _("Build project into build folder"),
         "method" : "_Build"},
        {"bitmap" : "Transfer",
         "name" : _("Transfer"),
         "shown" : False,
         "tooltip" : _("Transfer PLC"),
         "method" : "_Transfer"},
    ]
    
    ConfNodeMethods = []
    
    EditorType = LPCProjectNodeEditor
    
    def __init__(self, frame, logger, buildpath):
        self.OrigBuildPath = buildpath
        
        ProjectController.__init__(self, frame, logger)
        
        if havecanfestival:
            self.CTNChildrenTypes += [("LPCBus", LPCBus, "LPC bus"), ("CanOpen", LPCCanOpen, "CanOpen bus")]
        else:
            self.CTNChildrenTypes += [("LPCBus", LPCBus, "LPC bus")]
        self.CTNType = "LPC"
        
        self.OnlineMode = "OFF"
        self.LPCConnector = None
        self.ConnectorPath = None
        
        self.CurrentMode = None
        self.previous_mode = None
        
        self.SimulationBuildPath = None
        
        self.AbortTransferTimer = None
    
    def GetProjectInfos(self):
        infos = PLCControler.GetProjectInfos(self)
        configurations = infos["values"].pop(-1)
        resources = None
        for config_infos in configurations["values"]:
            if resources is None:
                resources = config_infos["values"][0]
            else:
                resources["values"].extend(config_infos["values"][0]["values"])
        if resources is not None:
            infos["values"].append(resources)
        return infos
    
    def GetProjectName(self):
        return self.Project.getname()

    def GetDefaultTargetName(self):
        if self.CurrentMode == SIMULATION_MODE:
            return ProjectController.GetDefaultTargetName(self)
        else:
            return "LPC"

    def GetTarget(self):
        target = ProjectController.GetTarget(self)
        if self.CurrentMode != SIMULATION_MODE:
            target.getcontent().setBuildPath(self.BuildPath)
        return target
    
    def _getBuildPath(self):
        if self.CurrentMode == SIMULATION_MODE:
            if self.SimulationBuildPath is None:
                self.SimulationBuildPath = os.path.join(tempfile.mkdtemp(), os.path.basename(self.ProjectPath), "build")
            return self.SimulationBuildPath
        else:
            return ProjectController._getBuildPath(self)

    def _Build(self):
        save = self.ProjectTestModified()
        if save:
            self.SaveProject()
            self.AppFrame._Refresh(TITLE, FILEMENU)
        if self.BuildPath is not None:
            mycopytree(self.OrigBuildPath, self.BuildPath)
        ProjectController._Build(self)
        if save:
            wx.CallAfter(self.AppFrame.RefreshAll)
    
    def SetProjectName(self, name):
        return self.Project.setname(name)

    def SetOnlineMode(self, mode, path=None):
        mode = mode.upper()
        if self.OnlineMode != mode:
            if mode not in ["OFF", ""]:
                self.OnlineMode = mode
                self.ConnectorPath = path
                uri = "LPC://%s/%s" % (self.OnlineMode,path)
                try:
                    self.LPCConnector = connectors.ConnectorFactory(uri, self)
                except Exception, msg:
                    self.logger.write_error(_("Exception while connecting %s!\n")%uri)
                    self.logger.write_error(traceback.format_exc())

                # Did connection success ?
                if self.LPCConnector is None:
                    # Oups.
                    self.logger.write_error(_("Connection failed to %s!\n")%uri)
                
            else:
                self.OnlineMode = "OFF"
                self.LPCConnector = None
                self.ConnectorPath = None
            
            self.ApplyOnlineMode()

    def ApplyOnlineMode(self):
        if self.CurrentMode != SIMULATION_MODE:
            self.KillDebugThread()
            
            self._SetConnector(self.LPCConnector)
            
            # Init with actual PLC status and print it
            self.UpdateMethodsFromPLCStatus()
                
            if self.LPCConnector is not None and self.OnlineMode == "APPLICATION":
                
                self.CompareLocalAndRemotePLC()
                            
                if self.previous_plcstate is not None:
                    status = _(self.previous_plcstate)
                else:
                    status = ""
                self.logger.write(_("PLC is %s\n")%status)
                
                #if self.StatusTimer and not self.StatusTimer.IsRunning():
                #    # Start the status Timer
                #    self.StatusTimer.Start(milliseconds=2000, oneShot=False)
                
                if self.previous_plcstate=="Started":
                    if self.DebugAvailable() and self.GetIECProgramsAndVariables():
                        self.logger.write(_("Debug connect matching running PLC\n"))
                        self._connect_debug()
                    else:
                        self.logger.write_warning(_("Debug do not match PLC - stop/transfert/start to re-enable\n"))
            
            elif self.StatusTimer and self.StatusTimer.IsRunning():
                self.StatusTimer.Stop()
            
            if self.CurrentMode == TRANSFER_MODE:
                
                if self.OnlineMode == "BOOTLOADER":
                    self.BeginTransfer()
                
                elif self.OnlineMode == "APPLICATION":
                    self.CurrentMode = None
                    self.AbortTransferTimer.Stop()
                    self.AbortTransferTimer = None
                    
                    self.logger.write(_("PLC transferred successfully\n"))
    
    # Update a PLCOpenEditor Pou variable location
    def UpdateProjectVariableLocation(self, old_leading, new_leading):
        self.Project.updateElementAddress(old_leading, new_leading)
        self.BufferProject()
    
    # Update a PLCOpenEditor Pou variable name
    def UpdateProjectVariableName(self, old_name, new_name):
        self.Project.updateElementName(old_name, new_name)
        self.BufferProject()

    def RemoveProjectVariableByAddress(self, address):
        self.Project.removeVariableByAddress(address)
        self.BufferProject()

    def RemoveProjectVariableByFilter(self, leading):
        self.Project.removeVariableByFilter(leading)
        self.BufferProject()

    def AddProjectDefaultConfiguration(self, config_name="config", res_name="resource1"):
        ProjectController.AddProjectDefaultConfiguration(self, config_name, res_name)
        
        self.SetEditedResourceInfos(
            self.ComputeConfigurationResourceName(config_name, res_name),
            [{"Name": "main_task",
              "Triggering": "Cyclic",
              "Interval": "T#50ms",
              "Priority": 0}],
            [])
    
    def LoadProject(self, ProjectPath, BuildPath=None):
        """
        Load a project contained in a folder
        @param ProjectPath: path of the project folder
        """
        if os.path.basename(ProjectPath) == "":
            ProjectPath = os.path.dirname(ProjectPath)
        
        # Verify that project contains a PLCOpen program
        plc_file = os.path.join(ProjectPath, "plc.xml")
        if os.path.isfile(plc_file):
            # Load PLCOpen file
            result = self.OpenXMLFile(plc_file)
            if result:
                return result
        else:
            self.CreateNewProject({"companyName": "",
                                   "productName": "",
                                   "productVersion": "",
                                   "projectName": "",
                                   "pageSize": (0, 0),
                                   "scaling": {}})
        if len(self.GetProjectConfigNames()) == 0:
            self.AddProjectDefaultConfiguration()
        
        # Change XSD into class members
        self._AddParamsMembers()
        self.Children = {}
        
        # Keep track of the root confnode (i.e. project path)
        self.ProjectPath = ProjectPath
        
        self.BuildPath = self._getBuildPath()
        if self.OrigBuildPath is not None:
            mycopytree(self.OrigBuildPath, self.BuildPath)
        
        # If dir have already be made, and file exist
        if os.path.isdir(self.CTNPath()) and os.path.isfile(self.ConfNodeXmlFilePath()):
            #Load the confnode.xml file into parameters members
            result = self.LoadXMLParams()
            if result:
                return result
            #Load and init all the children
            self.LoadChildren()
        
        canopen_child = self.GetChildByName("CanOpen")
        if arch == "MC8" and havecanfestival and canopen_child is None:
            canopen = self.CTNAddChild("CanOpen", "CanOpen", 0)
            canopen.LoadChildren()
            canopen.CTNRequestSave()
        
        elif (arch != "MC8" or not havecanfestival) and canopen_child is not None:
            canopen_child.CTNRemove()
        
        if self.CTNTestModified():
            self.SaveProject()
        
        if wx.GetApp() is None:
            self.RefreshConfNodesBlockLists()
        else:
            wx.CallAfter(self.RefreshConfNodesBlockLists)

        return None
    
    def IsPLCStarted(self):
        return self.previous_plcstate == "Started" or self.previous_mode == SIMULATION_MODE
    
    def UpdateMethodsFromPLCStatus(self):
        simulating = self.CurrentMode == SIMULATION_MODE
        if self.OnlineMode == "OFF":
            if simulating:
                status, log_count = self._connector.GetPLCstatus()
                self.UpdatePLCLog(log_count)
            status = "Disconnected"
        elif self.OnlineMode == "BOOTLOADER":
            status = "Connected"
        else:
            if self._connector is not None:
                status, log_count = self._connector.GetPLCstatus()
                if status == "Disconnected":
                    self._SetConnector(None, False)
                else:
                    self.UpdatePLCLog(log_count)
            else:
                status = "Disconnected"
        if self.previous_plcstate != status or self.previous_mode != self.CurrentMode:
            for args in {
                     "Started" :     [("_Simulate", False),
                                      ("_Run", False),
                                      ("_Stop", True),
                                      ("_Build", True),
                                      ("_Transfer", True)],
                     "Stopped" :     [("_Simulate", False),
                                      ("_Run", True),
                                      ("_Stop", False),
                                      ("_Build", True),
                                      ("_Transfer", True)],
                     "Connected" :   [("_Simulate", not simulating),
                                      ("_Run", True),
                                      ("_Stop", simulating),
                                      ("_Build", True),
                                      ("_Transfer", True)],
                     "Disconnected" :[("_Simulate", not simulating),
                                      ("_Run", False),
                                      ("_Stop", simulating),
                                      ("_Build", True),
                                      ("_Transfer", False)],
                   }.get(status,[]):
                self.ShowMethod(*args)
            self.previous_plcstate = status
            self.previous_mode = self.CurrentMode
            if self.AppFrame is not None:
                self.AppFrame.RefreshStatusToolBar()
                connection_text = _("Connected to: ")
                status_text = ""
                if simulating:
                    connection_text += _("Simulation")
                    status_text += _("ON")
                if status == "Disconnected":
                    if not simulating:
                        self.AppFrame.ConnectionStatusBar.SetStatusText(_(status), 1)
                        self.AppFrame.ConnectionStatusBar.SetStatusText('', 2)
                    else:
                        self.AppFrame.ConnectionStatusBar.SetStatusText(connection_text, 1)
                        self.AppFrame.ConnectionStatusBar.SetStatusText(status_text, 2)
                else:
                    if simulating:
                        connection_text += " (%s)"
                        status_text += " (%s)"
                    else:
                        connection_text += "%s"
                        status_text += "%s"
                    self.AppFrame.ConnectionStatusBar.SetStatusText(connection_text % self.ConnectorPath, 1)
                    self.AppFrame.ConnectionStatusBar.SetStatusText(status_text % _(status), 2)
            return True
        return False

    def Generate_plc_declare_locations(self):
        """
        Declare used locations in order to simulatePLC in a black box
        """
        return """#include "iec_types_all.h"

#define __LOCATED_VAR(type, name, ...) \
type beremiz_##name;\
type *name = &beremiz_##name;

#include "LOCATED_VARIABLES.h"

#undef __LOCATED_VAR

"""

    def Generate_lpc_retain_array_sim(self):
        """
        Support for retain array in Simulation
        """
        return """/* Support for retain array */
#define USER_RETAIN_ARRAY_SIZE 2000
#define NUM_OF_COLS    3
unsigned char readOK = 0;
unsigned int foundIndex = USER_RETAIN_ARRAY_SIZE;
unsigned int retainArray[USER_RETAIN_ARRAY_SIZE][NUM_OF_COLS];

unsigned int __GetRetainData(unsigned char READ, unsigned int INDEX, unsigned int COLUMN)
{
    if(READ == 1)
    {
        if((0<=INDEX) && (INDEX<USER_RETAIN_ARRAY_SIZE) && (0<=COLUMN) && (COLUMN<NUM_OF_COLS))
        {
            readOK = 1;
            return retainArray[INDEX][COLUMN];
        }
    }

    readOK = 0;
    return 0;
}

unsigned char __SetRetainData(unsigned char WRITE, unsigned int INDEX, unsigned int WORD1, unsigned int WORD2, unsigned int WORD3)
{
    if(WRITE == 1)
    {
        if((0<=INDEX) && (INDEX<USER_RETAIN_ARRAY_SIZE))
        {
            retainArray[INDEX][0] = WORD1;
            retainArray[INDEX][1] = WORD2;
            retainArray[INDEX][2] = WORD3;
            return 1;
        }
    }
    
    return 0;
}

unsigned char __FindRetainData(unsigned char SEARCH, unsigned int START_IDX, unsigned int END_IDX, unsigned int WORD1, unsigned int WORD2, unsigned int WORD3)
{
    unsigned int i;

    if((SEARCH==1) && (0<=START_IDX) && (START_IDX<USER_RETAIN_ARRAY_SIZE) && (START_IDX<=END_IDX) && (END_IDX<USER_RETAIN_ARRAY_SIZE))
    {
        for(i=START_IDX;i<=END_IDX;i++)
        {
            if((retainArray[i][0] == WORD1) && (retainArray[i][1] == WORD2) && (retainArray[i][2] == WORD3))
            {
                foundIndex = i;
                return 1;
            }
        }
    }

    foundIndex = USER_RETAIN_ARRAY_SIZE;    /* Data not found => return index that is out of array bounds */
    return 0;
}

/* Since Beremiz debugger doesn't like pointer-by-reference stuff or global varibles, separate function is a must */
unsigned char __GetReadStatus(unsigned char dummy)
{
    return readOK;
}

unsigned int __GetFoundIndex(unsigned char dummy)
{
    return foundIndex;
}
"""

    def _Simulate(self):
        """
        Method called by user to Simulate PLC
        """
        uri = "LOCAL://"
        try:
            self._SetConnector(connectors.ConnectorFactory(uri, self))
        except Exception, msg:
            self.logger.write_error(_("Exception while connecting %s!\n")%uri)
            self.logger.write_error(traceback.format_exc())

        # Did connection success ?
        if self._connector is None:
            # Oups.
            self.logger.write_error(_("Connection failed to %s!\n")%uri)
            self.StopSimulation()
            return False
        
        self.CurrentMode = SIMULATION_MODE
        
        buildpath = self._getBuildPath()
        
        # Eventually create build dir
        if not os.path.exists(buildpath):
            os.makedirs(buildpath)
        
        # Generate SoftPLC IEC code
        IECGenRes = self._Generate_SoftPLC()
        
         # If IEC code gen fail, bail out.
        if not IECGenRes:
            self.logger.write_error(_("IEC-61131-3 code generation failed !\n"))
            self.StopSimulation()
            return False

        # Reset variable and program list that are parsed from
        # CSV file generated by IEC2C compiler.
        self.ResetIECProgramsAndVariables()
        
        gen_result = self.CTNGenerate_C(buildpath, self.PLCGeneratedLocatedVars)
        CTNCFilesAndCFLAGS, CTNLDFLAGS, DoCalls = gen_result[:3]
        # if some files have been generated put them in the list with their location
        if CTNCFilesAndCFLAGS:
            self.LocationCFilesAndCFLAGS = [(self.GetCurrentLocation(), CTNCFilesAndCFLAGS, DoCalls)]
        else:
            self.LocationCFilesAndCFLAGS = []

        # confnode asks for some LDFLAGS
        if CTNLDFLAGS:
            # LDFLAGS can be either string
            if type(CTNLDFLAGS)==type(str()):
                self.LDFLAGS=[CTNLDFLAGS]
            #or list of strings
            elif type(CTNLDFLAGS)==type(list()):
                self.LDFLAGS=CTNLDFLAGS[:]
        else:
            self.LDFLAGS=[]
        
        # Header file for extensions
        open(os.path.join(buildpath,"beremiz.h"), "w").write(targets.GetHeader())
        
        # Template based part of C code generation
        # files are stacked at the beginning, as files of confnode tree root
        for generator, filename, name in [
           # debugger code
           (self.Generate_plc_debugger, "plc_debugger.c", "Debugger"),
           # init/cleanup/retrieve/publish, run and align code
           (self.Generate_plc_main,"plc_main.c","Common runtime"),
           # declare located variables for simulate in a black box
           (self.Generate_plc_declare_locations,"plc_declare_locations.c","Declare Locations"),
           # declare located variables for simulate in a black box
           (self.Generate_lpc_retain_array_sim,"lpc_retain_array_sim.c","Retain Array for Simulation")]:
            try:
                # Do generate
                code = generator()
                if code is None:
                     raise
                code_path = os.path.join(buildpath,filename)
                open(code_path, "w").write(code)
                # Insert this file as first file to be compiled at root confnode
                self.LocationCFilesAndCFLAGS[0][1].insert(0,(code_path, self.plcCFLAGS))
            except Exception, exc:
                self.logger.write_error(name+_(" generation failed !\n"))
                self.logger.write_error(traceback.format_exc())
                self.StopSimulation()
                return False
        
        # Get simulation builder
        builder = self.GetBuilder()
        if builder is None:
            self.logger.write_error(_("Fatal : cannot get builder.\n"))
            self.StopSimulation()
            return False

        # Build
        try:
            if not builder.build() :
                self.logger.write_error(_("C Build failed.\n"))
                self.StopSimulation()
                return False
        except Exception, exc:
            self.logger.write_error(_("C Build crashed !\n"))
            self.logger.write_error(traceback.format_exc())
            self.StopSimulation()
            return False

        data = builder.GetBinaryCode()
        if data is not None :
            if self._connector.NewPLC(builder.GetBinaryCodeMD5(), data, []):
                self.UnsubscribeAllDebugIECVariable()
                self.ProgramTransferred()
                if self.AppFrame is not None:
                    self.AppFrame.CloseObsoleteDebugTabs()
                    self.AppFrame.RefreshPouInstanceVariablesPanel()
                self.logger.write(_("Transfer completed successfully.\n"))
            else:
                self.logger.write_error(_("Transfer failed\n"))
                self.StopSimulation()
                return False
        
        self._Run()
                
        if not self.StatusTimer.IsRunning():
            # Start the status Timer
            self.StatusTimer.Start(milliseconds=500, oneShot=False)
    
    def StopSimulation(self):
        self.CurrentMode = None
        self._SetConnector(None, False)
        self.ApplyOnlineMode()
    
    def _Stop(self):
        ProjectController._Stop(self)
        
        if self.CurrentMode == SIMULATION_MODE:
            self.StopSimulation()

    def CompareLocalAndRemotePLC(self):
        if self.LPCConnector is None:
            return
        # We are now connected. Update button status
        MD5 = self.GetLastBuildMD5()
        # Check remote target PLC correspondance to that md5
        if MD5 is not None and self.LPCConnector.MatchMD5(MD5):
            # warns controller that program match
            self.ProgramTransferred()

    def ResetBuildMD5(self):
        builder=self.GetBuilder()
        if builder is not None:
            builder.ResetBinaryCodeMD5(self.OnlineMode)
        
    def GetLastBuildMD5(self):
        builder=self.GetBuilder()
        if builder is not None:
            return builder.GetBinaryCodeMD5(self.OnlineMode)
        else:
            return None

    def _Transfer(self):
        if self.CurrentMode is None and self.OnlineMode != "OFF":
            self.CurrentMode = TRANSFER_MODE
            
            if ProjectController._Build(self):
            
                ID_ABORTTRANSFERTIMER = wx.NewId()
                self.AbortTransferTimer = wx.Timer(self.AppFrame, ID_ABORTTRANSFERTIMER)
                self.AppFrame.Bind(wx.EVT_TIMER, self.AbortTransfer, self.AbortTransferTimer)  
                
                if self.OnlineMode == "BOOTLOADER":
                    self.BeginTransfer()
                
                else:
                    self.logger.write(_("Resetting PLC\n"))
                    #self.StatusTimer.Stop()
                    self.LPCConnector.ResetPLC()
                    self.AbortTransferTimer.Start(milliseconds=5000, oneShot=True)
            
            else:
                self.CurrentMode = None
    
    def BeginTransfer(self):
        self.logger.write(_("Start PLC transfer\n"))
        
        self.AbortTransferTimer.Stop()
        ProjectController._Transfer(self)
        self.AbortTransferTimer.Start(milliseconds=5000, oneShot=True)
    
    def AbortTransfer(self, event):
        self.logger.write_warning(_("Timeout waiting PLC to recover\n"))
        
        self.CurrentMode = None
        self.AbortTransferTimer.Stop()
        self.AbortTransferTimer = None
        event.Skip()

    def _Run(self):
        """
        Start PLC
        """
        if self.GetIECProgramsAndVariables():
            self._connector.StartPLC()
            self.logger.write(_("Starting PLC\n"))
            self._connect_debug()
        else:
            self.logger.write_error(_("Couldn't start PLC !\n"))
        self.UpdateMethodsFromPLCStatus()

#-------------------------------------------------------------------------------
#                              LPCBeremiz Class
#-------------------------------------------------------------------------------
lpcberemiz_cmd=None

class LPCBeremiz(Beremiz):
    
    def _init_coll_FileMenu_Items(self, parent):
        AppendMenu(parent, help='', id=wx.ID_SAVE,
              kind=wx.ITEM_NORMAL, text=_(u'Save\tCTRL+S'))
        AppendMenu(parent, help='', id=wx.ID_CLOSE,
              kind=wx.ITEM_NORMAL, text=_(u'Close Tab\tCTRL+W'))
        parent.AppendSeparator()
        AppendMenu(parent, help='', id=wx.ID_PAGE_SETUP,
              kind=wx.ITEM_NORMAL, text=_(u'Page Setup'))
        AppendMenu(parent, help='', id=wx.ID_PREVIEW,
              kind=wx.ITEM_NORMAL, text=_(u'Preview'))
        AppendMenu(parent, help='', id=wx.ID_PRINT,
              kind=wx.ITEM_NORMAL, text=_(u'Print'))
        parent.AppendSeparator()
        AppendMenu(parent, help='', id=wx.ID_EXIT,
              kind=wx.ITEM_NORMAL, text=_(u'Quit\tCTRL+Q'))
        
        self.Bind(wx.EVT_MENU, self.OnSaveProjectMenu, id=wx.ID_SAVE)
        self.Bind(wx.EVT_MENU, self.OnCloseTabMenu, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_MENU, self.OnPageSetupMenu, id=wx.ID_PAGE_SETUP)
        self.Bind(wx.EVT_MENU, self.OnPreviewMenu, id=wx.ID_PREVIEW)
        self.Bind(wx.EVT_MENU, self.OnPrintMenu, id=wx.ID_PRINT)
        self.Bind(wx.EVT_MENU, self.OnQuitMenu, id=wx.ID_EXIT)
    
        self.AddToMenuToolBar([(wx.ID_SAVE, "save", _(u'Save'), None),
                               (wx.ID_PRINT, "print", _(u'Print'), None)])
    
    def _init_coll_AddMenu_Items(self, parent):
        IDEFrame._init_coll_AddMenu_Items(self, parent, False)
    
    def _init_ctrls(self, prnt):
        Beremiz._init_ctrls(self, prnt)
        
        self.PLCConfig = wx.ScrolledWindow(
              name='PLCConfig', parent=self.LeftNoteBook, pos=wx.Point(0, 0),
              size=wx.Size(-1, -1), style=wx.TAB_TRAVERSAL|wx.SUNKEN_BORDER|wx.HSCROLL|wx.VSCROLL)
        self.PLCConfig.SetBackgroundColour(wx.WHITE)
        self.PLCConfig.Bind(wx.EVT_LEFT_DOWN, self.OnPanelLeftDown)
        self.PLCConfig.Bind(wx.EVT_SIZE, self.OnMoveWindow)
        self.MainTabs["PLCConfig"] = (self.PLCConfig, _("Topology"))
        self.LeftNoteBook.InsertPage(0, self.PLCConfig, _("Topology"), True)
        
        self.PLCConfigMainSizer = wx.FlexGridSizer(cols=1, hgap=2, rows=2, vgap=2)
        self.PLCParamsSizer = wx.BoxSizer(wx.VERTICAL)
        self.ConfNodeTreeSizer = wx.FlexGridSizer(cols=1, hgap=0, rows=0, vgap=2)
        self.ConfNodeTreeSizer.AddGrowableCol(0)
        
        self.PLCConfigMainSizer.AddSizer(self.PLCParamsSizer, 0, border=10, flag=wx.GROW|wx.TOP|wx.LEFT|wx.RIGHT)
        self.PLCConfigMainSizer.AddSizer(self.ConfNodeTreeSizer, 0, border=10, flag=wx.BOTTOM|wx.LEFT|wx.RIGHT)
        self.PLCConfigMainSizer.AddGrowableCol(0)
        self.PLCConfigMainSizer.AddGrowableRow(1)
        
        self.PLCConfig.SetSizer(self.PLCConfigMainSizer)
        
        self.AUIManager.Update()

    def __init__(self, parent, projectOpen=None, buildpath=None, ctr=None, debug=True):
        self.ConfNodeInfos = {}
        
        Beremiz.__init__(self, parent, projectOpen, buildpath, ctr, debug)
    
    def Show(self):
        wx.Frame.Show(self)
    
    def OnCloseFrame(self, event):
        global frame
        
        if self.CheckSaveBeforeClosing(_("Close Application")):
            
            frame.Hide()
            
            self.CTR.ResetAppFrame(lpcberemiz_cmd.Log)
            if self.CTR.OnlineMode == 0:
                self.CTR._SetConnector(None, False)
            
            self.CTR.KillDebugThread()
            self.KillLocalRuntime()
            
            self.SaveLastState()
            
            lpcberemiz_cmd.Log.write("Closed\n")
            
        event.Veto()

    def OnMoveWindow(self, event):
        self.GetBestSize()
        self.RefreshScrollBars()
        event.Skip()

    def OnPanelLeftDown(self, event):
        focused = self.FindFocus()
        if isinstance(focused, TextCtrlAutoComplete):
            focused.DismissListBox()
        event.Skip()

    def RefreshFileMenu(self):
        MenuToolBar = self.Panes["MenuToolBar"]
        if self.CTR is not None:
            selected = self.TabsOpened.GetSelection()
            if selected >= 0:
                graphic_viewer = isinstance(self.TabsOpened.GetPage(selected), Viewer)
            else:
                graphic_viewer = False
            if self.TabsOpened.GetPageCount() > 0:
                self.FileMenu.Enable(wx.ID_CLOSE, True)
                if graphic_viewer:
                    self.FileMenu.Enable(wx.ID_PREVIEW, True)
                    self.FileMenu.Enable(wx.ID_PRINT, True)
                    MenuToolBar.EnableTool(wx.ID_PRINT, True)
                else:
                    self.FileMenu.Enable(wx.ID_PREVIEW, False)
                    self.FileMenu.Enable(wx.ID_PRINT, False)
                    MenuToolBar.EnableTool(wx.ID_PRINT, False)
            else:
                self.FileMenu.Enable(wx.ID_CLOSE, False)
                self.FileMenu.Enable(wx.ID_PREVIEW, False)
                self.FileMenu.Enable(wx.ID_PRINT, False)
                MenuToolBar.EnableTool(wx.ID_PRINT, False)
            self.FileMenu.Enable(wx.ID_PAGE_SETUP, True)
            project_modified = self.CTR.ProjectTestModified()
            self.FileMenu.Enable(wx.ID_SAVE, project_modified)
            MenuToolBar.EnableTool(wx.ID_SAVE, project_modified)
        else:
            self.FileMenu.Enable(wx.ID_CLOSE, False)
            self.FileMenu.Enable(wx.ID_PAGE_SETUP, False)
            self.FileMenu.Enable(wx.ID_PREVIEW, False)
            self.FileMenu.Enable(wx.ID_PRINT, False)
            MenuToolBar.EnableTool(wx.ID_PRINT, False)
            self.FileMenu.Enable(wx.ID_SAVE, False)
            MenuToolBar.EnableTool(wx.ID_SAVE, False)
            
    def RefreshScrollBars(self):
        xstart, ystart = self.PLCConfig.GetViewStart()
        window_size = self.PLCConfig.GetClientSize()
        sizer = self.PLCConfig.GetSizer()
        if sizer:
            maxx, maxy = sizer.GetMinSize()
            posx = max(0, min(xstart, (maxx - window_size[0]) / SCROLLBAR_UNIT))
            posy = max(0, min(ystart, (maxy - window_size[1]) / SCROLLBAR_UNIT))
            self.PLCConfig.Scroll(posx, posy)
            self.PLCConfig.SetScrollbars(SCROLLBAR_UNIT, SCROLLBAR_UNIT, 
                maxx / SCROLLBAR_UNIT, maxy / SCROLLBAR_UNIT, posx, posy)

    def RefreshPLCParams(self):
        self.Freeze()
        self.ClearSizer(self.PLCParamsSizer)
        
        if self.CTR is not None:    
            plcwindow = wx.Panel(self.PLCConfig, -1, size=wx.Size(-1, -1))
            if self.CTR.CTNTestModified():
                bkgdclr = CHANGED_TITLE_COLOUR
            else:
                bkgdclr = TITLE_COLOUR
                
            plcwindow.SetBackgroundColour(TITLE_COLOUR)
            plcwindow.Bind(wx.EVT_LEFT_DOWN, self.OnPanelLeftDown)
            self.PLCParamsSizer.AddWindow(plcwindow, 0, border=0, flag=wx.GROW)
            
            plcwindowsizer = wx.BoxSizer(wx.HORIZONTAL)
            plcwindow.SetSizer(plcwindowsizer)
            
            st = wx.StaticText(plcwindow, -1)
            st.SetFont(wx.Font(faces["size"], wx.DEFAULT, wx.NORMAL, wx.BOLD, faceName = faces["helv"]))
            st.SetLabel(self.CTR.GetProjectName())
            plcwindowsizer.AddWindow(st, 0, border=5, flag=wx.ALL|wx.ALIGN_CENTER)
            
            plcwindowmainsizer = wx.BoxSizer(wx.VERTICAL)
            plcwindowsizer.AddSizer(plcwindowmainsizer, 0, border=5, flag=wx.ALL)
            
            plcwindowbuttonsizer = wx.BoxSizer(wx.HORIZONTAL)
            plcwindowmainsizer.AddSizer(plcwindowbuttonsizer, 0, border=0, flag=wx.ALIGN_CENTER)
            
            msizer = self.GenerateMethodButtonSizer(self.CTR, plcwindow)
            plcwindowbuttonsizer.AddSizer(msizer, 0, border=0, flag=wx.GROW)
            
        self.PLCConfigMainSizer.Layout()
        self.RefreshScrollBars()
        self.Thaw()

    def GenerateMethodButtonSizer(self, confnode, parent):
        normal_bt_font=wx.Font(faces["size"] / 3, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName = faces["helv"])
        mouseover_bt_font=wx.Font(faces["size"] / 3, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=True, faceName = faces["helv"])
        msizer = wx.FlexGridSizer(cols=len(confnode.ConfNodeMethods))
        for confnode_method in confnode.ConfNodeMethods:
            if "method" in confnode_method and confnode_method.get("shown",True):
                id = wx.NewId()
                label = confnode_method["name"]
                button = GenBitmapTextButton(id=id, parent=parent,
                    bitmap=wx.Bitmap(Bpath("images", "%s.png"%confnode_method.get("bitmap", "Unknown"))), label=label, 
                    name=label, pos=wx.DefaultPosition, style=wx.NO_BORDER)
                button.SetFont(normal_bt_font)
                button.SetToolTipString(confnode_method["tooltip"])
                button.Bind(wx.EVT_BUTTON, self.GetButtonCallBackFunction(confnode, confnode_method["method"]), id=id)
                # a fancy underline on mouseover
                def setFontStyle(b, s):
                    def fn(event):
                        b.SetFont(s)
                        b.Refresh()
                        event.Skip()
                    return fn
                button.Bind(wx.EVT_ENTER_WINDOW, setFontStyle(button, mouseover_bt_font))
                button.Bind(wx.EVT_LEAVE_WINDOW, setFontStyle(button, normal_bt_font))
                #hack to force size to mini
                if not confnode_method.get("enabled",True):
                    button.Disable()
                msizer.AddWindow(button, 0, border=0, flag=wx.ALIGN_CENTER)
        return msizer

    def GenerateEnableButton(self, parent, sizer, confnode):
        enabled = confnode.CTNEnabled()
        if enabled is not None:
            enablebutton_id = wx.NewId()
            enablebutton = wx.lib.buttons.GenBitmapToggleButton(id=enablebutton_id, bitmap=wx.Bitmap(Bpath( 'images', 'Disabled.png')),
                  name='EnableButton', parent=parent, size=wx.Size(16, 16), pos=wx.Point(0, 0), style=0)#wx.NO_BORDER)
            enablebutton.SetToolTipString(_("Enable/Disable this confnode"))
            make_genbitmaptogglebutton_flat(enablebutton)
            enablebutton.SetBitmapSelected(wx.Bitmap(Bpath( 'images', 'Enabled.png')))
            enablebutton.SetToggle(enabled)
            def toggleenablebutton(event):
                res = self.SetConfNodeParamsAttribute(confnode, "BaseParams.Enabled", enablebutton.GetToggle())
                enablebutton.SetToggle(res)
                event.Skip()
            enablebutton.Bind(wx.EVT_BUTTON, toggleenablebutton, id=enablebutton_id)
            sizer.AddWindow(enablebutton, 0, border=0, flag=wx.RIGHT|wx.ALIGN_CENTER_VERTICAL)
        else:
            sizer.AddSpacer(wx.Size(16, 16))

    def RefreshConfNodeTree(self):
        self.Freeze()
        self.ClearSizer(self.ConfNodeTreeSizer)
        if self.CTR is not None:
            for child in self.CTR.IECSortedChildren():
                self.GenerateTreeBranch(child)
                if not self.ConfNodeInfos[child]["expanded"]:
                    self.CollapseConfNode(child)
        self.PLCConfigMainSizer.Layout()
        self.RefreshScrollBars()
        self.Thaw()

    def GenerateTreeBranch(self, confnode):
        nodewindow = wx.Panel(self.PLCConfig, -1, size=wx.Size(-1, -1))
        if confnode.CTNTestModified():
            bkgdclr=CHANGED_WINDOW_COLOUR
        else:
            bkgdclr=WINDOW_COLOUR

        nodewindow.SetBackgroundColour(bkgdclr)
        
        if confnode not in self.ConfNodeInfos:
            self.ConfNodeInfos[confnode] = {"expanded" : False, "visible" : False}
            
        self.ConfNodeInfos[confnode]["children"] = confnode.IECSortedChildren()
        confnode_infos = confnode.GetVariableLocationTree()
        confnode_locations = []
        if len(self.ConfNodeInfos[confnode]["children"]) == 0:
            confnode_locations = confnode_infos["children"]
            if not self.ConfNodeInfos[confnode].has_key("locations_infos"):
                self.ConfNodeInfos[confnode]["locations_infos"] = {"root": {"expanded" : False}}
            
            self.ConfNodeInfos[confnode]["locations_infos"]["root"]["window"] = None
            self.ConfNodeInfos[confnode]["locations_infos"]["root"]["children"] = []
        
        self.ConfNodeTreeSizer.AddWindow(nodewindow, 0, border=0, flag=wx.GROW)
        
        nodewindowvsizer = wx.BoxSizer(wx.VERTICAL)
        nodewindow.SetSizer(nodewindowvsizer)
        
        nodewindowsizer = wx.BoxSizer(wx.HORIZONTAL)
        nodewindowvsizer.AddSizer(nodewindowsizer, 0, border=0, flag=0)
        
        #self.GenerateEnableButton(nodewindow, nodewindowsizer, confnode)
        
        st = wx.StaticText(nodewindow, -1)
        st.SetFont(wx.Font(faces["size"], wx.DEFAULT, wx.NORMAL, wx.BOLD, faceName = faces["helv"]))
        st.SetLabel(confnode.GetFullIEC_Channel())
        nodewindowsizer.AddWindow(st, 0, border=5, flag=wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL)
        
        expandbutton_id = wx.NewId()
        expandbutton = wx.lib.buttons.GenBitmapToggleButton(id=expandbutton_id, bitmap=wx.Bitmap(Bpath( 'images', 'plus.png')),
              name='ExpandButton', parent=nodewindow, pos=wx.Point(0, 0),
              size=wx.Size(13, 13), style=wx.NO_BORDER)
        expandbutton.labelDelta = 0
        expandbutton.SetBezelWidth(0)
        expandbutton.SetUseFocusIndicator(False)
        expandbutton.SetBitmapSelected(wx.Bitmap(Bpath( 'images', 'minus.png')))
            
        if len(self.ConfNodeInfos[confnode]["children"]) > 0:
            expandbutton.SetToggle(self.ConfNodeInfos[confnode]["expanded"])
            def togglebutton(event):
                if expandbutton.GetToggle():
                    self.ExpandConfNode(confnode)
                else:
                    self.CollapseConfNode(confnode)
                self.ConfNodeInfos[confnode]["expanded"] = expandbutton.GetToggle()
                self.PLCConfigMainSizer.Layout()
                self.RefreshScrollBars()
                event.Skip()
            expandbutton.Bind(wx.EVT_BUTTON, togglebutton, id=expandbutton_id)
        elif len(confnode_locations) > 0:
            locations_infos = self.ConfNodeInfos[confnode]["locations_infos"]
            expandbutton.SetToggle(locations_infos["root"]["expanded"])
            def togglebutton(event):
                if expandbutton.GetToggle():
                    self.ExpandLocation(locations_infos, "root")
                else:
                    self.CollapseLocation(locations_infos, "root")
                self.ConfNodeInfos[confnode]["expanded"] = expandbutton.GetToggle()
                locations_infos["root"]["expanded"] = expandbutton.GetToggle()
                self.PLCConfigMainSizer.Layout()
                self.RefreshScrollBars()
                event.Skip()
            expandbutton.Bind(wx.EVT_BUTTON, togglebutton, id=expandbutton_id)
        else:
            expandbutton.Enable(False)
        nodewindowsizer.AddWindow(expandbutton, 0, border=5, flag=wx.RIGHT|wx.ALIGN_CENTER_VERTICAL)
        
        sb = wx.StaticBitmap(nodewindow, -1)
        icon = confnode_infos.get("icon", None)
        if icon is None:
            icon_bitmap = self.LocationImageList.GetBitmap(self.LocationImageDict[confnode_infos["type"]])
        else: 
            icon_bitmap = wx.Bitmap(icon)
        sb.SetBitmap(icon_bitmap)
        nodewindowsizer.AddWindow(sb, 0, border=5, flag=wx.RIGHT|wx.ALIGN_CENTER_VERTICAL)
        
        st_id = wx.NewId()
        st = wx.StaticText(nodewindow, st_id, size=wx.DefaultSize, style=wx.NO_BORDER)
        st.SetFont(wx.Font(faces["size"] * 0.75, wx.DEFAULT, wx.NORMAL, wx.BOLD, faceName = faces["helv"]))
        st.SetLabel(confnode.MandatoryParams[1].getName())
        nodewindowsizer.AddWindow(st, 0, border=5, flag=wx.RIGHT|wx.ALIGN_CENTER_VERTICAL)
        
        buttons_sizer = self.GenerateMethodButtonSizer(confnode, nodewindow)
        nodewindowsizer.AddSizer(buttons_sizer, flag=wx.ALIGN_CENTER_VERTICAL)
        
        self.ConfNodeInfos[confnode]["window"] = nodewindow
        for child in self.ConfNodeInfos[confnode]["children"]:
            self.GenerateTreeBranch(child)
            if not self.ConfNodeInfos[child]["expanded"]:
                self.CollapseConfNode(child)
        
        if len(confnode_locations) > 0:
            locations_infos = self.ConfNodeInfos[confnode]["locations_infos"]
            treectrl = wx.TreeCtrl(self.PLCConfig, -1, size=wx.DefaultSize, 
                                   style=wx.TR_HAS_BUTTONS|wx.TR_SINGLE|wx.NO_BORDER|wx.TR_HIDE_ROOT|wx.TR_NO_LINES|wx.TR_LINES_AT_ROOT)
            treectrl.SetImageList(self.LocationImageList)
            treectrl.Bind(wx.EVT_TREE_BEGIN_DRAG, self.GenerateLocationBeginDragFunction(locations_infos))
            treectrl.Bind(wx.EVT_TREE_ITEM_EXPANDED, self.GenerateLocationExpandCollapseFunction(locations_infos, True))
            treectrl.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.GenerateLocationExpandCollapseFunction(locations_infos, False))
            
            treectrl.AddRoot("")
            self.ConfNodeTreeSizer.AddWindow(treectrl, 0, border=0, flag=0)
            
            locations_infos["root"]["window"] = treectrl
            for location in confnode_locations:
                locations_infos["root"]["children"].append("root.%s" % location["name"])
                self.GenerateLocationTreeBranch(treectrl, treectrl.GetRootItem(), locations_infos, "root", location)
            if locations_infos["root"]["expanded"]:
                self.ExpandLocation(locations_infos, "root")

    def ExpandConfNode(self, confnode, force = False):
        for child in self.ConfNodeInfos[confnode]["children"]:
            self.ConfNodeInfos[child]["window"].Show()
            if force or self.ConfNodeInfos[child]["expanded"]:
                self.ExpandConfNode(child, force)
                if force:
                    self.ConfNodeInfos[child]["expanded"] = True
        locations_infos = self.ConfNodeInfos[confnode].get("locations_infos", None)
        if locations_infos is not None:
            if force or locations_infos["root"]["expanded"]:
                self.ExpandLocation(locations_infos, "root", force)
                if force:
                    locations_infos["root"]["expanded"] = True
    
    def CollapseConfNode(self, confnode, force = False):
        for child in self.ConfNodeInfos[confnode]["children"]:
            self.ConfNodeInfos[child]["window"].Hide()
            self.CollapseConfNode(child, force)
            if force:
                self.ConfNodeInfos[child]["expanded"] = False
        locations_infos = self.ConfNodeInfos[confnode].get("locations_infos", None)
        if locations_infos is not None:
            self.CollapseLocation(locations_infos, "root", force)
            if force:
                locations_infos["root"]["expanded"] = False

    def ExpandLocation(self, locations_infos, group, force = False, refresh_size=True):
        locations_infos[group]["expanded"] = True
        if group == "root":
            if locations_infos[group]["window"] is not None:
                locations_infos[group]["window"].Show()
        elif locations_infos["root"]["window"] is not None:
            locations_infos["root"]["window"].Expand(locations_infos[group]["item"])
            if force:
                for child in locations_infos[group]["children"]:
                    self.ExpandLocation(locations_infos, child, force, False)
        if locations_infos["root"]["window"] is not None and refresh_size:
            self.RefreshTreeCtrlSize(locations_infos["root"]["window"])
        
    def CollapseLocation(self, locations_infos, group, force = False, refresh_size=True):
        locations_infos[group]["expanded"] = False
        if group == "root":
            if locations_infos[group]["window"] is not None:
                locations_infos[group]["window"].Hide()
        elif locations_infos["root"]["window"] is not None:
            locations_infos["root"]["window"].Collapse(locations_infos[group]["item"])
            if force:
                for child in locations_infos[group]["children"]:
                    self.CollapseLocation(locations_infos, child, force, False)
        if locations_infos["root"]["window"] is not None and refresh_size:
            self.RefreshTreeCtrlSize(locations_infos["root"]["window"])

    def GenerateLocationTreeBranch(self, treectrl, root, locations_infos, parent, location):
        location_name = "%s.%s" % (parent, location["name"])
        if not locations_infos.has_key(location_name):
            locations_infos[location_name] = {"expanded" : False}
        
        if location["type"] in [LOCATION_VAR_INPUT, LOCATION_VAR_OUTPUT, LOCATION_VAR_MEMORY]:
            label = "%(name)s (%(location)s)" % location
        elif location["location"] != "":
            label = "%(location)s: %(name)s" % location
        else:
            label = location["name"]
        item = treectrl.AppendItem(root, label)
        treectrl.SetPyData(item, location_name)
        treectrl.SetItemImage(item, self.LocationImageDict[location["type"]])
        
        locations_infos[location_name]["item"] = item
        locations_infos[location_name]["children"] = []
        infos = location.copy()
        infos.pop("children")
        locations_infos[location_name]["infos"] = infos
        for child in location["children"]:
            child_name = "%s.%s" % (location_name, child["name"])
            locations_infos[location_name]["children"].append(child_name)
            self.GenerateLocationTreeBranch(treectrl, item, locations_infos, location_name, child)
        if locations_infos[location_name]["expanded"]:
            self.ExpandLocation(locations_infos, location_name)
    
    def GenerateLocationBeginDragFunction(self, locations_infos):
        def OnLocationBeginDragFunction(event):
            item = event.GetItem()
            location_name = locations_infos["root"]["window"].GetPyData(item)
            if location_name is not None:
                infos = locations_infos[location_name]["infos"]
                if infos["type"] in [LOCATION_VAR_INPUT, LOCATION_VAR_OUTPUT, LOCATION_VAR_MEMORY]:
                    data = wx.TextDataObject(str((infos["location"], "location", infos["IEC_type"], infos["var_name"], infos["description"])))
                    dragSource = wx.DropSource(self)
                    dragSource.SetData(data)
                    dragSource.DoDragDrop()
        return OnLocationBeginDragFunction
    
    def RefreshTreeCtrlSize(self, treectrl):
        rect = self.GetTreeCtrlItemRect(treectrl, treectrl.GetRootItem())
        treectrl.SetMinSize(wx.Size(max(rect.width, rect.x + rect.width) + 20, max(rect.height, rect.y + rect.height)))
        self.PLCConfigMainSizer.Layout()
        self.PLCConfig.Refresh()
        wx.CallAfter(self.RefreshScrollBars)

    def GetTreeCtrlItemRect(self, treectrl, item):
        item_rect = treectrl.GetBoundingRect(item, True)
        if item_rect is not None:
            minx, miny = item_rect.x, item_rect.y
            maxx, maxy = item_rect.x + item_rect.width, item_rect.y + item_rect.height
        else:
            minx = miny = maxx = maxy = 0
        
        if treectrl.ItemHasChildren(item) and (item == treectrl.GetRootItem() or treectrl.IsExpanded(item)):
            if wx.VERSION >= (2, 6, 0):
                child, item_cookie = treectrl.GetFirstChild(item)
            else:
                child, item_cookie = treectrl.GetFirstChild(item, 0)
            while child.IsOk():
                child_rect = self.GetTreeCtrlItemRect(treectrl, child)
                minx = min(minx, child_rect.x)
                miny = min(miny, child_rect.y)
                maxx = max(maxx, child_rect.x + child_rect.width)
                maxy = max(maxy, child_rect.y + child_rect.height)
                child, item_cookie = treectrl.GetNextChild(item, item_cookie)
                
        return wx.Rect(minx, miny, maxx - minx, maxy - miny)
    
    def GenerateLocationExpandCollapseFunction(self, locations_infos, expand):
        def OnLocationExpandedFunction(event):
            item = event.GetItem()
            location_name = locations_infos["root"]["window"].GetPyData(item)
            if location_name is not None:
                locations_infos[location_name]["expanded"] = expand
                self.RefreshTreeCtrlSize(locations_infos["root"]["window"])
            event.Skip()
        return OnLocationExpandedFunction
    
    def GetButtonCallBackFunction(self, confnode, method):
        """ Generate the callbackfunc for a given confnode method"""
        def OnButtonClick(event):
            # Disable button to prevent re-entrant call 
            event.GetEventObject().Disable()
            # Call
            getattr(confnode,method)()
            # Re-enable button 
            event.GetEventObject().Enable()
            # Trigger refresh on Idle
            wx.CallAfter(self.RefreshAll)
            event.Skip()
        return OnButtonClick
    
    def ClearSizer(self, sizer):
        staticboxes = []
        for item in sizer.GetChildren():
            if item.IsSizer():
                item_sizer = item.GetSizer()
                self.ClearSizer(item_sizer)
                if isinstance(item_sizer, wx.StaticBoxSizer):
                    staticboxes.append(item_sizer.GetStaticBox())
        sizer.Clear(True)
        for staticbox in staticboxes:
            staticbox.Destroy()
                
    def RefreshAll(self):
        Beremiz.RefreshAll(self)
        self.RefreshPLCParams()
        self.RefreshConfNodeTree()
    
    # Remove taskbar icon when simulating
    def StartLocalRuntime(self, taskbaricon = True):
        return Beremiz.StartLocalRuntime(self, taskbaricon = False)

class StdoutPseudoFile:
    
    def __init__(self, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(('localhost', port))
        self.Buffer = ""
    
    def __del__(self):
        self.socket.close()
    
    def readline(self):
        idx = self.Buffer.find("\n")
        while idx == -1:
            text = self.socket.recv(2048)
            if text == "":
                return ""
            self.Buffer += text
            idx = self.Buffer.find("\n")
        if idx != -1:
            line = self.Buffer[:idx+1]
            self.Buffer = self.Buffer[idx+1:]
            if BMZ_DBG:
                print "command >"+line
            return line
        return ""
    
    """ Base class for file like objects to facilitate StdOut for the Shell."""
    def write(self, s, style = None):
        if s != '':
            self.socket.send(s.encode('utf8'))
        
    def writeyield(self, s):
        self.write(s)

    def write_warning(self, s):
        self.write(s)

    def write_error(self, s):
        self.write(s)

    def flush(self):
        pass
    
    def isatty(self):
        return False

if __name__ == '__main__':
    
    from threading import Thread, Timer, Semaphore
    import cmd

    wx_eval_lock = Semaphore(0)
    eval_res = None
    def wx_evaluator(callable, *args, **kwargs):
        global eval_res
        eval_res = None
        try:
            eval_res=callable(*args,**kwargs)
        finally:
            wx_eval_lock.release()

    def evaluator(callable, *args, **kwargs):
        global eval_res
        wx.CallAfter(wx_evaluator,callable,*args,**kwargs)
        wx_eval_lock.acquire()
        return eval_res

    # Command log for debug, for viewing from wxInspector
    if BMZ_DBG:
        __builtins__.cmdlog = []

    class LPCBeremiz_Cmd(cmd.Cmd):
        
        prompt = ""
        RefreshTimer = None
        
        def __init__(self, CTR, Log):
            cmd.Cmd.__init__(self, stdin=Log, stdout=Log)
            self.use_rawinput = False
            self.restore_last_state = False
            self.Log = Log
            self.CTR = CTR
            
        def RestartTimer(self):
            if self.RefreshTimer is not None:
                self.RefreshTimer.cancel()
            self.RefreshTimer = Timer(0.1, wx.CallAfter, args = [self.Refresh])
            self.RefreshTimer.start()
        
        def Exit(self):
            global frame, app
            self.Close()
            app.ExitMainLoop()
            return True
        
        def do_EOF(self, line):
            return self.Exit()
        
        def Show(self):
            global frame
            if frame is not None:
                self.CTR.SetAppFrame(frame, frame.Log)
                self.CTR.UpdateMethodsFromPLCStatus()
                frame.Show()
                frame.Raise()
                self.restore_last_state = True
            self.RestartTimer()
        
        def Refresh(self):
            global frame
            if frame is not None:
                if self.restore_last_state:
                    self.restore_last_state = False
                    frame.RestoreLastState()
                else:
                    frame.RefreshEditor()
                frame._Refresh(TITLE, PROJECTTREE, POUINSTANCEVARIABLESPANEL, FILEMENU, EDITMENU)
                frame.RefreshAll()
        
        def Close(self):
            global frame
            
            self.CTR.ResetAppFrame(self.Log)
            if frame is not None:
                frame.Hide()
        
        def Compile(self):
            self.CTR._Build()
        
        def SetProjectProperties(self, projectname, productname, productversion, companyname):
            new_properties = {
                "projectName": projectname,
                "productName": productname,
                "productVersion": productversion,
                "companyName": companyname}
            self.CTR.SetProjectProperties(properties=new_properties, buffer=False)
            self.RestartTimer()
        
        def SetOnlineMode(self, mode, path=None):
            self.CTR.SetOnlineMode(mode, path)
            self.RestartTimer()
        
        def AddBus(self, iec_channel, name, icon=None):
            for child in self.CTR.IterChildren():
                if child.BaseParams.getName() == name:
                    return "Error: A bus named %s already exists\n" % name
                elif child.BaseParams.getIEC_Channel() == iec_channel:
                    return "Error: A bus with IEC_channel %d already exists\n" % iec_channel
            bus = self.CTR.CTNAddChild(name, "LPCBus", iec_channel)
            if bus is None:
                return "Error: Unable to create bus\n"
            bus.SetIcon(icon)
            self.RestartTimer()
        
        def RenameBus(self, iec_channel, name):
            bus = self.CTR.GetChildByIECLocation((iec_channel,))
            if bus is None:
                return "Error: No bus found\n"
            for child in self.CTR.IterChildren():
                if child != bus and child.BaseParams.getName() == name:
                    return "Error: A bus named %s already exists\n" % name
            bus.BaseParams.setName(name)
            self.RestartTimer()
        
        def ChangeBusIECChannel(self, old_iec_channel, new_iec_channel):
            bus = self.CTR.GetChildByIECLocation((old_iec_channel,))
            if bus is None:
                return "Error: No bus found\n"
            for child in self.CTR.IterChildren():
                if child != bus and child.BaseParams.getIEC_Channel() == new_iec_channel:
                    return "Error: A bus with IEC_channel %d already exists\n" % new_iec_channel
            if wx.GetApp() is None:
                self.CTR.UpdateProjectVariableLocation(str(old_iec_channel), 
                                                              str(new_iec_channel))
            else:
                self.CTR.UpdateProjectVariableLocation(
                             str(old_iec_channel), 
                             str(new_iec_channel))
            bus.BaseParams.setIEC_Channel(new_iec_channel)
            self.RestartTimer()
        
        def RemoveBus(self, iec_channel):
            bus = self.CTR.GetChildByIECLocation((iec_channel,))
            if bus is None:
                return "Error: No bus found\n"
            self.CTR.RemoveProjectVariableByFilter(str(iec_channel))
            self.CTR.Children["LPCBus"].remove(bus)
            self.RestartTimer()
    
        def AddModule(self, parent, iec_channel, name, icode, icon=None):
            module = self.CTR.GetChildByIECLocation(parent)
            if module is None:
                return "Error: No parent found\n"
            for child in _GetModuleChildren(module):
                if child["name"] == name:
                    return "Error: A module named %s already exists\n" % name
                elif child["IEC_Channel"] == iec_channel:
                    return "Error: A module with IEC_channel %d already exists\n" % iec_channel 
            _GetLastModuleGroup(module).append({"name": name, 
                                                "type": LOCATION_MODULE, 
                                                "IEC_Channel": iec_channel, 
                                                "icon": icon, 
                                                "init": icode, 
                                                "children": []})
            self.RestartTimer()
    
        def RenameModule(self, iec_location, name):
            module = self.CTR.GetChildByIECLocation(iec_location)
            if module is None:
                return "Error: No module found\n"
            parent = self.CTR.GetChildByIECLocation(iec_location[:-1])
            if parent is self.CTR:
                return "Error: No module found\n"
            if module["name"] != name:
                for child in _GetModuleChildren(parent):
                    if child["name"] == name:
                        return "Error: A module named %s already exists\n" % name
                module["name"] = name
            self.RestartTimer()
    
        def ChangeModuleIECChannel(self, old_iec_location, new_iec_channel):
            module = self.CTR.GetChildByIECLocation(old_iec_location)
            if module is None:
                return "Error: No module found\n"
            parent = self.CTR.GetChildByIECLocation(old_iec_location[:-1])
            if parent is self.CTR:
                return "Error: No module found\n"
            if module["IEC_Channel"] != new_iec_channel:
                for child in _GetModuleChildren(parent):
                    if child["IEC_Channel"] == new_iec_channel:
                        return "Error: A module with IEC_channel %d already exists\n" % new_iec_channel
            self.CTR.UpdateProjectVariableLocation(".".join(map(str, old_iec_location)), ".".join(map(str, old_iec_location[:1] + (new_iec_channel,))))
            module["IEC_Channel"] = new_iec_channel
            self.RestartTimer()
        
        def ChangeModuleInitCode(self, iec_location, icode):
            module = self.CTR.GetChildByIECLocation(iec_location)
            if module is None:
                return "Error: No module found\n"
            module["init"] = icode
        
        def RemoveModule(self, parent, iec_channel):
            module = self.CTR.GetChildByIECLocation(parent)
            if module is None:
                return "Error: No parent found\n"
            child = _GetModuleBySomething(module, "IEC_Channel", (iec_channel,))
            if child is None:
                return "Error: No module found\n"
            self.CTR.RemoveProjectVariableByFilter(".".join(map(str, parent + (iec_channel,))))
            _RemoveModuleChild(module, child)
            self.RestartTimer()
        
        def StartGroup(self, parent, name, icon=None):
            module = self.CTR.GetChildByIECLocation(parent)
            if module is None:
                return "Error: No parent found\n"
            for child in module["children"]:
                if child["type"] == LOCATION_GROUP and child["name"] == name:
                    return "Error: A group named %s already exists\n" % name
            module["children"].append({"name": name, 
                                      "type": LOCATION_GROUP, 
                                      "icon": icon, 
                                      "children": []})
            self.RestartTimer()
    
        def AddVariable(self, parent, location, name, direction, type, rcode, pcode, description=""):
            module = self.CTR.GetChildByIECLocation(parent)
            if module is None:
                return "Error: No parent found\n"
            for child in _GetModuleChildren(module):
                if child["name"] == name:
                    return "Error: A variable named %s already exists\n" % name
                if child["location"] == location and child["type"] == LOCATION_TYPES[direction]:
                    return "Error: A variable with location %s already exists\n" % ".".join(map(str, location))
            _GetLastModuleGroup(module).append({"name": name, 
                                                "location": location, 
                                                "type": LOCATION_TYPES[direction], 
                                                "IEC_type": type, 
                                                "description": description, 
                                                "retrieve": rcode, 
                                                "publish": pcode})
            self.RestartTimer()

        def ChangeVariableParams(self, parent, location, new_name, new_direction, new_type, new_rcode, new_pcode, new_description=None):
            module = self.CTR.GetChildByIECLocation(parent)
            if module is None:
                return "Error: No parent found\n"
            variable = None
            for child in _GetModuleChildren(module):
                if child["location"] == location and child["type"] == LOCATION_TYPES[new_direction]:
                    variable = child
                elif child["name"] == new_name:
                    return "Error: A variable named %s already exists\n" % new_name
            if variable is None:
                return "Error: No variable found\n"
            if variable["name"] != new_name:
                self.CTR.UpdateProjectVariableName(variable["name"], new_name)
                variable["name"] = new_name
            variable["type"] = LOCATION_TYPES[new_direction]
            variable["IEC_type"] = new_type
            variable["retrieve"] = new_rcode
            variable["publish"] = new_pcode
            if new_description is not None:
                variable["description"] = new_description
            self.RestartTimer()
    
        def RemoveVariable(self, parent, location, direction):
            module = self.CTR.GetChildByIECLocation(parent)
            if module is None:
                return "Error: No parent found\n"
            child = _GetModuleVariable(module, location, direction)
            if child is None:
                return "Error: No variable found\n"
            size = LOCATION_SIZES[self.CTR.GetBaseType(child["IEC_type"])]
            address = "%" + LOCATION_DIRS[child["type"]] + size + ".".join(map(str, parent + location))
            self.CTR.RemoveProjectVariableByAddress(address)
            _RemoveModuleChild(module, child)
            self.RestartTimer()
        
    def location(loc):
        return tuple(map(int, loc.split(".")))
    
    def GetCmdFunction(function, arg_types, opt=0):
        arg_number = len(arg_types)
        def CmdFunction(self, line):
            args_toks = line.split('"')
            if len(args_toks) % 2 == 0:
                self.Log.write("Error: Invalid command\n")
                sys.stdout.flush()
                return
            args = []
            for num, arg in enumerate(args_toks):
                if num % 2 == 0:
                    stripped = arg.strip()
                    if stripped:
                        args.extend(stripped.split(" "))
                else:
                    args.append(arg)
            number = None
            extra = ""
            if opt == 0 and len(args) != arg_number:
                number = arg_number
            elif len(args) > arg_number:
                number = arg_number
                extra = " at most"
            elif len(args) < arg_number - opt:
                number = arg_number - opt
                extra = " at least"
            if number is not None:
                if number == 0:
                    self.Log.write("Error: No argument%s expected\n" % extra)
                elif number == 1:
                    self.Log.write("Error: 1 argument%s expected\n" % extra)
                else:
                    self.Log.write("Error: %d arguments%s expected\n" % (number, extra))
                sys.stdout.flush()
                return
            for num, arg in enumerate(args):
                try:
                    args[num] = arg_types[num](arg)
                except:
                    self.Log.write("Error: Invalid value for argument %d\n" % (num + 1))
                    sys.stdout.flush()
                    return

            func = getattr(self, function)
            res = evaluator(func,*args)

            if BMZ_DBG:
                cmdlog.append((function,line,res))
                if len(cmdlog) > 100: #prevent debug log to grow too much
                    cmdlog.pop(0) 

            if isinstance(res, (StringType, UnicodeType)):
                self.Log.write(res)
                return False
            else:
                return res
        return CmdFunction

    def CmdThreadProc(CTR, Log):
        global lpcberemiz_cmd
        for function, (arg_types, opt) in {"Exit": ([], 0),
                                           "Show": ([], 0),
                                           "Refresh": ([], 0),
                                           "Close": ([], 0),
                                           "Compile": ([], 0),
                                           "SetProjectProperties": ([str, str, str, str], 0),
                                           "SetOnlineMode": ([str, str], 1),
                                           "AddBus": ([int, str, str], 1),
                                           "RenameBus": ([int, str], 0),
                                           "ChangeBusIECChannel": ([int, int], 0),
                                           "RemoveBus": ([int], 0),
                                           "AddModule": ([location, int, str, str, str], 1), 
                                           "RenameModule": ([location, str], 0),
                                           "ChangeModuleIECChannel": ([location, int], 0),
                                           "ChangeModuleInitCode": ([location, str], 0),
                                           "RemoveModule": ([location, int], 0),
                                           "StartGroup": ([location, str, str], 1),
                                           "AddVariable": ([location, location, str, str, str, str, str, str], 1),
                                           "ChangeVariableParams": ([location, location, str, str, str, str, str, str], 1),
                                           "RemoveVariable": ([location, location], 0)}.iteritems():
            
            setattr(LPCBeremiz_Cmd, "do_%s" % function, GetCmdFunction(function, arg_types, opt))
        lpcberemiz_cmd = LPCBeremiz_Cmd(CTR, Log)
        lpcberemiz_cmd.cmdloop()

    Log = StdoutPseudoFile(port)

    if projectOpen is not None:
        projectOpen = DecodeFileSystemPath(projectOpen, False)
    
    CTR = LPCProjectController(None, Log, buildpath)
    if projectOpen is not None and os.path.isdir(projectOpen):
        result = CTR.LoadProject(projectOpen)
        if result:
            Log.write("Error: Invalid project directory", result)
    else:
        Log.write("Error: No such file or directory")
    
    cmd_thread=Thread(target=CmdThreadProc, args=[CTR, Log])
    cmd_thread.start()

    # Install a exception handle for bug reports
    AddExceptHook(os.getcwd(),__version__)
    
    frame = LPCBeremiz(None, ctr=CTR, debug=True)
    
    app.MainLoop()
    
