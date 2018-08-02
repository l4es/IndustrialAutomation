/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datapoints;

import br.org.scadabr.rt.PointFolderRT;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.AbstractVO;
import br.org.scadabr.vo.NodeType;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonTypeName;
import java.util.List;


/**
 * @author Matthew Lohbihler
 *
 */
@JsonTypeName("POINT_FOLDER")
public class PointFolderVO extends AbstractVO<PointFolderVO> implements DataPointNodeVO<PointFolderVO> {

    public PointFolderVO() {
        // no op
    }

    @Override
    public PointFolderRT createRT() {
        return new PointFolderRT(this);
    }

    @JsonIgnore
    @Override
    public String getTypeKey() {
        return "event.audit.pointFolder";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, PointFolderVO from) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public NodeType getNodeType() {
        return NodeType.POINT_FOLDER;
    }

}
