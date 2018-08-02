<%--TODO 

setting cbo Latest disable to* clicking inception fetching inception and setting and disabling from*

calculate major|minior chart ticks...
--%>
<%@ include file="/WEB-INF/jsp/include/tech.jsp" %>
<dijit:headlineLayoutContainer id="renderChartContent">
    <dijit:topContentPane>
        <form id="timeFilterId">
            <dojox:tableContainer cols="2" style="float:left;">
                <dijit:dateTextBox id="fromDateId" i18nLabel="common.dateRangeFrom" timestamp="${fromTimestamp}" />
                <dijit:dateTextBox id="toDateId" i18nLabel="common.dateRangeTo" timestamp="${toTimestamp}" />
                <dojox:timeSpinner id="fromTimeId" i18nLabel="common.dateRangeFrom" timestamp="${fromTimestamp}" />
                <dojox:timeSpinner id="toTimeId" i18nLabel="common.dateRangeTo" timestamp="${toTimestamp}" />
                <dijit:checkBox id="checkInceptionId" i18nLabel="common.inception" checked="false" />
                <dijit:checkBox id="checkLatestId" i18nLabel="common.latest" checked="true"/>
            </dojox:tableContainer>
            <dijit:button type="submit" i18nLabel="login.loginButton" />
        </form>
    </dijit:topContentPane>
    <dijit:centerContentPane>
        <div id="chartId"></div >
    </dijit:centerContentPane>
</dijit:headlineLayoutContainer>

<script type="text/javascript">
        var fromDate;
        var fromTime;
        var toDate;
        var toTime;
        function getFromTimestamp() {
            return fromDate.get("value").getTime() + fromTime.get("value").getTime() - new Date().getTimezoneOffset() * 60 * 1000;
        }

        function getToTimestamp() {
            return toDate.get("value").getTime() + toTime.get("value").getTime() - new Date().getTimezoneOffset() * 60 * 1000;
        }

    require([
        "dojo/dom",
        "dojo/on",
        "dijit/registry",
        "dojo/ready",
        "dojo/parser",
        "dojox/charting/StoreSeries"
    ], function (dom, on, registry, ready, parser, StoreSeries) {

        parser.parse(dom.byId("renderChartContent").parentNode).then(function () {
            var form = dom.byId('timeFilterId');
            fromDate = registry.byId("fromDateId");
            fromTime = registry.byId("fromTimeId");
            toDate = registry.byId("toDateId");
            toTime = registry.byId("toTimeId");

            var date = new Date();
            date.setTime(${fromTimestamp});
          //  fromDate.set("value", date);
            fromTime.set("value", fromTime.format(date)); //TODO Bugfix for timeSpinner
            date.setTime(${toTimestamp});
          //  toDate.set("value", date);
            toTime.set("value", toTime.format(date));

            // Attach the onsubmit event handler of the form
            on(form, "submit", function (evt) {
                // prevent the page from navigating after submit
                evt.stopPropagation();
                evt.preventDefault();
                var query = {
                    from: getFromTimestamp(),
                    to: getToTimestamp()
                };
                chart.updateSeries("Series A", new StoreSeries(store, {query: query}, {x: "x", y: "y"}));
            });
            setupChart();
        }, function (error) {
            alert(error);
        });
    })
    var store;
    var chart;
    //TODO calc some meaningful values
    function calcMajorTickStep() {
        return 1000 * 3600 * 24; // 7 Days
    }

    function calcMinorTickStep() {
        return 1000 * 3600 * 3;
    }

    function setupChart() {
        require(["dojo/store/JsonRest",
            "dojox/charting/Chart",
            "dojox/charting/StoreSeries",
            "dojox/charting/plot2d/Lines",
            "dojox/charting/plot2d/Grid",
            "dojox/charting/axis2d/Default"
        ], function (JsonRest, Chart, StoreSeries) {
            store = new JsonRest({
                target: "rest/pointValues/${dataPoint.id}",
                idProperty: "x"
            });
            chart = new Chart("chartId");
            chart.addPlot("grid", {
                type: "Grid", hMajorLines: true, vMajorLines: true, hMinorLines: true, vMinorLines: false
            });
            chart.addPlot("default", {type: "Lines", markers: true});
            chart.addAxis("x", {
                title: "Timestamp",
//                from: startTime,
//                min: startTime,
                titleOrientation: "away",
                majorLabels: true, majorTicks: true, majorTick: {length: 10},
                minorLabels: true, minorTicks: true, minorTick: {length: 6},
                microTicks: false,
                majorTickStep: calcMajorTickStep(),
                minorTickStep: calcMinorTickStep(),
                natural: false,
                fixed: true,
                labelFunc: function (text, value, precision) {
                    var d = new Date();
                    d.setTime(value);
                    // todo timestamp is utc not current timezone
                    return d.toLocaleString();
                }
            });
            chart.addAxis("y", {
                vertical: true,
                fixUpper: "major",
                includeZero: true,
                title: "${dataPoint.name}"
            });
            chart.addSeries("Series A", new StoreSeries(store, {query: {from: getFromTimestamp(), to: getToTimestamp()}}, {x: "x", y: "y"}), {stroke: {color: "red"}});
            chart.render();
        });
    }

</script>