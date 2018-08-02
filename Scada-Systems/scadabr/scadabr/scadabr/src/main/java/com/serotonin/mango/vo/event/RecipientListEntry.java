/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.vo.event;

import java.io.Serializable;

import br.org.scadabr.ShouldNeverHappenException;
import com.serotonin.mango.vo.mailingList.AddressEntry;
import com.serotonin.mango.vo.mailingList.EmailRecipient;
import com.serotonin.mango.vo.mailingList.MailingList;
import com.serotonin.mango.vo.mailingList.UserEntry;


public class RecipientListEntry implements Serializable {

    private static final long serialVersionUID = -1;

    private int recipientType;
    @Deprecated //TODO use RT here ...
    private int referenceId;
    private String referenceAddress;

    public EmailRecipient createEmailRecipient() {
        switch (recipientType) {
            case EmailRecipient.TYPE_MAILING_LIST:
                MailingList ml = new MailingList();
                ml.setId(referenceId);
                return ml;
            case EmailRecipient.TYPE_USER:
                UserEntry u = new UserEntry();
                u.setUserId(referenceId);
                return u;
            case EmailRecipient.TYPE_ADDRESS:
                AddressEntry a = new AddressEntry();
                a.setAddress(referenceAddress);
                return a;
        }
        throw new ShouldNeverHappenException("Unknown email recipient type: " + recipientType);
    }

    public String getReferenceAddress() {
        return referenceAddress;
    }

    public void setReferenceAddress(String address) {
        referenceAddress = address;
    }

    public int getRecipientType() {
        return recipientType;
    }

    public void setRecipientType(int typeId) {
        recipientType = typeId;
    }

    @Deprecated
    public int getReferenceId() {
        return referenceId;
    }

    @Deprecated
    public void setReferenceId(int refId) {
        referenceId = refId;
    }

}
