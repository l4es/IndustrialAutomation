/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.vmstat;

import br.org.scadabr.web.i18n.LocalizableI18nKey;

/**
 *
 * @author aploese
 */
public enum Attribute implements LocalizableI18nKey {

    PROCS_R("dsEdit.vmstat.attr.procsR"),
    PROCS_B("dsEdit.vmstat.attr.procsB"),
    MEMORY_SWPD("dsEdit.vmstat.attr.memorySwpd"),
    MEMORY_FREE("dsEdit.vmstat.attr.memoryFree"),
    MEMORY_BUFF("dsEdit.vmstat.attr.memoryBuff"),
    MEMORY_CACHE("dsEdit.vmstat.attr.memoryCache"),
    SWAP_SI("dsEdit.vmstat.attr.swapSi"),
    SWAP_SO("dsEdit.vmstat.attr.swapSo"),
    IO_BI("dsEdit.vmstat.attr.ioBi"),
    IO_BO("dsEdit.vmstat.attr.ioBo"),
    SYSTEM_IN("dsEdit.vmstat.attr.systemIn"),
    SYSTEM_CS("dsEdit.vmstat.attr.systemCs"),
    CPU_US("dsEdit.vmstat.attr.cpuUs"),
    CPU_SY("dsEdit.vmstat.attr.cpuSy"),
    CPU_ID("dsEdit.vmstat.attr.cpuId"),
    CPU_WA("dsEdit.vmstat.attr.cpuWa"),
    CPU_ST("dsEdit.vmstat.attr.cpuSt");

    public final String i18nKey;

    private Attribute(String i18nKey) {
        this.i18nKey = i18nKey;
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    public String getName() {
        return name();
    }

}
