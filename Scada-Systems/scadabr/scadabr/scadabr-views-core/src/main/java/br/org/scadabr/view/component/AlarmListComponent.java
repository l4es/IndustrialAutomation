package br.org.scadabr.view.component;

import br.org.scadabr.DataType;
import br.org.scadabr.dao.EventDao;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;


import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.event.AlarmLevel;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.view.ImplDefinition;
import java.util.EnumSet;
import java.util.LinkedList;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class AlarmListComponent extends CustomComponent {

    public static ImplDefinition DEFINITION = new ImplDefinition("alarmlist",
            "ALARMLIST", "graphic.alarmlist", EnumSet.noneOf(DataType.class));

    @Autowired
    private EventDao eventDao;

    private AlarmLevel minAlarmLevel = AlarmLevel.INFORMATION;

    private int maxListSize = 5;

    private int width = 500;

    private boolean hideIdColumn = true;
    private boolean hideAlarmLevelColumn = false;
    private boolean hideTimestampColumn = false;
    private boolean hideInactivityColumn = true;
    private boolean hideAckColumn = false;

    @Override
    public String generateContent() {
        throw new ImplementMeException();
/*
        Map<String, Object> model = new HashMap<>();
        HttpServletRequest request = webContext.getHttpServletRequest();

        List<EventInstance> events = filter(eventDao.getPendingEvents(Common.getUser()), minAlarmLevel);

        model.put("nome", "marlon");
        model.put("events", events);
        model.put("width", width > 0 ? width : 500);
        model.put("hideIdColumn", hideIdColumn);
        model.put("hideAlarmLevelColumn", hideAlarmLevelColumn);
        model.put("hideTimestampColumn", hideTimestampColumn);
        model.put("hideInactivityColumn", hideInactivityColumn);
        model.put("hideAckColumn", hideAckColumn);

        throw new ImplementMeException();
        //TODO String content = BaseDwr.generateContent(request, "alarmList.jsp", model);
        // return content;
        */
    }

    private List<EventInstance> filter(Iterable<EventInstance> eventInstances, AlarmLevel alarmLevel) {
        LinkedList result = new LinkedList();
        int count = 0;
        for (EventInstance eventInstance : eventInstances) {
            if (eventInstance.getAlarmLevel().compareTo(alarmLevel) >= 0) {
                result.add(eventInstance);
                if (maxListSize >= count++) {
                    return result;
                }
            }
        }
        return result;
    }

    @Override
    public boolean containsValidVisibleDataPoint(int dataPointId) {
        return false;
    }

    @Override
    public ImplDefinition definition() {
        return DEFINITION;
    }

    @Override
    public String generateInfoContent() {
        return "<b> info content</b>";
    }

    public int getMaxListSize() {
        return maxListSize;
    }

    public void setMaxListSize(int maxListSize) {
        this.maxListSize = maxListSize;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public boolean isHideIdColumn() {
        return hideIdColumn;
    }

    public void setHideIdColumn(boolean hideIdColumn) {
        this.hideIdColumn = hideIdColumn;
    }

    public boolean isHideTimestampColumn() {
        return hideTimestampColumn;
    }

    public void setHideTimestampColumn(boolean hideTimestampColumn) {
        this.hideTimestampColumn = hideTimestampColumn;
    }

    public boolean isHideAlarmLevelColumn() {
        return hideAlarmLevelColumn;
    }

    public void setHideAlarmLevelColumn(boolean hideAlarmLevelColumn) {
        this.hideAlarmLevelColumn = hideAlarmLevelColumn;
    }

    public boolean isHideInactivityColumn() {
        return hideInactivityColumn;
    }

    public void setHideInactivityColumn(boolean hideInactivityColumn) {
        this.hideInactivityColumn = hideInactivityColumn;
    }

    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(minAlarmLevel.getId());
        out.writeInt(maxListSize);
        out.writeInt(width);
        out.writeBoolean(hideIdColumn);
        out.writeBoolean(hideAlarmLevelColumn);
        out.writeBoolean(hideTimestampColumn);
        out.writeBoolean(hideInactivityColumn);
        out.writeBoolean(hideAckColumn);

    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();
        // Switch on the version of the class so that version changes can be
        // elegantly handled.
        if (ver == 1) {
            minAlarmLevel = AlarmLevel.fromId(in.readInt());
            maxListSize = in.readInt();
            width = in.readInt();
            hideIdColumn = in.readBoolean();
            hideAlarmLevelColumn = in.readBoolean();
            hideTimestampColumn = in.readBoolean();
            hideInactivityColumn = in.readBoolean();
            hideAckColumn = in.readBoolean();
        }

    }

    public void setHideAckColumn(boolean hideAckColumn) {
        this.hideAckColumn = hideAckColumn;
    }

    public boolean isHideAckColumn() {
        return hideAckColumn;
    }

    public void setMinAlarmLevel(AlarmLevel minAlarmLevel) {
        this.minAlarmLevel = minAlarmLevel;
    }

    public AlarmLevel getMinAlarmLevel() {
        return minAlarmLevel;
    }

}
