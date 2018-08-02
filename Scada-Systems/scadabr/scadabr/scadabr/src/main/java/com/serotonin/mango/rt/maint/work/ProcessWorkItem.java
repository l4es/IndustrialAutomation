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
package com.serotonin.mango.rt.maint.work;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringWriter;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.io.StreamUtils;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.SystemRunnable;
import com.serotonin.mango.rt.event.type.SystemEventType;
import br.org.scadabr.vo.event.type.SystemEventKey;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class ProcessWorkItem implements SystemRunnable {

    @Autowired
    private SchedulerPool schedulerPool; 
    
    static final Log LOG = LogFactory.getLog(ProcessWorkItem.class);
    private static final int TIMEOUT = 15000; // 15 seconds

    public void queue() {
        schedulerPool.execute(this);
    }

    final String command;

    public ProcessWorkItem(String command) {
        this.command = command;
    }

    @Override
    public void run() {
        try {
            executeProcessCommand(command);
        } catch (IOException e) {
            new SystemEventType(SystemEventKey.PROCESS_FAILURE).fire("event.process.failure", command, e.getMessage());
        }
    }

    public void executeProcessCommand(String command) throws IOException {

        Process process = Runtime.getRuntime().exec(command);

        InputReader out = new InputReader(process.getInputStream());
        InputReader err = new InputReader(process.getErrorStream());

        schedulerPool.execute(out);
        schedulerPool.execute(err);

        try {
            ProcessTimeout timeout = new ProcessTimeout(process, command);
            schedulerPool.execute(timeout);

            process.waitFor();
            out.join();
            err.join();
            process.destroy();

            // If we've made it this far, the process exited properly, so kill the timeout thread if it exists.
            timeout.interrupt();

            String input = out.getInput();
            if (!input.isEmpty()) {
                LOG.info("Process output: '" + input + "'");
            }

            input = err.getInput();
            if (!input.isEmpty()) {
                LOG.warn("Process error: '" + input + "'");
            }
        } catch (InterruptedException e) {
            throw new IOException("Timeout while running command: '" + command + "'");
        }
    }
/*
    @Override
    public int getPriority() {
        return WorkItem.PRIORITY_HIGH;
    }
*/
    static class ProcessTimeout implements SystemRunnable {

        private final Process process;
        private final String command;
        private volatile boolean interrupted;

        ProcessTimeout(Process process, String command) {
            this.process = process;
            this.command = command;
        }

        /*
        @Override
        public int getPriority() {
            return WorkItem.PRIORITY_HIGH;
        }
*/
        public void interrupt() {
            synchronized (this) {
                interrupted = true;
                notifyAll();
            }
        }

        @Override
        public void run() {
            try {
                synchronized (this) {
                    wait(TIMEOUT);
                }

                if (!interrupted) {
                    // If the sleep time has expired, destroy the process.
                    LOG.warn("Timeout waiting for process to end. command=" + command);
                    process.destroy();
                }
            } catch (InterruptedException e) { /* no op */

            }
        }
    }

    static class InputReader implements SystemRunnable {

        private final InputStreamReader reader;
        private final StringWriter writer = new StringWriter();
        private boolean done;

        InputReader(InputStream is) {
            reader = new InputStreamReader(is);
        }

        public String getInput() {
            return writer.toString();
        }

        public void join() {
            synchronized (this) {
                if (!done) {
                    try {
                        wait();
                    } catch (InterruptedException e) {
                        // no op
                    }
                }
            }
        }
/*
        @Override
        public int getPriority() {
            return WorkItem.PRIORITY_HIGH;
        }
*/
        @Override
        public void run() {
            try {
                StreamUtils.transfer(reader, writer);
            } catch (IOException e) {
                LOG.error("Error in process input reader", e);
            } finally {
                synchronized (this) {
                    done = true;
                    notifyAll();
                }
            }
        }
    }
    //    
    // public static void main(String[] args) throws Exception {
    // // ServletContext ctx = new DummyServletContext();
    // BackgroundProcessing bp = new BackgroundProcessing();
    // bp.initialize();
    // // ctx.setAttribute(Common.ContextKeys.BACKGROUND_PROCESSING, bp);
    // // Common.ctx = new ContextWrapper(ctx);
    // // ProcessWorkItem.queueProcess("");
    // // bp.terminate();
    //        
    // // //ProcessBuilder pb = new ProcessBuilder("cmd /c dir");
    // // ProcessBuilder pb = new ProcessBuilder("cmd", "/c", "dir");
    // // pb.redirectErrorStream(true);
    // // Process process = pb.start();
    // Process process = Runtime.getRuntime().exec("cmd /c java -version");
    //        
    // InputReader out = new InputReader(process.getInputStream());
    // InputReader err = new InputReader(process.getErrorStream());
    //        
    // bp.addWorkItem(out);
    // bp.addWorkItem(err);
    //        
    // process.waitFor();
    // out.join();
    // err.join();
    // process.destroy();
    // bp.terminate();
    //        
    // System.out.println("out: "+ out.getInput());
    // System.out.println("err: "+ err.getInput());
    // }
}
