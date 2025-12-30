#include "web_server.h"
#include "config.h"
#include "matrix_patterns.h"

WiFiServer server(80);

extern CRGB leds[NUM_LEDS];
extern CRGB leds2[NUM_LEDS];
extern CRGB matrix_leds[MATRIX_NUM_LEDS];
extern uint8_t gCurrentPatternNumber;
extern uint32_t gPatternStartTime;
extern bool gHoldPattern;
extern uint8_t gStripBrightness;
extern uint8_t gMatrixBrightness;
extern const char* const kPatternNames[];
extern const uint8_t gNumPatterns;
extern DisplayMode gDisplayMode;

void setPattern(uint8_t newPattern);
void nextPatternManual();
void previousPatternManual();
void toggleHold();
void toggleDisplayMode();
void nextMatrixPatternManual();
void previousMatrixPatternManual();

void sendJsonResponse(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/json");
    client.println();

    client.print("{\"patternNumber\": ");
    client.print(gCurrentPatternNumber + 1);
    client.print(", \"patternName\": \"");
    client.print(kPatternNames[gCurrentPatternNumber]);
    client.print("\", \"hold\": ");
    client.print(gHoldPattern ? "true" : "false");
    client.print(", \"stripBrightness\": ");
    client.print(gStripBrightness);
    client.print(", \"matrixBrightness\": ");
    client.print(gMatrixBrightness);
    client.print(", \"matrixPatternName\": \"");
    client.print(getMatrixPatternName());
    client.print("\", \"matrixRemainingTime\": ");
    client.print(getMatrixRemainingTime());
    client.print(", \"displayMode\": \"");
    client.print(gDisplayMode == SECTIONAL ? "Sectional" : "Whole");
    client.print("\"");

    if (!gHoldPattern) {
        uint32_t duration = (uint32_t)gDesignConfigs[gCurrentPatternNumber].duration_seconds * 1000;
        uint32_t elapsed = millis() - gPatternStartTime;
        uint32_t remaining = (duration > elapsed) ? (duration - elapsed) / 1000 : 0;
        client.print(", \"remainingTime\": ");
        client.print(remaining);
    }

    client.println("}");

    client.println();
}

void sendStripLedsBinary(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/octet-stream");
    client.println("Access-Control-Allow-Origin: *");
    client.println();
    client.write((uint8_t*)leds, sizeof(leds));
}

void sendMatrixLedsBinary(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/octet-stream");
    client.println("Access-Control-Allow-Origin: *");
    client.println();
    client.write((uint8_t*)matrix_leds, sizeof(matrix_leds));
}

void sendStripLedsSampledBinary(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/octet-stream");
    client.println("Access-Control-Allow-Origin: *");
    client.println();

    const int num_leds_sampled = NUM_LEDS / WEB_PREVIEW_SAMPLING_RATE;
    uint8_t buffer[num_leds_sampled * 3];
    for (int i = 0; i < num_leds_sampled; i++) {
        buffer[i * 3] = leds[i * WEB_PREVIEW_SAMPLING_RATE].r;
        buffer[i * 3 + 1] = leds[i * WEB_PREVIEW_SAMPLING_RATE].g;
        buffer[i * 3 + 2] = leds[i * WEB_PREVIEW_SAMPLING_RATE].b;
    }
    client.write(buffer, sizeof(buffer));
}

void sendMatrixLedsSampledBinary(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/octet-stream");
    client.println("Access-Control-Allow-Origin: *");
    client.println();

    const int sampled_width = MATRIX_WIDTH / WEB_PREVIEW_SAMPLING_RATE;
    const int sampled_height = MATRIX_HEIGHT / WEB_PREVIEW_SAMPLING_RATE;
    const int num_leds_sampled = sampled_width * sampled_height;
    uint8_t buffer[num_leds_sampled * 3];

    for (int y = 0; y < sampled_height; y++) {
        for (int x = 0; x < sampled_width; x++) {
            int original_x = x * WEB_PREVIEW_SAMPLING_RATE;
            int original_y = y * WEB_PREVIEW_SAMPLING_RATE;
            int index = XY(original_x, original_y);
            if (index != -1) {
                int buffer_index = (y * sampled_width + x) * 3;
                buffer[buffer_index] = matrix_leds[index].r;
                buffer[buffer_index + 1] = matrix_leds[index].g;
                buffer[buffer_index + 2] = matrix_leds[index].b;
            }
        }
    }
    client.write(buffer, sizeof(buffer));
}

