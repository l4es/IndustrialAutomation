/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.util;

import java.util.Date;
import java.util.GregorianCalendar;
import java.util.List;

/**
 *
 * @author aploese
 */
@Deprecated
public class PagingDataForm<P, T extends PaginatedData<P>> {

    private List<T> data;
    private int page;
    private int numberOfPages;
    private int numberOfItems;
    private int offset;
    private int itemsPerPage = 25;
    private String sortField;
    private boolean sortDesc;
    private int startMonth;
    private int startDay;
    private int startYear;
    private int endMonth;
    private int endDay;
    private int endYear;

    public String getOrderByClause() {
        if (sortField == null) {
            return "";
        }
        return "order by " + sortField + (sortDesc ? " desc" : "");
    }

    public void setData(List<T> data) {
        this.data = data;
        // Set default
        numberOfItems = data.size();
    }

    public void setData(List<T> data, int numberOfItems) {
        this.data = data;
        // Set default
        this.numberOfItems = numberOfItems;
    }

    public void setNumberOfItems(int numberOfItems) {
        this.numberOfItems = numberOfItems;
        
        numberOfPages = ((numberOfItems - 1) / itemsPerPage + 1);
        if (page >= numberOfPages) {
            page = (numberOfPages - 1);
        }
        offset = (page * itemsPerPage);
    }

    public int getEndIndex() {
        int end = offset + itemsPerPage - 1;
        if (end >= numberOfItems) {
            end = numberOfItems - 1;
        }
        if (end < offset) {
            end = offset;
        }
        return end;
    }

    public Date getRunTime() {
        return new Date(System.currentTimeMillis());
    }

    public Date getStartDate() {
        Date date = null;
        if (startMonth > 0) {
            GregorianCalendar gc = new GregorianCalendar(startYear, startMonth - 1, startDay);
            gc.setLenient(true);
            date = new Date(gc.getTimeInMillis());

            startYear = gc.get(1);
            startMonth = (gc.get(2) + 1);
            startDay = gc.get(5);
        }
        return date;
    }

    public Date getEndDate(boolean increment) {
        Date date = null;
        if (endMonth > 0) {
            GregorianCalendar gc = new GregorianCalendar(endYear, endMonth - 1, endDay);
            gc.setLenient(true);

            endYear = gc.get(1);
            endMonth = (gc.get(2) + 1);
            endDay = gc.get(5);
            if (increment) {
                gc.add(5, 1);
            }
            date = new Date(gc.getTimeInMillis());
        }
        return date;
    }

    public int getItemsPerPage() {
        return itemsPerPage;
    }

    public void setItemsPerPage(int itemsPerPage) {
        this.itemsPerPage = itemsPerPage;
    }

    public int getNumberOfPages() {
        return numberOfPages;
    }

    public int getNumberOfItems() {
        return numberOfItems;
    }

    public int getPage() {
        return page;
    }

    public void setPage(int page) {
        this.page = page;
    }

    public int getOffset() {
        return offset;
    }

    public String getSortField() {
        return sortField;
    }

    public void setSortField(String sortField) {
        this.sortField = sortField;
    }

    public boolean getSortDesc() {
        return sortDesc;
    }

    public void setSortDesc(boolean sortDesc) {
        this.sortDesc = sortDesc;
    }

    public List<T> getData() {
        return data;
    }

    public int getEndDay() {
        return endDay;
    }

    public void setEndDay(int endDay) {
        this.endDay = endDay;
    }

    public int getEndMonth() {
        return endMonth;
    }

    public void setEndMonth(int endMonth) {
        this.endMonth = endMonth;
    }

    public int getEndYear() {
        return endYear;
    }

    public void setEndYear(int endYear) {
        this.endYear = endYear;
    }

    public int getStartDay() {
        return startDay;
    }

    public void setStartDay(int startDay) {
        this.startDay = startDay;
    }

    public int getStartMonth() {
        return startMonth;
    }

    public void setStartMonth(int startMonth) {
        this.startMonth = startMonth;
    }

    public int getStartYear() {
        return startYear;
    }

    public void setStartYear(int startYear) {
        this.startYear = startYear;
    }

}
