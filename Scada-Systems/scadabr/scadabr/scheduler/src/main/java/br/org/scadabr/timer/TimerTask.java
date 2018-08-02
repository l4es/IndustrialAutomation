/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer;

import br.org.scadabr.utils.ImplementMeException;

/**
 *
 * @author aploese
 */
@Deprecated
public abstract class TimerTask implements Runnable {

    public TimerTask(TimerTrigger trigger) {
        throw new ImplementMeException();
    }

    public void cancel() {
        throw new ImplementMeException();
    }

    protected abstract void run(long fireTime);

    @Override
    public void run() {
        throw new ImplementMeException();
    }
}
