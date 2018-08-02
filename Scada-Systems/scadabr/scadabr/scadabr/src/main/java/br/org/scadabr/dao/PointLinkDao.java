/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import com.serotonin.mango.vo.link.PointLinkVO;

/**
 *
 * @author aploese
 */
public interface PointLinkDao {

    public void deletePointLink(int pointLinkId);

    public void savePointLink(PointLinkVO vo);

    public Iterable<PointLinkVO> getPointLinks();

    public Iterable<PointLinkVO> getPointLinksForPoint(int dataPointId);
    
}
