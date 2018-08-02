/*
 *   Mango - Open Source M2M - http://mango.serotoninsoftware.com
 *   Copyright (C) 2010 Arne Pl\u00f6se
 *   @author Arne Pl\u00f6se
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.web.dwr.beans;

import br.org.scadabr.l10n.AbstractLocalizer;
import java.io.IOException;
import java.util.Map;
import java.util.ResourceBundle;

import net.sf.mbus4j.dataframes.MBusResponseFramesContainer;
import net.sf.mbus4j.master.MBusMaster;
import net.sf.mbus4j.master.MasterEventListener;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.l10n.Localizer;
import net.sf.mbus4j.Connection;
import br.org.scadabr.vo.datasource.mbus.MBusSearchByAddressing;
import br.org.scadabr.vo.datasource.mbus.PrimaryAddressingSearch;
import br.org.scadabr.vo.datasource.mbus.SecondaryAddressingSearch;

/**
 * @author aploese
 */
public class MBusDiscovery implements MasterEventListener, TestingUtility {

    private final MBusSearchByAddressing searchByAddressing;

    public void start() {
        searchThread.start();
    }

    public MBusResponseFramesContainer getDevice(int deviceIndex) {
        return master.getDevice(deviceIndex);
    }

    class SearchThread extends Thread {

        @Override
        public void run() {
            LOG.info("start search");
            try {
                master.open();
                if (searchByAddressing instanceof PrimaryAddressingSearch) {
                    final PrimaryAddressingSearch pas = (PrimaryAddressingSearch) searchByAddressing;
                    master.searchDevicesByPrimaryAddress(pas.firstAddr(), pas.lastAddr());
                } else if (searchByAddressing instanceof SecondaryAddressingSearch) {
                    final SecondaryAddressingSearch sasd = (SecondaryAddressingSearch) searchByAddressing;
                    master.widcardSearch(sasd.maskedId(), sasd.maskedManufacturer(), sasd.maskedVersion(), sasd.maskedMedium(), sasd.getMaxTries());
                } else {
                }
            } catch (InterruptedException ex) {
                LOG.info("Interrupted)");
            } catch (IOException ex) {
                LOG.warn("SearchThread.run", ex);
            } catch (Exception ex) {
                LOG.warn("SearchThread.run", ex);
            } finally {
                finished = true;
                LOG.info("Search finished!");
                try {
                    master.close();
                } catch (IOException ex) {
                    LOG.error("IO Ex)");
                }
            }
        }
    }
    static final Log LOG = LogFactory.getLog(MBusDiscovery.class);
    final ResourceBundle bundle;
    final MBusMaster master;
    private final AutoShutOff autoShutOff;
    String message;
    boolean finished;
    private final SearchThread searchThread;

    public MBusDiscovery(ResourceBundle bundle, Connection connection, MBusSearchByAddressing searchByAddressing) {
        LOG.info("MBusDiscovery(...)");
        this.bundle = bundle;

        autoShutOff = new AutoShutOff(AutoShutOff.DEFAULT_TIMEOUT * 4) {

            @Override
            void shutOff() {
                message = AbstractLocalizer.localizeI18nKey("dsEdit.mbus.tester.autoShutOff", MBusDiscovery.this.bundle);
                MBusDiscovery.this.cleanup();
            }
        };

        // Thread starten , der sucht....
        master = new MBusMaster();
        master.setConnection(connection);
        this.searchByAddressing = searchByAddressing;
        message = AbstractLocalizer.localizeI18nKey("dsEdit.mbus.tester.searchingDevices", bundle);
        searchThread = new SearchThread();
    }

    public void addUpdateInfo(Map<String, Object> result) {
        LOG.info("addUpdateInfo()");
        autoShutOff.update();

        MBusDeviceBean[] devs = new MBusDeviceBean[master.deviceCount()];
        for (int i = 0; i < devs.length; i++) {
            MBusResponseFramesContainer dev = master.getDevice(i);
            devs[i] = new MBusDeviceBean(i, dev);
        }

        result.put("devices", devs);
        result.put("message", message);
        result.put("finished", finished);
    }

    @Override
    public void cancel() {
        LOG.info("cancel()");
        message = AbstractLocalizer.localizeI18nKey("dsEdit.mbus.tester.cancelled", bundle);
        cleanup();
    }

    void cleanup() {
        LOG.info("cleanup()");
        if (!finished) {
            finished = true;
            master.cancel();
            autoShutOff.cancel();
            searchThread.interrupt();
        }
    }

    public void getDeviceDetails(int deviceIndex, Map<String, Object> result) {
        LOG.info("getDeviceDetails()");
        MBusResponseFramesContainer dev = master.getDevice(deviceIndex);
        result.put("deviceName", String.format("%s %s 0x%02X %08d @0x%02X", dev.getManufacturer(), dev.getMedium(),
                dev.getVersion(), dev.getIdentNumber(), dev.getAddress()));

        result.put("deviceIndex", deviceIndex);

        MBusResponseFrameBean[] responseFrames = new MBusResponseFrameBean[dev.getResponseFrameContainerCount()];
        for (int i = 0; i < dev.getResponseFrameContainerCount(); i++) {
            responseFrames[i] = new MBusResponseFrameBean(dev.getResponseFrameContainer(i).getResponseFrame(),
                    deviceIndex, i, dev.getResponseFrameContainer(i).getName());
        }
        result.put("responseFrames", responseFrames);
    }
}
