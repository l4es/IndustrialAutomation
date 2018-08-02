package br.ufsc.edugraf.rose.recursos;

import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.representation.Representation;
import org.restlet.representation.Variant;
import org.restlet.resource.Get;
import org.restlet.resource.ServerResource;

import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.EventDao;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.rt.event.type.DataPointEventType;
import com.serotonin.mango.rt.event.type.EventType.EventSources;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.event.PointEventDetectorVO;

public class RecursoAlarmesAtivos extends ServerResource {

	@Override
	protected void doInit() {

	}

	@Get
	public Representation represent(Variant variant) {

		Representation representacao = null;
		JSONObject alarmes = new JSONObject();

		List<EventInstance> activeEvents = new EventDao().getActiveEvents();

		for (EventInstance evt : activeEvents) {

			if (evt.isAcknowledged() || evt.isSilenced()) {
				continue;
			}

			if (evt.getEventType().getEventSourceId() == EventSources.DATA_POINT) {

				DataPointEventType evtType = (DataPointEventType) evt
						.getEventType();

				DataPointVO datapointvo = new DataPointDao().getDataPoint(evt
						.getEventType().getDataPointId());

				if (datapointvo.getXid().startsWith("ID")) {

					for (PointEventDetectorVO detector : datapointvo
							.getEventDetectors()) {

						if ((evtType.getPointEventDetectorId() == detector
								.getId())
								&& ((detector.getDetectorType() == PointEventDetectorVO.TYPE_ANALOG_LOW_LIMIT) || (detector
										.getDetectorType() == PointEventDetectorVO.TYPE_ANALOG_HIGH_LIMIT))) {

							try {

								alarmes.append("alarme", "ativo");

								representacao = new JsonRepresentation(alarmes);
								activeEvents = null;
								evtType = null;
								datapointvo = null;
								return representacao;

							} catch (JSONException e) {
								e.printStackTrace();
								getResponse().setStatus(
										Status.SERVER_ERROR_INTERNAL);
							}

						}
					}

				}
			}
		}

		representacao = new JsonRepresentation(alarmes);

		return representacao;

	}
}
