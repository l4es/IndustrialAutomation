/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.UserVO;
import br.org.scadabr.vo.event.AlarmLevel;
import java.time.LocalDate;
import br.org.scadabr.vo.EdgeConsumer;
import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import java.util.Objects;

/**
 *
 * @author aploese
 */
public class UserRT extends AbstractRT<UserVO> {

    public static String encryptPassword(String password) {
        try {
            final String alg = "SHA-256";
            MessageDigest md = MessageDigest.getInstance(alg);
            if (md == null) {
                throw new ShouldNeverHappenException(
                        "MessageDigest algorithm "
                        + alg
                        + " not found. Set the 'security.hashAlgorithm' property in env.properties appropriately. "
                        + "Use 'NONE' for no hashing.");
            }
            md.update(password.getBytes(Charset.forName("UTF8")));
            byte raw[] = md.digest();
            return Base64.getEncoder().encodeToString(raw);
        } catch (NoSuchAlgorithmException e) {
            // Should never happen, so just wrap in a runtime exception and
            // rethrow
            throw new ShouldNeverHappenException(e);
        }
    }

    private boolean admin;
    private String email;
    private boolean disabled;
    private String homeUrl;
    private LocalDate lastLogin;
    private String passwordHash;
    private String phone;
    private AlarmLevel receiveAlarmEmails;
    private boolean receiveOwnAuditEvents;

    private WatchListRT selectedWatchList;


    public UserRT(UserVO vo) {
        super(vo);
        patch(vo);
    }
    
    @Override
    public void patch(UserVO vo) {
        super.patch(vo);
        this.admin = vo.isAdmin();
        this.disabled = vo.isDisabled();
        this.email = vo.getEmail();
        this.homeUrl = vo.getHomeUrl();
        this.passwordHash = vo.getPasswordHash();
        this.phone = vo.getPhone();
        this.receiveAlarmEmails = vo.getReceiveAlarmEmails();
        this.receiveOwnAuditEvents = vo.isReceiveOwnAuditEvents();
    }
    
    @Override
    protected UserVO fillVO(UserVO vo) {
        super.fillVO(vo);
        vo.setAdmin(admin);
        vo.setDisabled(disabled);
        vo.setEmail(email);
        vo.setHomeUrl(homeUrl);
        vo.setPasswordHash(passwordHash);
        vo.setPhone(phone);
        vo.setReceiveAlarmEmails(receiveAlarmEmails);
        vo.setReceiveOwnAuditEvents(receiveOwnAuditEvents);
        return vo;
    }
    
    @Override
    public UserVO getVO() {
        return fillVO(new UserVO());
    }

    @Override
    public void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void wireEdgeAsDest(RT<?> src, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public NodeType getNodeType() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    public boolean isFirstLogin() {
        return lastLogin == null;
    }

    // Properties
    public boolean isAdmin() {
        return admin;
    }

    public String getEmail() {
        return email;
    }

    public String getPhone() {
        return phone;
    }

    public String getPasswordHash() {
        return passwordHash;
    }

    public boolean isDisabled() {
        return disabled;
    }

    public WatchListRT getSelectedWatchList() {
        return selectedWatchList;
    }

    public void setSelectedWatchList(WatchListRT selectedWatchList) {
        this.selectedWatchList = selectedWatchList;
    }

    public String getHomeUrl() {
        return homeUrl;
    }

    public LocalDate getLastLogin() {
        return lastLogin;
    }

    public void updateLastLogin(LocalDate lastLogin) {
        this.lastLogin = lastLogin;
    }

    public AlarmLevel getReceiveAlarmEmails() {
        return receiveAlarmEmails;
    }

    public boolean isReceiveOwnAuditEvents() {
        return receiveOwnAuditEvents;
    }

    public boolean isReceiveAlarmEmails() {
        return receiveAlarmEmails != null;
    }

    /**
     * Encrypts the password and compares it with the passwordHash
     * @param password
     * @return true if encrypted password and passwordHash are equal
     */
    public boolean comparePaswordWithHash(String password) {
        return Objects.equals(passwordHash, encryptPassword(password));
    }
    
    /**
     * Encrypts the password and set the passwordHash
     * @param password 
     */
    public void setPassword(String password) {
        passwordHash = encryptPassword(password);
    }
    
}