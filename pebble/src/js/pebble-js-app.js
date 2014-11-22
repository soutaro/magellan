// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
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
  error = { "KEY_ERROR": 1 }
  Pebble.sendAppMessage(error)
}

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    var req = new XMLHttpRequest();
    req.setRequestHeader('X-API-Key', 'xxxxxxxxxxxxxxxxxx');
    req.open('GET', 'https://api.newrelic.com/v2/applications/xxxxxxxxxx.json', true);
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
);
