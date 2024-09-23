var sheet_id = "1rf_CХХХХХХХХХХХХХХХХХХХХХХ_fSaVM"; // URL таблицы
var sheet_name = "meteo"; // имя листа
function doGet(e) {
  var ss = SpreadsheetApp.openById(sheet_id);
  var sheet = ss.getSheetByName(sheet_name);
  var date = String(e.parameter.date);
  var temp = Number(e.parameter.temp);
  var hum = Number(e.parameter.hum);
  var press = Number(e.parameter.press);
  var temp1 = Number(e.parameter.temp1);
  var hum1 = Number(e.parameter.hum1);
  var press1 = Number(e.parameter.press1);
  var light = Number(e.parameter.light);
  sheet.appendRow([date, temp, hum, press, temp1, hum1, press1, light]);
}
