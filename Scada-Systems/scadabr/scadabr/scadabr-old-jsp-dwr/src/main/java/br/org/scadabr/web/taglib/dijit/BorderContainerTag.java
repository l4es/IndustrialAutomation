/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dijit;

import br.org.scadabr.web.taglib.DojoTag;

/**
 *
 * @author aploese
 */
public class BorderContainerTag extends DojoTag {

    protected BorderContainerTag(String design) {
        super("div", "dijit/layout/BorderContainer");
        putDataDojoProp("design", design);
    }

    public void setGutters(boolean gutters) {
        putDataDojoProp("gutters", gutters);
    }
    
    public void setLiveSplitters(boolean liveSplitters) {
        putDataDojoProp("liveSplitters", liveSplitters);
    }


}
