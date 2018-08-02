package br.org.scadabr.web.dwr;

import java.util.List;

import br.org.scadabr.db.dao.ScriptDao;
import br.org.scadabr.rt.scripting.ScriptRT;
import br.org.scadabr.vo.scripting.ContextualizedScriptVO;
import br.org.scadabr.vo.scripting.ScriptVO;

import br.org.scadabr.db.IntValuePair;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.vo.DataPointExtendedNameComparator;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.web.dwr.BaseDwr;
import javax.inject.Inject;

public class ScriptsDwr extends BaseDwr {
    
    @Inject
    private ScriptDao scriptDao;
    

    public List<DataPointVO> getPoints() {
        List<DataPointVO> allPoints = DataPointDao.getInstance().getDataPoints(
                DataPointExtendedNameComparator.instance, false);
        return allPoints;
    }

    public List<ScriptVO<?>> getScripts() {
        List<ScriptVO<?>> scripts = scriptDao.getScripts();
        return scripts;
    }

    public ScriptVO<?> getScript(int id) {
        if (id == Common.NEW_ID) {
            ContextualizedScriptVO vo = new ContextualizedScriptVO();
            vo.setXid(scriptDao.generateUniqueXid());
            return vo;
        }

        return scriptDao.getScript(id);
    }

    public DwrResponseI18n saveScript(int id, String xid, String name,
            String script, List<IntValuePair> pointsOnContext,
            List<IntValuePair> objectsOnContext) {

        ContextualizedScriptVO vo = new ContextualizedScriptVO();
        vo.setId(id);
        vo.setXid(xid);
        vo.setName(name);
        vo.setScript(script);
        vo.setPointsOnContext(pointsOnContext);
        vo.setObjectsOnContext(objectsOnContext);
        vo.setUserId(Common.getUser().getId());

        DwrResponseI18n response = new DwrResponseI18n();

        vo.validate(response);

        if (response.isEmpty()) {
            scriptDao.saveScript(vo);
        }

        response.addData("seId", vo.getId());
        return response;
    }

    public void deleteScript(int scriptId) {
        scriptDao.deleteScript(scriptId);
    }

    public boolean executeScript(int scriptId) {
        ScriptVO<?> script = scriptDao.getScript(scriptId);

        try {
            if (script != null) {
                ScriptRT rt = script.createScriptRT();
                rt.execute();
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    /**
     * @return the scriptDao
     */
    public ScriptDao getScriptDao() {
        return scriptDao;
    }

    /**
     * @param scriptDao the scriptDao to set
     */
    public void setScriptDao(ScriptDao scriptDao) {
        this.scriptDao = scriptDao;
    }

}
