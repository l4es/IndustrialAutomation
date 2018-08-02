/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr;

/**
 *
 * @author aploese
 */
public class ShouldNeverHappenException extends RuntimeException {

    public ShouldNeverHappenException(String message) {
        super(message);
    }

    public ShouldNeverHappenException(String message, Throwable cause) {
        super(message, cause);
    }

    public ShouldNeverHappenException(Throwable cause) {
        super(cause);
    }

}
