<%@ taglib prefix="fmt" uri="http://java.sun.com/jstl/fmt_rt" %>
<%@ taglib prefix="dijit" uri="/WEB-INF/tld/dijit.tld" %>
<%@attribute name="helpId" required="true" %>
<!DOCTYPE html>
<html>
    <head>
        <meta name="Copyright" content="&copy;©2009-2011 Fundação Certi, MCA Sistemas, Unis Sistemas, Conetec, Todos os direitos reservados."/>
        <meta name="DESCRIPTION" content="ScadaBR Software"/>
        <meta name="KEYWORDS" content="ScadaBR Software"/>
        <link rel="stylesheet" href="resources/scadabr.css">
        <link rel="icon" href="images/favicon.ico"/>
        <link rel="shortcut icon" href="images/favicon.ico"/>
        <%-- set dojos locale to that of the session --%>
        <%-- TODO USe this for timezone support ??? https://github.com/mde/timezone-js ??? --%>
        <script src='resources/dojo/dojo.js' data-dojo-config="isDebug: true, async: true, parseOnLoad: true, locale: '${pageContext.response.locale}'"></script>
        <script>
            var homeUrl = "${userSessionContextBean.userHomeUrl}";
            var _mainPage;
            function setHomeUrl() {
                _MainPage.setHomeUrl(window.location.href).then(function (newHomeUrl) {
                    homeUrl = newHomeUrl;
                    alert("HomeURL set to: " + homeUrl);
                }, function (error) {
                    alert("SetHomeUrl Error: " + error);
                });
            }
            require(["dojo/parser",
                "scadabr/jsp/MainPage",
                "dojo/ready"], function (parser, MainPage, ready) {
                ready(function () {
                    _MainPage = new MainPage()
                });
            });


        </script>
    </head>

    <body class="soria">
        <style>
            #preloader,
            body, html {
                width:100%; height:100%; margin:0; padding:0;
            }

            #preloader {
                background-color:#fff;
                position:absolute;
            }

            html, body {
                width: 100%;
                height: 100%;
                margin: 0;
                overflow:hidden;
            }

            #mainLayout {
                width: 100%;
                height: 100%;
            }
        </style>
        <!-- Prevents flicker on page load of declarative dijits-->
        <div id="preloader"></div>

        <dijit:headlineLayoutContainer id="mainLayout">
            <dijit:topContentPane >
                <dijit:headlineLayoutContainer>
                    <dijit:leftContentPane>
                        <i><%-- TODO version number not shown ??? Why??? --%>
                            ScadaBR ${scadaBrVersionBean.versionNumber}@${pageContext.request.localAddr}:${pageContext.request.localPort}${pageContext.request.contextPath}
                        </i>
                    </dijit:leftContentPane>
                    <dijit:rightContentPane>
                        <img src="images/mangoLogoMed.jpg" alt="Logo"/>
                    </dijit:rightContentPane>
                    <dijit:bottomContentPane>
                        <dijit:toolbar>
                            <dijit:button iconClass="scadaBrWatchListIcon" i18nTitle="header.watchlist">
                                <script type="dojo/connect" data-dojo-event="onClick">window.location = "watchList";</script>
                            </dijit:button>
                            <dijit:button iconClass="scadaBrEventsIcon" i18nTitle="header.alarms">
                                <script type="dojo/connect" data-dojo-event="onClick">window.location = "events";</script>
                            </dijit:button>
                            <dijit:button iconClass="dsIcon" i18nTitle="header.dataSources">
                                <script type="dojo/connect" data-dojo-event="onClick">window.location = "dataSources";</script>
                            </dijit:button>
                            <dijit:button iconClass="scadaBrPointHierarchyIcon" i18nTitle="header.pointHierarchy" >
                                <script type="dojo/connect" data-dojo-event="onClick">window.location = "dataPoints";</script>
                            </dijit:button>
                            <dijit:toolbarSeparator/>
                            <dijit:button iconClass="scadaBrLogoutIcon" i18nTitle="header.logout" >
                                <script type="dojo/connect" data-dojo-event="onClick">window.location = "logout";</script>
                            </dijit:button>
                            <dijit:button iconClass="scadaBrHelpIcon" i18nTitle="header.help" >
                                <script type="dojo/connect" data-dojo-event="onClick">_MainPage.showHelp("${pageScope.helpId}");</script>
                            </dijit:button>
                            <div style="float:right;"><%-- move content to the right side --%>
                                <dijit:toolbarSeparator/>
                                <label><fmt:message key="common.user" />:&nbsp;<b>${userSessionContextBean.username}</b></label>
                                <dijit:button iconClass="scadaBrGotoHomeUrlIcon" i18nTitle="header.goHomeUrl" disabled="${!userSessionContextBean.loggedIn}">
                                    <script type="dojo/connect" data-dojo-event="onClick">window.location = homeUrl;</script>
                                </dijit:button>
                                <dijit:button iconClass="scadaBrSetHomeUrlIcon" i18nTitle="header.setHomeUrl" disabled="${!userSessionContextBean.loggedIn}">
                                    <script type="dojo/connect" data-dojo-event="onClick">setHomeUrl();</script>
                                </dijit:button>
                            </div>
                        </dijit:toolbar>
                    </dijit:bottomContentPane>
                </dijit:headlineLayoutContainer>
            </dijit:topContentPane>
            <dijit:centerContentPane >
                <jsp:doBody />
            </dijit:centerContentPane>
            <dijit:bottomContentPane >
                <span>&copy;2009-2014 Funda&ccedil;&atilde;o Certi, MCA Sistemas, Unis Sistemas, Conetec. <fmt:message key="footer.rightsReserved"/></span>
            </dijit:bottomContentPane>
        </dijit:headlineLayoutContainer>

    </body>
</html>