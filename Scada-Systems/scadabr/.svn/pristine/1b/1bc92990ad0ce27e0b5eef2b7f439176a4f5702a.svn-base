/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.util;

import freemarker.ext.beans.MapModel;
import java.util.Map;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.springframework.validation.BindException;
import org.springframework.web.servlet.ModelAndView;

/**
 *
 * @author aploese
 */
@Deprecated
public abstract class PaginatedListController {

    private String viewName;

    public void setViewName(String viewName) {
        this.viewName = viewName;
    }

  //  @Override
    protected ModelAndView handle(MapModel mapModel, HttpServletRequest request, HttpServletResponse response, BindException errors)
            throws Exception {
        PagingDataForm paging = getPagingDataForm();

        PaginatedData data = getData(request, paging, paging.getOrderByClause(), paging.getOffset(), paging.getItemsPerPage(), errors);

        paging.setData(data.getData());
        paging.setNumberOfItems(data.getRowCount());

        Map model = errors.getModel();
//        model.put(getCommandName(), command);

        return new ModelAndView(viewName, model);
    }

    protected abstract PaginatedData getData(HttpServletRequest request, PagingDataForm paging, String orderByClause, int offset, int limit, BindException errors) throws Exception;
    
    protected abstract PagingDataForm getPagingDataForm();
    
}
