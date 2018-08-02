/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.util;

import br.org.scadabr.utils.ImplementMeException;

/**
 *
 * @author aploese
 */
public abstract class ProgressiveTask implements Runnable {

    protected boolean completed;

    protected abstract void runImpl();

    @Override
    public void run() {
        throw new ImplementMeException();
    }

    public boolean isCancelled() {
        throw new ImplementMeException();
    }

    public boolean isCompleted() {
        throw new ImplementMeException();
    }

    public void cancel() {
        throw new ImplementMeException();
    }

}
