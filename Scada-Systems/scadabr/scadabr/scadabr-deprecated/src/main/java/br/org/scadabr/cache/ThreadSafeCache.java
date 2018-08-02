/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.cache;

import br.org.scadabr.utils.ImplementMeException;
import java.text.SimpleDateFormat;

/**
 *
 * @author aploese
 * @param <T>
 */
public class ThreadSafeCache<T extends Object> {

    public ThreadSafeCache(ObjectCreator<SimpleDateFormat> objectCreator) {
        throw new ImplementMeException();
    }

    public T getObject() {
        throw new ImplementMeException();
    }

}