void sendHtmlResponse(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<!DOCTYPE html><html><head><title>LED Pattern Controller</title><style>");
    client.println("body { font-family: sans-serif; background-color: #121212; color: #e0e0e0; }");
    client.println("h1, h2, h3 { color: #ffffff; }");
    client.println("button, select { background-color: #333; color: #e0e0e0; border: 1px solid #555; padding: 5px 10px; margin: 2px; }");
    client.println("input[type=range] { width: 300px; }");
    client.println("hr { border-color: #333; }");
    client.println("</style></head><body>");
    client.println("<h1>LED Pattern Controller</h1>");

    client.print("<p id='currentPattern'>Current Pattern: ");
    client.print(gCurrentPatternNumber + 1);
    client.print(" - ");
    client.print(kPatternNames[gCurrentPatternNumber]);
    client.println("</p>");

    client.println("<div>");
    client.println("<button onclick=\"sendCommand('previous')\">Previous</button>");
    client.println("<button onclick=\"sendCommand('toggle-hold')\" id='holdBtn'>Hold</button>");
    client.println("<button onclick=\"sendCommand('next')\">Next</button>");
    client.println("<button onclick=\"sendCommand('toggle-mode')\">Toggle Mode</button>");
    client.println("</div>");

    client.println("<p id='remainingTime'></p>");
    client.println("<p id='displayMode'></p>");
    client.println("<hr>");

    client.println("<h2>Controls</h2>");
    client.println("<div>");
    client.println("<label for='pattern'>Select a Pattern:</label>");
    client.println("<select id='pattern' name='pattern'>");
    for (int i = 0; i < gNumPatterns; i++) {
        client.print("<option value='");
        client.print(i);
        if (i == gCurrentPatternNumber) { client.print("' selected>"); } else { client.print("'>"); }
        client.print(i + 1);
        client.print(" - ");
        client.print(kPatternNames[i]);
        client.println("</option>");
    }
    client.println("</select>");
    client.println("<button onclick=\"sendCommand('set-pattern-' + document.getElementById('pattern').value)\">Send</button>");
    client.println("</div>");
    client.println("<div>");
    client.println("<label for='brightness_strips'>Strip Brightness (Pin 6):</label>");
    client.println("<input type='range' id='brightness_strips' name='brightness_strips' min='0' max='255' value='" + String(gStripBrightness) + "' onchange=\"sendCommand('brightness_strips=' + this.value)\">");
    client.println("</div>");
    client.println("<div>");
    client.println("<label for='brightness_matrix'>Matrix Brightness (Pin 5):</label>");
    client.println("<input type='range' id='brightness_matrix' name='brightness_matrix' min='0' max='255' value='" + String(gMatrixBrightness) + "' onchange=\"sendCommand('brightness_matrix=' + this.value)\">");
    client.println("</div>");
    client.println("<hr>");

    client.println("<h2>Matrix Controls</h2>");
    client.println("<div>");
    client.println("<button onclick=\"sendCommand('matrix-previous')\">Previous</button>");
    client.println("<button onclick=\"sendCommand('matrix-next')\">Next</button>");
    client.println("</div>");
    client.println("<p id='currentMatrixPattern'></p>");
    client.println("<p id='matrixRemainingTime'></p>");
    client.println("<hr>");

    client.println("<h2>Real-Time Display</h2>");
    client.println("<div>");
    client.println("  <input type='checkbox' id='toggle-strip-realtime' onchange='toggleStripRealtime()'> <label for='toggle-strip-realtime'>Enable Strip Display</label>");
    client.println("  <input type='checkbox' id='toggle-matrix-realtime' onchange='toggleMatrixRealtime()'> <label for='toggle-matrix-realtime'>Enable Matrix Display</label>");
    client.println("</div>");
    client.println("<div id='display-container' style='margin: 20px 0;'>");
    client.println("  <h3>Strip</h3>");
    client.println("  <canvas id='strip-canvas' width='700' height='102' style='margin-bottom: 10px; border: 1px solid #333;'></canvas>");
    client.println("  <h3>Matrix</h3>");
    client.println("  <canvas id='matrix-canvas' width='" + String(MATRIX_WIDTH * 16 - 1) + "' height='" + String(MATRIX_HEIGHT * 16 - 1) + "' style='border: 1px solid #333;'></canvas>");
    client.println("</div>");

    client.println("<script>");
    client.println("function sendCommand(action) { fetch('/control?action=' + action); }");
    client.println("function updateStatus() {");
    client.println("  fetch('/status').then(response => response.json()).then(data => {");
    client.println("    document.getElementById('currentPattern').innerText = `Current Pattern: ${data.patternNumber} - ${data.patternName}`;");
    client.println("    document.getElementById('holdBtn').innerText = data.hold ? 'Resume' : 'Hold';");
    client.println("    document.getElementById('brightness_strips').value = data.stripBrightness;");
    client.println("    document.getElementById('brightness_matrix').value = data.matrixBrightness;");
    client.println("    document.getElementById('currentMatrixPattern').innerText = `Matrix Pattern: ${data.matrixPatternName}`;");
    client.println("    document.getElementById('matrixRemainingTime').innerText = `Time Remaining: ${data.matrixRemainingTime}s`;");
    client.println("    document.getElementById('displayMode').innerText = `Display Mode: ${data.displayMode}`;");
    client.println("    if (data.remainingTime !== undefined) { document.getElementById('remainingTime').innerText = `Time Remaining: ${data.remainingTime}s`; } else { document.getElementById('remainingTime').innerText = ''; }");
    client.println("  });");
    client.println("}");
    client.println("setInterval(updateStatus, 1000);");

    client.println("const MATRIX_WIDTH = " + String(MATRIX_WIDTH) + ";");
    client.println("const MATRIX_HEIGHT = " + String(MATRIX_HEIGHT) + ";");
    client.println("const STRIP_LENGTH = " + String(NUM_LEDS) + ";");

    client.println("let stripRealtimeInterval, matrixRealtimeInterval;");

    client.println("function updateStripLeds() {");
    client.println("  if (!document.getElementById('toggle-strip-realtime').checked) return;");
    client.println("  fetch('/strip_sampled.bin')");
    client.println("    .then(response => response.arrayBuffer())");
    client.println("    .then(buffer => {");
    client.println("      const data = new Uint8Array(buffer);");
    client.println("      const canvas = document.getElementById('strip-canvas');");
    client.println("      const ctx = canvas.getContext('2d');");
    client.println("      const ledWidth = 7 * " + String(WEB_PREVIEW_SAMPLING_RATE) + ";");
    client.println("      const ledHeight = 15;");
    client.println("      let offset = 0;");
    client.println("      for (let row = 0; row < 6; row++) {");
    client.println("        for (let i = 0; i < 100 / " + String(WEB_PREVIEW_SAMPLING_RATE) + "; i++) {");
    client.println("          ctx.fillStyle = `rgb(${data[offset]}, ${data[offset+1]}, ${data[offset+2]})`;");
    client.println("          ctx.fillRect(i * ledWidth, row * (ledHeight + 2), ledWidth - 2, ledHeight);");
    client.println("          offset += 3;");
    client.println("        }");
    client.println("      }");
    client.println("    })");
    client.println("    .catch(error => console.error('Error fetching strip data:', error));");
    client.println("}");

    client.println("function updateMatrixLeds() {");
    client.println("  if (!document.getElementById('toggle-matrix-realtime').checked) return;");
    client.println("  fetch('/matrix_sampled.bin')");
    client.println("    .then(response => response.arrayBuffer())");
    client.println("    .then(buffer => {");
    client.println("      const data = new Uint8Array(buffer);");
    client.println("      const canvas = document.getElementById('matrix-canvas');");
    client.println("      const ctx = canvas.getContext('2d');");
    client.println("      const ledSize = 16 * " + String(WEB_PREVIEW_SAMPLING_RATE) + ";");
    client.println("      const sampled_width = MATRIX_WIDTH / " + String(WEB_PREVIEW_SAMPLING_RATE) + ";");
    client.println("      for (let i = 0; i < data.length / 3; i++) {");
    client.println("        const x = i % sampled_width;");
    client.println("        const y = Math.floor(i / sampled_width);");
    client.println("        ctx.fillStyle = `rgb(${data[i*3]}, ${data[i*3+1]}, ${data[i*3+2]})`;");
    client.println("        ctx.fillRect(x * ledSize, y * ledSize, ledSize - 1, ledSize - 1);");
    client.println("      }");
    client.println("    })");
    client.println("    .catch(error => console.error('Error fetching matrix data:', error));");
    client.println("}");

    client.println("function toggleStripRealtime() {");
    client.println("  if (document.getElementById('toggle-strip-realtime').checked) {");
    client.println("    if (!stripRealtimeInterval) { stripRealtimeInterval = setInterval(updateStripLeds, 250); }");
    client.println("  } else {");
    client.println("    clearInterval(stripRealtimeInterval); stripRealtimeInterval = null;");
    client.println("  }");
    client.println("}");

    client.println("function toggleMatrixRealtime() {");
    client.println("  if (document.getElementById('toggle-matrix-realtime').checked) {");
    client.println("    if (!matrixRealtimeInterval) { matrixRealtimeInterval = setInterval(updateMatrixLeds, 250); }");
    client.println("  } else {");
    client.println("    clearInterval(matrixRealtimeInterval); matrixRealtimeInterval = null;");
    client.println("  }");
    client.println("}");
    
    client.println("document.addEventListener('DOMContentLoaded', () => { /* No initDisplay needed */ });");

    client.println("</script>");
    client.println("</body></html>");
    client.println();
}

