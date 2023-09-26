/*
 * WEB.cpp
 *
 *  Copyright BinOwl 2022
 *
 *  Created on: 7.04.2022
 *      Author: BinOwl
 */

#include "WEB.h"

const char* MAIN_page = 
"<!DOCTYPE html>"
"<html>"
"<style>"
".b{position:absolute;top:0;bottom:0;left:0;right:0;height:100%;background-color:#000;height:auto !important;}"
".f{border-radius: 10px;font-weight:bold;position:absolute;top:50\%;left:0;right:0;margin:auto;background:#024d27;padding:50px;box-sizing:border-box;color:#FF0;margin:30px;box-shadow:0px 2px 18px -4px #0F0;transform:translateY(-50%);}"
"#V{font-size:96px;}"
"#U{font-size: 56px;}"
"#N{font-size: 36px;}"
"</style>"
"<body><div class='b'><div class='f'><span id='N'>?</span><br><span id='V'>?</span>&nbsp;<span id='U'>?</span><br></div></div>"
"<script>"
"reqData();"
"setInterval(reqData, 200);"
"function reqData() {"
  "var xhr = new XMLHttpRequest();"
  "xhr.onload = function() {"
    "if (this.status == 200) {"
	  "var data = JSON.parse(xhr.responseText);"
	  "document.getElementById('N').innerHTML = data.n;"
      "document.getElementById('V').innerHTML = data.v;"
	  "document.getElementById('U').innerHTML = data.u;"
    "} else {"
	  "document.getElementById('N').innerHTML = \"?\";"
      "document.getElementById('V').innerHTML =  \"?\";"
	  "document.getElementById('U').innerHTML = \"?\";"
	"}"
  "};"
  "xhr.open('GET', 'readVal', true);"
  "xhr.send();"
"}"
"</script>"
"</body></html>";
// "<!DOCTYPE html>"
// "<html>"
// "<style>"
// ".frame{"
// 	"max-width: 400px;"
// 	"min-height: 250px;"
//     "background: #02b875;"
//     "padding: 30px;"
//     "box-sizing: border-box;"
//     "color: #FFF;"
//     "margin:20px;"
//     "box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);"
// "}"
// "</style>"
// "<body>"

// "<div class='frame'>"
//   "<h4><span id='N'>?</span></h4><br>"
//   "<h1><span id='V'>?</span>&nbsp;<span id='U'>?</span></h1><br>"
// "</div>"
// "<script>"
// "reqData();"
// "setInterval(reqData, 200);"
// "function reqData() {"
//   "var xhr = new XMLHttpRequest();"
//   "xhr.onload = function() {"
//     "if (this.status == 200) {"
// 	  "var data = JSON.parse(xhr.responseText);"
// 	  "document.getElementById('N').innerHTML = data.n;"
//       "document.getElementById('V').innerHTML = data.v;"
// 	  "document.getElementById('U').innerHTML = data.u;"
//     "} else {"
// 	  "document.getElementById('N').innerHTML = \"?\";"
//       "document.getElementById('V').innerHTML =  \"?\";"
// 	  "document.getElementById('U').innerHTML = \"?\";"
// 	"}"
//   "};"
//   "xhr.open('GET', 'readVal', true);"
//   "xhr.send();"
// "}"
// "</script>"
// "</body>"
// "</html>";

const char* UPDATE_page =
"<!DOCTYPE html>"
     "<html lang='en'>"
     "<head>"
         "<meta charset='utf-8'>"
         "<meta name='viewport' content='width=device-width,initial-scale=1'/>"
     "</head>"
     "<body>"
	 "<h1 style=\"color:red\">CONNECT POWER SUPPLY AND DO NOT TURN OFF IGNITION KEY FROM POSITION 2</h1>"
     "<form method='POST' action='' enctype='multipart/form-data'>"
         "Firmware:<br>"
         "<input type='file' accept='.bin' name='firmware'>"
         "<input type='submit' value='Update Firmware'>"
     "</form>"
     "</body>"
     "</html>";
     
const char* UPDATE_success =
"<META http-equiv=\"refresh\" content=\"15;URL=/\">Update Success! Rebooting...";

WebServer server(80);
HTTPUpdateServer httpUpdater;

String ajaxValue = "";

void RunWebServer() {
    WiFi.softAP(NAME);
	MDNS.begin(NAME);
	server.begin();
	MDNS.addService("http", "tcp", 80);
	server.on("/", HTTP_GET, []() {
		server.sendHeader("Connection", "close");
    	server.send(200, "text/html", MAIN_page);
  	});
	server.on("/readVal", HTTP_GET, [](){
		server.sendHeader("Connection", "close");
		server.send(200, "application/json", ajaxValue);
	});
	httpUpdater.setup(&server);
}
