package br.org.scadabr.web;

import br.org.scadabr.ScadaBrConstants;

/**
 *
 * @author aploese
 */
@Deprecated
public abstract class LazyTreeNode {
    
    private Integer id;
    private Integer parentId;
    private String name;

    public LazyTreeNode() {
    }

    public LazyTreeNode(int parentId, String name) {
        this.parentId = parentId;
        this.name = name;
    }

    public boolean isNew() {
        return id == null;
    }
    
    /**
     * @return the id
     */
    public Integer getId() {
        return id;
    }

    /**
     * @param id the id to set
     */
    public void setId(Integer id) {
        this.id = id;
    }

    /**
     * @return the parentId
     */
    public Integer getParentId() {
        return parentId;
    }

    /**
     * @param parentId the parentId to set
     */
    public void setParentId(Integer parentId) {
        this.parentId = parentId;
    }

    /**
     * @return the Name
     */
    public String getName() {
        return name;
    }

    /**
     * @param Name the Name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * @return the nodeType
     */
    public abstract String getNodeType();

}
