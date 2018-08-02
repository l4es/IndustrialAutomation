define([
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/date/locale",

    // Require the basic chart class
    "dojox/charting/Chart",
    // Require the theme of our choosing
    "dojox/charting/themes/Claro",
    //  We want to plot Lines
    "dojox/charting/plot2d/Lines",
    // Load the Legend, Tooltip, and Magnify classes
    "dojox/charting/widget/Legend",
    "dojox/charting/action2d/Tooltip",
    "dojox/charting/action2d/Magnify",
    //  We want to use Markers
    "dojox/charting/plot2d/Markers",
    //  We'll use default x/y axes
    "dojox/charting/axis2d/Default",
    // Wait until the DOM is ready
    "dojo/domReady!"
], function(declare, lang, ddl, Chart, theme, Lines, Legend, Tooltip, Magnify, Markers, Default) {


    return declare(null, {
        constructor: function(dataSet, chartNode) {
            // Define the data
            this.chartData = dataSet;

            // Create the chart within it's "holding" node
            this.chart = new Chart(chartNode);

            // Set the theme
            this.chart.setTheme(theme);

            // Add the only/default plot
            this.chart.addPlot("default", {
                type: Lines,
                markers: true
            });

            // Add axes
            this.chart.addAxis("x", {
                title: "Time",
                titleOrientation: "away",
                from: this.chartData.from,
                        to: this.chartData.to,
                labelFunc: lang.hitch(this, function(value) {
                    // I am assuming that your timestamp needs to be multiplied by 1000. 
                    var date = new Date(parseInt(value));
                    console.log("DATE: " + date);
                    return ddl.format(date, { formatLength: "short"});

                })
            });
            this.chart.addAxis("y", {
                vertical: true,
                title: this.chartData.name
            });

            // Add the series of data
            this.chart.addSeries("DP", this.chartData.xyDataSet);
//            this.chart.addSeries("y", this.chartData.valueSeries);

            // Create the tooltip
            this.tip = new Tooltip(this.chart, "default");

            // Create the magnifier
            this.mag = new Magnify(this.chart, "default");

            // Render the chart!
            this.chart.render();

            // Create the legend
            this.legend = new Legend({chart: this.chart}, "legend");
        }
    });
});