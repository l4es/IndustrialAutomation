/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.event.type;

/**
 * This enum defines all of the possible actions that can occur if an event is
 * raised for which type there already exists an active event.
 *
 * @author aploese
 */
public enum DuplicateHandling {

    /**
     * Duplicates are not allowed. This should be the case for all event types
     * where there is an automatic return to normal.
     */
    DO_NOT_ALLOW(1),
    /**
     * Duplicates are ignored. This should be the case where the initial
     * occurrence of an event is really the only thing of interest to a user.
     * For example, the initial error in a data source is usually what is most
     * useful in diagnosing a problem.
     */
    IGNORE(2),
    /**
     * Duplicates are ignored only if their message is the same as the existing.
     */
    IGNORE_SAME_MESSAGE(3),
    /**
     * Duplicates are allowed. The change detector uses this so that user's can
     * acknowledge every change the point experiences.
     */
    ALLOW(4);
    public final int mangoDbId;

    private DuplicateHandling(int mangoDbId) {
        this.mangoDbId = mangoDbId;
    }

    public static DuplicateHandling fromMangoDbId(int mangoDbId) {
        switch (mangoDbId) {
            case 1:
                return DuplicateHandling.DO_NOT_ALLOW;
            case 2:
                return DuplicateHandling.IGNORE;
            case 3:
                return DuplicateHandling.IGNORE_SAME_MESSAGE;
            case 4:
                return DuplicateHandling.ALLOW;
            default:
                throw new RuntimeException("Cant get LoggingTypes from mangoDbId: " + mangoDbId);
        }
    }

}