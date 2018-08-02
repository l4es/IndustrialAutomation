/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.web.dwr.beans;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.l10n.AbstractLocalizer;
import java.util.LinkedList;
import java.util.List;
import java.util.ResourceBundle;

import com.serotonin.modbus4j.ModbusMaster;
import com.serotonin.modbus4j.NodeScanListener;
import com.serotonin.modbus4j.exception.ModbusInitException;
import br.org.scadabr.util.ProgressiveTask;
import br.org.scadabr.l10n.Localizer;

/**
 * @author Matthew Lohbihler
 *
 */
public class ModbusNodeScanListener implements NodeScanListener, TestingUtility {

    private final ResourceBundle bundle;
    private final ModbusMaster modbusMaster;
    private ProgressiveTask task;
    private final List<Integer> nodesFound = new LinkedList<>();
    private String message = "";

    public ModbusNodeScanListener(ResourceBundle bundle, ModbusMaster modbusMaster, boolean serial) {
        this.bundle = bundle;
        this.modbusMaster = modbusMaster;

        // Don't start the scan if we can't initialize the master.
        try {
            modbusMaster.init();
        } catch (ModbusInitException e) {
            if (serial) {
                message = AbstractLocalizer.localizeI18nKey("dsEdit.modbus.scannerSerial.startError", bundle, e.getMessage());
            } else {
                message = AbstractLocalizer.localizeI18nKey("dsEdit.modbus.scannerIp.startError", bundle, e.getMessage());
            }
            return;
        }

//TODO convert??        task = modbusMaster.scanForSlaveNodes(this);
        throw new ImplementMeException();
    }

    public List<Integer> getNodesFound() {
        return nodesFound;
    }

    public String getMessage() {
        return message;
    }

    public boolean isFinished() {
        return task == null;
    }

    @Override
    public synchronized void cancel() {
        if (task != null) {
            task.cancel();
            try {
                // Wait for the task to finish. The cleanup call below will notify when it gets called.
                wait();
            } catch (InterruptedException e) {
                // no op
            }
        }
    }

    private void cleanup() {
        modbusMaster.destroy();
        task = null;
        synchronized (this) {
            notifyAll();
        }
    }

    //
    // / NodeScanListener implementation
    //
    @Override
    public void progressUpdate(float progress) {
        message = AbstractLocalizer.localizeI18nKey("dsEdit.modbus.scanner.progress", bundle, Integer.toString((int) (progress * 100)));
    }

    @Override
    public synchronized void taskCancelled() {
        cleanup();
        message = AbstractLocalizer.localizeI18nKey("dsEdit.modbus.scanner.cancelled", bundle);
    }

    @Override
    public synchronized void taskCompleted() {
        cleanup();
        message = AbstractLocalizer.localizeI18nKey("dsEdit.modbus.scanner.complete", bundle);
    }

    @Override
    public void nodeFound(int nodeNumber) {
        nodesFound.add(nodeNumber);
    }
}
