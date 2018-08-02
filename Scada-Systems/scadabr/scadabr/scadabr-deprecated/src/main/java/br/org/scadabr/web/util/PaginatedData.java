/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.util;

import java.util.List;

/**
 *
 * @author aploese
 */
@Deprecated
public class PaginatedData<T> {

    private final List<T> data;
    private final int rowCount;

    public PaginatedData(List<T> data) {
        this.data = data;
        this.rowCount = data.size();
    }

    public PaginatedData(List<T> data, int rowCount) {
        this.data = data;
        this.rowCount = rowCount;
    }

    public List<T> getData() {
        return data;
    }

    public int getRowCount() {
        return rowCount;
    }
}
