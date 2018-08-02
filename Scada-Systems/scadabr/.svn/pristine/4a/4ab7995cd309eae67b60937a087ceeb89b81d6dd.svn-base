/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

import java.util.Objects;

/**
 *
 * @author aploese
 */
public class Edge {
    private Integer srcNodeId;
    private Integer destNodeId;
    private EdgeType edgeType;

    
    public Edge(VO srcNode, VO destNode, EdgeType edgeType) {
        this.srcNodeId = srcNode.getId();
        this.destNodeId = destNode.getId();
        this.edgeType = edgeType;
    }

    public Edge(Integer srcNodeId, Integer destNodeId, EdgeType edgeType) {
        this.srcNodeId = srcNodeId;
        this.destNodeId = destNodeId;
        this.edgeType = edgeType;
    }

    /**
     * @return the edgeType
     */
    public EdgeType getEdgeType() {
        return edgeType;
    }

    /**
     * @param edgeType the edgeType to set
     */
    public void setEdgeType(EdgeType edgeType) {
        this.edgeType = edgeType;
    }

    /**
     * @return the srcNodeId
     */
    public Integer getSrcNodeId() {
        return srcNodeId;
    }

    /**
     * @param srcNodeId the srcNodeId to set
     */
    public void setSrcNodeId(Integer srcNodeId) {
        this.srcNodeId = srcNodeId;
    }

    /**
     * @return the destNodeId
     */
    public Integer getDestNodeId() {
        return destNodeId;
    }

    /**
     * @param destNodeId the destNodeId to set
     */
    public void setDestNodeId(Integer destNodeId) {
        this.destNodeId = destNodeId;
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 53 * hash + Objects.hashCode(this.srcNodeId);
        hash = 53 * hash + Objects.hashCode(this.destNodeId);
        hash = 53 * hash + Objects.hashCode(this.edgeType);
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Edge other = (Edge) obj;
        if (!Objects.equals(this.srcNodeId, other.srcNodeId)) {
            return false;
        }
        if (!Objects.equals(this.destNodeId, other.destNodeId)) {
            return false;
        }
        if (this.edgeType != other.edgeType) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "Edge{" + "srcNodeId=" + srcNodeId + ", destNodeId=" + destNodeId + ", edgeType=" + edgeType + '}';
    }
    
}
