/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.monitor;

import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author aploese
 */
@Deprecated //Any Use??
public class MonitoredValues {
   
    private final Map<String, IntegerMonitor> monitors = new HashMap<>();
  
  public void addIfMissingStatMonitor(IntegerMonitor monitor)
  {
      monitors.put(monitor.getId(), monitor);
  }
  
  public IntegerMonitor getValueMonitor(String id)
  {
      return monitors.get(id);
  }

}
