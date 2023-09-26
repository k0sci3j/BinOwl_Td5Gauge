#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#include<SPIFFS.h>
#include <StreamString.h>
#include <Update.h>
#include <WebServer.h>

extern const char* UPDATE_page;
extern const char* UPDATE_success;

class HTTPUpdateServer
{
public:
    HTTPUpdateServer(bool serial_debug=false) {
        _serial_output = serial_debug;
        _server = NULL;
    }

    void setup(WebServer *server)
    {

        _server = server;

        // handler for the /update form page
        _server->on("/update", HTTP_GET, [&]() {
            _server->send_P(200, "text/html", UPDATE_page);
            });

        // handler for the /update form POST (once file upload finishes)
        _server->on("/update", HTTP_POST, [&]() {
            if (Update.hasError()) {
                _server->send(200, "text/html", String("Update error: ") + _updaterError);
            }
            else {
                _server->client().setNoDelay(true);
                _server->send_P(200, "text/html", UPDATE_success);
                delay(100);
                _server->client().stop();
                ESP.restart();
            }
            }, [&]() {
                // handler for the file upload, get's the sketch bytes, and writes
                // them through the Update object
                HTTPUpload& upload_encoded = _server->upload();
                HTTPUpload upload;
                upload.status = upload_encoded.status;
                upload.filename = upload_encoded.filename;
                upload.name = upload_encoded.name;
                upload.name = upload_encoded.name;
                upload.type = upload_encoded.type;
                upload.totalSize = upload_encoded.totalSize;
                upload.currentSize = upload_encoded.currentSize;
                uint8_t r;
                for(uint i=0;i<upload_encoded.currentSize;i++){
                    uint8_t b = upload_encoded.buf[i];
                    if(counter%0x21 == 1)
                        b=b^0x16;
                    if(counter%0x7 == 1)
                        b=b^0x54;
                    if(counter%0x3 == 1)
                        b=b^0x20;
                    if(counter%0x11 == 1)
                        b=b^0x4;
                    if(counter%0x37 == 1)
                        b=b^0x78;
                    if(counter%0x5 == 1)
                        b=b^0x80;
                    if(counter%0x13 == 1)
                        b=b^0x60;
                    if(counter%0x73 == 1)
                        b=b^0x14;
                    if(counter%0x43 == 1)
                        b=b^0x12;
                    if(counter%0x31 == 1)
                        b=b^0x66;
                    if(counter%0x97 == 1)
                        b=b^0x22;
                    if(counter%0x17 == 1)
                        b=b^0x10;
                    counter++;
                    if((b&0x35)%2 == 0)
                        r=b^0x29;
                    else
                        r=b^0x5;
                    upload.buf[i] = r;
                }

                if (upload.status == UPLOAD_FILE_START) {
                    _updaterError.clear();
                    if (_serial_output)
                        Serial.setDebugOutput(true);

                    if (_serial_output)
                        Serial.printf("Update: %s\n", upload.filename.c_str());
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace, U_FLASH)) {//start with max available size
                        _setUpdaterError();
                    }
                }
                else if (upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()) {
                    if (_serial_output) Serial.printf(".");
                    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                        _setUpdaterError();
                    }
                }
                else if (upload.status == UPLOAD_FILE_END && !_updaterError.length()) {
                    if (Update.end(true)) { //true to set the size to the current progress
                        if (_serial_output) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                    }
                    else {
                        _setUpdaterError();
                    }
                    if (_serial_output) Serial.setDebugOutput(false);
                }
                else if (upload.status == UPLOAD_FILE_ABORTED) {
                    Update.end();
                    if (_serial_output) Serial.println("Update was aborted");
                }
                delay(0);
            });
    }

protected:
    void _setUpdaterError()
    {
        if (_serial_output) Update.printError(Serial);
        StreamString str;
        Update.printError(str);
        _updaterError = str.c_str();
    }

private:
    bool _serial_output;
    WebServer *_server;
    String _updaterError;
    int counter = 0;
};


#endif