package com.serotonin.mango.rt.event.type;


import br.org.scadabr.dao.MaintenanceEventDao;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.MaintenanceEventKey;
import com.serotonin.mango.vo.event.MaintenanceEventVO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class MaintenanceEventType extends EventType<MaintenanceEventKey> {

    @Autowired
    private MaintenanceEventDao maintenanceEventDao;
    private final int maintenanceId;
    private final AlarmLevel alarmLevel;

    @Deprecated
    public MaintenanceEventType(int maintenanceId, MaintenanceEventKey eventKey, AlarmLevel alarmLevel) {
        super(eventKey);
        this.maintenanceId = maintenanceId;
        this.alarmLevel = alarmLevel;
    }

    public MaintenanceEventType(MaintenanceEventVO vo) {
        super(vo.getScheduleType());
        this.maintenanceId = vo.getId();
        this.alarmLevel = vo.getAlarmLevel();
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.MAINTENANCE;
    }

    public int getMaintenanceId() {
        return maintenanceId;
    }

    @Override
    public String toString() {
        return "MaintenanceEventType(maintenanceId=" + maintenanceId + ")";
    }

    public int getReferenceId1() {
        return maintenanceId;
    }

    public int getReferenceId2() {
        return 0;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + maintenanceId;
        return result;
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
        MaintenanceEventType other = (MaintenanceEventType) obj;
        if (maintenanceId != other.maintenanceId) {
            return false;
        }
        return true;
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

}
