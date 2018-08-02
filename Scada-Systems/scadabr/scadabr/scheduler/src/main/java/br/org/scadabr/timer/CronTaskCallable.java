/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer;

import java.util.concurrent.Callable;

/**
 *
 * @author aploese
 * @param <V>
 */
public interface CronTaskCallable<V> extends Callable<V>{
    
}