void initWebServer() {
    server.begin();
    Serial.println("Web server started");
}

void handleWebServer() {
    WiFiClient client = server.available();
    if (client) {
        String request = client.readStringUntil('\n');
        request.trim();

        bool isStatusRequest = request.startsWith("GET /status");
        bool isStripLedsRequest = request.startsWith("GET /strip.bin");
        bool isMatrixLedsRequest = request.startsWith("GET /matrix.bin");
        bool isStripLedsSampledRequest = request.startsWith("GET /strip_sampled.bin");
        bool isMatrixLedsSampledRequest = request.startsWith("GET /matrix_sampled.bin");

        if (!isStatusRequest && !isStripLedsRequest && !isMatrixLedsRequest && !isStripLedsSampledRequest && !isMatrixLedsSampledRequest) {
            Serial.println("New client");
        }

        if (isStatusRequest) {
            Serial.println("Status request received");
            sendJsonResponse(client);
        } else if (isStripLedsRequest) {
            sendStripLedsBinary(client);
        } else if (isMatrixLedsRequest) {
            sendMatrixLedsBinary(client);
        } else if (isStripLedsSampledRequest) {
            sendStripLedsSampledBinary(client);
        } else if (isMatrixLedsSampledRequest) {
            sendMatrixLedsSampledBinary(client);
        } else if (request.startsWith("GET /control?action=")) {
            Serial.print("Control request received: ");
            int actionIndex = request.indexOf('=') + 1;
            int endIndex = request.indexOf(' ', actionIndex);
            String action = request.substring(actionIndex, endIndex);

            if (action == "next") {
                nextPatternManual();
            } else if (action == "previous") {
                previousPatternManual();
            } else if (action == "toggle-hold") {
                toggleHold();
            } else if (action.startsWith("set-pattern-")) {
                setPattern(action.substring(12).toInt());
            } else if (action.startsWith("brightness_strips=")) {
                gStripBrightness = action.substring(18).toInt();
            } else if (action.startsWith("brightness_matrix=")) {
                gMatrixBrightness = action.substring(18).toInt();
            } else if (action == "matrix-next") {
                nextMatrixPatternManual();
            } else if (action == "matrix-previous") {
                previousMatrixPatternManual();
            } else if (action == "toggle-mode") {
                toggleDisplayMode();
            }
            sendJsonResponse(client);
        } else if (request.startsWith("GET /")) {
            sendHtmlResponse(client);
        }

        // After sending the response, close the connection
        delay(1);
        client.stop();

        if (!isStatusRequest && !isStripLedsRequest && !isMatrixLedsRequest) {
            Serial.println("Client disconnected");
        }
    }
}
