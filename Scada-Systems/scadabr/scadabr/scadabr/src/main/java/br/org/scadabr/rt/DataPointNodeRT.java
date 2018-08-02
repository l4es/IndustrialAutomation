/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.vo.datapoints.DataPointNodeVO;

/**
 *
 * @author aploese
 * @param <T>
 */
public interface DataPointNodeRT<T extends DataPointNodeVO<T>> extends RT<T>{
    
    void setParent(PointFolderRT parent);
    
    PointFolderRT getParent();

}
