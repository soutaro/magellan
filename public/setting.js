window.nrm = { setting: {} };

$(function() {
  var hash = location.hash.substr(1);

  if (hash) {
    nrm.setting = JSON.parse(decodeURIComponent(hash));
  } else {
    nrm.setting = { apikey: "", appid: "" };
  }

  $("input#apikey").val(nrm.setting.apikey);
  $("input#appid").val(nrm.setting.appid);

  $("form").submit(function() {
    nrm.setting.apikey = $("input#apikey").val();
    nrm.setting.appid = $("input#appid").val();

    var json = encodeURIComponent(JSON.stringify(nrm.setting));
    location.href = "pebblejs://close#" + json;

    return false;
  });
});
