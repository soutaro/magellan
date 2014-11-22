var App = {
  setting: {
    appid: "",
    apikey: ""
  }
};

Pebble.addEventListener('ready', 
  function(e) {
    App.setting.appid = localStorage.getItem("nr.appid") || "";
    App.setting.apikey = localStorage.getItem("nr.apikey") || "";
  }
);

Pebble.addEventListener('showConfiguration', 
  function(e) {
    var json = JSON.stringify(App.setting);
    Pebble.openURL("https://immense-spire-6656.herokuapp.com/setting.html#" + encodeURIComponent(json));
  }
);

Pebble.addEventListener('webviewclosed', 
  function(e) {
    if (e.response) {
      var configuration = JSON.parse(decodeURIComponent(e.response));

      App.setting.appid = configuration.appid || "";
      App.setting.apikey = configuration.apikey || "";

      localStorage.setItem("nr.appid", App.setting.appid);
      localStorage.setItem("nr.apikey", App.setting.apikey);

      reloadNewRelic();
    }
  }
);

function sendResponseToPebble(application) {
  var summary = application.application_summary;

  var object = {
    "KEY_THROUGHPUT": summary.throughput,
    "KEY_APPDEX": Math.floor(summary.apdex_score * 100),
    "KEY_TIME": summary.response_time,
    "KEY_STATUS": application.health_status
  };

  Pebble.sendAppMessage(object);
}

function sendErrorToPebble() {
  console.log("sending error message to pebble...");
  error = { "KEY_ERROR": 1 }
  Pebble.sendAppMessage(error)
}

function reloadNewRelic() {
  var req = new XMLHttpRequest();
  req.setRequestHeader('X-API-Key', App.setting.apikey);
  req.open('GET', 'https://api.newrelic.com/v2/applications/'+App.setting.appid+'.json', true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      sendResponseToPebble(JSON.parse(req.responseText).application);
    } else {
      sendErrorToPebble();
    }
  }
  req.onerror = function(e) {
    sendErrorToPebble();
  }

  req.send(null);
}

Pebble.addEventListener('appmessage',
  function(e) {
    reloadNewRelic();
  }                     
);
