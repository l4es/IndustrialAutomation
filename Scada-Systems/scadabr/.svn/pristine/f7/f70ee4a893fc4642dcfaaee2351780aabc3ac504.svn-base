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
package br.org.scadabr.rt.datasource.vmstat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.rt.dataSource.EventDataSource;
import br.org.scadabr.vo.datasource.vmstat.VMStatDataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.vmstat.Attribute;
import br.org.scadabr.vo.datasource.vmstat.VMStatPointLocatorVO;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import java.util.EnumMap;

/**
 * @author Matthew Lohbihler
 */
public class VMStatDataSourceRT extends EventDataSource<VMStatDataSourceVO> implements Runnable {

    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int PARSE_EXCEPTION_EVENT = 2;

    private final Log log = LogFactory.getLog(VMStatDataSourceRT.class);
    private Process vmstatProcess;
    private BufferedReader in;
    private Map<Attribute, Integer> attributePositions;
    private boolean terminated;

    public VMStatDataSourceRT(VMStatDataSourceVO vo) {
        super(vo, true);
    }

    //
    // /
    // / Lifecycle
    // /
    //
    @Override
    public void initialize() {
        super.initialize();

        String command = "vmstat -n ";
        switch (vo.getOutputScale()) {
            case LOWER_K:
                command += "-S k ";
                break;
            case UPPER_K:
                command += "-S K ";
                break;
            case LOWER_M:
                command += "-S m ";
                break;
            case UPPER_M:
                command += "-S M ";
                break;
        }

        command += vo.getPollSeconds();

        try {
            vmstatProcess = Runtime.getRuntime().exec(command, new String[]{"LANG=C"}); //LANG=DE will have Frei instead of free...

            // Create the input stream readers.
            in = new BufferedReader(new InputStreamReader(vmstatProcess.getInputStream()));

            // Read the first two lines of output. They are the headers.
            in.readLine();
            String headers = in.readLine();

            // Create a mapping of attribute ids to split array positions.
            attributePositions = new EnumMap(Attribute.class);
            String[] headerParts = headers.split("\\s+");
            for (int i = 0; i < headerParts.length; i++) {
                if (null != headerParts[i]) {
                    switch (headerParts[i]) {
                        case "r":
                            attributePositions.put(Attribute.PROCS_R, i);
                            break;
                        case "b":
                            attributePositions.put(Attribute.PROCS_B, i);
                            break;
                        case "swpd":
                            attributePositions.put(Attribute.MEMORY_SWPD, i);
                            break;
                        case "free":
                            attributePositions.put(Attribute.MEMORY_FREE, i);
                            break;
                        case "buff":
                            attributePositions.put(Attribute.MEMORY_BUFF, i);
                            break;
                        case "cache":
                            attributePositions.put(Attribute.MEMORY_CACHE, i);
                            break;
                        case "si":
                            attributePositions.put(Attribute.SWAP_SI, i);
                            break;
                        case "so":
                            attributePositions.put(Attribute.SWAP_SO, i);
                            break;
                        case "bi":
                            attributePositions.put(Attribute.IO_BI, i);
                            break;
                        case "bo":
                            attributePositions.put(Attribute.IO_BO, i);
                            break;
                        case "in":
                            attributePositions.put(Attribute.SYSTEM_IN, i);
                            break;
                        case "cs":
                            attributePositions.put(Attribute.SYSTEM_CS, i);
                            break;
                        case "us":
                            attributePositions.put(Attribute.CPU_US, i);
                            break;
                        case "sy":
                            attributePositions.put(Attribute.CPU_SY, i);
                            break;
                        case "id":
                            attributePositions.put(Attribute.CPU_ID, i);
                            break;
                        case "wa":
                            attributePositions.put(Attribute.CPU_WA, i);
                            break;
                        case "st":
                            attributePositions.put(Attribute.CPU_ST, i);
                            break;
                    }
                }

            }

            // Read the first line of data. This is a summary of beginning of time until now, so it is no good for
            // our purposes. Just throw it away.
            in.readLine();

            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (IOException e) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "event.initializationError", e.getMessage()));
        }
    }

    @Override
    public void terminate() {
        super.terminate();

        terminated = true;

        // Stop the process.
        if (vmstatProcess != null) {
            vmstatProcess.destroy();
        }
    }

    @Override
    public void beginPolling() {
        if (vmstatProcess != null) {
            new Thread(this, "VMStat data source").start();
        }
    }

    @Override
    public void run() {
        try {
            while (true) {
                String line = in.readLine();

                if (line == null) {
                    if (terminated) {
                        break;
                    }
                    throw new IOException("no data");
                }

                readParts(line.split("\\s+"));
                readError();
            }
        } catch (IOException e) {
            // Assume that the process was ended.
            readError();

            if (!terminated) {
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "event.vmstat.process", e.getMessage()));
            }
        }
    }

    private void readParts(String[] parts) {
        LocalizableMessage error = null;
        long time = System.currentTimeMillis();
        updateChangedPoints();

        for (DataPointRT dp : enabledDataPoints.values()) {
            VMStatPointLocatorVO locator = ((VMStatPointLocatorRT) dp.getPointLocator()).getVo();

            Integer position = attributePositions.get(locator.getAttribute());
            if (position == null) {
                if (error != null) {
                    error = new LocalizableMessageImpl("event.vmstat.attributeNotFound", locator
                            .getConfigurationDescription());
                }
            } else {
                try {
                    String data = parts[position];
                    Double value = new Double(data);
                    dp.updatePointValue(new PointValueTime(value, time));
                } catch (NumberFormatException e) {
                    log.error("Weird. We couldn't parse the value " + parts[position]
                            + " into a double. attribute=" + locator.getAttribute());
                } catch (ArrayIndexOutOfBoundsException e) {
                    log.error("Weird. We need element " + position + " but the vmstat data is only " + parts.length
                            + " elements long");
                }
            }
        }

        if (error == null) {
            returnToNormal(PARSE_EXCEPTION_EVENT, time);
        } else {
            raiseEvent(PARSE_EXCEPTION_EVENT, time, true, error);
        }

    }

    private void readError() {
        Process p = vmstatProcess;
        if (p != null) {
            try {
                if (p.getErrorStream().available() > 0) {
                    StringBuilder errorMessage = new StringBuilder();
                    InputStreamReader err = new InputStreamReader(p.getErrorStream());
                    char[] buf = new char[1024];
                    int read;

                    while (p.getErrorStream().available() > 0) {
                        read = err.read(buf);
                        if (read == -1) {
                            break;
                        }
                        errorMessage.append(buf, 0, read);
                    }

                    if (!terminated) {
                        log.warn("Error message from vmstat process: " + errorMessage);
                    }
                }
            } catch (IOException e) {
                log.warn("Exception while reading error stream", e);
            }
        }
    }
}
