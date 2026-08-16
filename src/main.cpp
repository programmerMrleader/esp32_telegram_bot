// Incliduing Necessary Library
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "settings.h"
// initinaling structure
WiFiClientSecure secured_client; // wifi client 
UniversalTelegramBot bot(BOT_TOKEN, secured_client); // bot client


void status(String &msg_id) {
  
  WiFiClientSecure testClient;
  testClient.setInsecure();

  bot.sendMessage(msg_id,"Testing HTTPS connection to Telegram...","Markdown");

  if (testClient.connect("api.telegram.org", 443))
  {
      bot.sendMessage(msg_id,"HTTPS connection Success!", "");
      testClient.stop();
  }
  else
  {
      bot.sendMessage(msg_id,"HTTPS connection FAILED!", "");
  }
  secured_client.setInsecure();

}
void music(String query, String msg_id) {
  if (query.length() == 0) {
    Serial.println("Query is empty!");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure(); // Skip SSL certificate validation for simplicity

  if (!client.connect("musicbrainz.org", 443)) {
    Serial.println("Connection to MusicBrainz failed!");
    return;
  }

  // Format search query URL (encode spaces)
  query.replace(" ", "%20");
  String search_url = "/ws/2/recording?query=recording:" + query + "&fmt=json";

  // Force HTTP/1.0 to disable Chunked Transfer Encoding
  String req = String("GET ") + search_url + " HTTP/1.0\r\n" +
               "Host: musicbrainz.org\r\n" +
               "User-Agent: ESP32TelegramBot/1.0 ( contact@example.com )\r\n" +
               "Connection: close\r\n\r\n";

  client.print(req);

  // 1. Skip HTTP Headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break; // Reached end of headers
    }
  }

  // 2. Parse JSON response stream directly
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, client);
  client.stop(); // Close connection after parsing

  if (err) {
    Serial.print("JSON Error: ");
    Serial.println(err.f_str());
    return;
  }

  // 3. Extract recordings array
  JsonArray recordings = doc["recordings"];
  if (recordings.size() == 0) {
    Serial.println("No recordings found.");
    return;
  }

  String telegramMessageSong = "**Search Results:**\n\n";
  MusicQuery music_query;

  int count = min((int)recordings.size(), 3);
  for (int i = 0; i < count; i++) {
    JsonObject rec = recordings[i];

    // Extract Title
    music_query.title = rec["title"] | "Unknown";

    // Extract Artist (Nested inside artist-credit -> 0 -> name)
    music_query.artist = rec["artist-credit"][0]["name"] | "Unknown";

    // Extract Year (From YYYY-MM-DD in first-release-date)
    String releaseDate = rec["first-release-date"] | "Unknown";
    if (releaseDate.length() >= 4) {
      music_query.year = releaseDate.substring(0, 4);
    } else {
      music_query.year = "Unknown";
    }

    // Build user message
    telegramMessageSong += String(i + 1) + ". " + music_query.title + "\n";
    telegramMessageSong += "   Artist: " + music_query.artist + "\n";
    telegramMessageSong += "   Year: " + music_query.year + "\n\n";
  }

  // Print compiled message to the LCD
  //Serial.println(telegramMessageSong);

  // Send to Telegram (uncomment when integrating UniversalTelegramBot)
  bot.sendMessage(msg_id, telegramMessageSong, "Markdown");
}
void Search_image_random(String query,String chat_id) {
  if (query.length() == 0) {
    bot.sendMessage(chat_id,"Usage /search <name>","");
    return;
  }
  
  String random_url = "/photos/random?query=" + query + "&per_page=1";
  WiFiClientSecure client;
  client.setInsecure();
  if (!client.connect(api_endpoint,443)) {
    Serial.println("Unable to connect");
    bot.sendMessage(chat_id, "Unable to connect to Unsplash", "");
    return;
  }
  String req = String("GET ") + random_url + " HTTP/1.1\r\n" +
               "Host: " + api_endpoint + "\r\n" +
               "Authorization: Client-ID " + String(UNI_SPLASH_TOKEN) + "\r\n" +
               "Connection: close\r\n\r\n";
  client.print(req);
  // reading the full response
  String resp;
  while (client.connected() || client.available()) {
    if (client.available()) resp += client.readStringUntil('\n') + "\n";
  }
  client.stop();
  int bodyIndex = resp.indexOf("\r\n\r\n");
  if (bodyIndex == -1) {
    bot.sendMessage(chat_id, "Invalid HTTP response", "");
    return;
  }
  String body = resp.substring(bodyIndex + 4);
  Serial.println("Unsplash body:");
  Serial.println(body);

  // parse JSON (ArduinoJson)
  DynamicJsonDocument doc(8 * 1024);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    String errMsg = String("JSON parse error: ") + err.c_str();
    Serial.println(errMsg);
    bot.sendMessage(chat_id, errMsg, "");
    return;
  }

  if (doc.containsKey("errors")) {

    String errorMessage;
    if (doc["errors"].is<JsonArray>() && doc["errors"].size() > 0) {
      errorMessage = doc["errors"][0].as<String>();
    } else {
      errorMessage = doc["errors"].as<String>();
    }
    Serial.println("Unsplash API error: " + errorMessage);
    bot.sendMessage(chat_id, "Unsplash error: " + errorMessage, "");
    return;
  }

  if (!doc.containsKey("results")) {
    bot.sendMessage(chat_id, "Unexpected Unsplash response", "");
    return;
  }

  JsonArray results = doc["results"].as<JsonArray>();
  if (results.size() == 0) {
    bot.sendMessage(chat_id, "No results found.", "");
    return;
  }

  String title = results[0]["description"] | results[0]["alt_description"] | "Untitled";
  String imageUrl = results[0]["urls"]["small"].as<String>();

  String reply = "Found: " + title + "\n" + imageUrl;
  bot.sendMessage(chat_id, reply, "");
}
String string_strip(String &msg_text) {
  String text = msg_text;
  int spaceIndex = text.indexOf(' ');
  String query = (spaceIndex == -1) ? "" : text.substring(spaceIndex + 1);
  query.trim();
  
  return query;
}
void handleNewMessages(int numNewMessages)
{
  //replace LCD code
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  //replace LCD code end
  String error;
  String answer;
  bool sendAnswer = false;
  for (int i = 0; i < numNewMessages; i++)

  {
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
    msg.chat_title = "Welcome";
    if (msg.text == "/help") {
      answer = "Welcome to my bot please use /start or /status or /help for available commands";
      sendAnswer = true;
    }
    else if (msg.text == "/start") {
      answer = "Welcome " + msg.chat_id + " \nPlease do not show that ID to anyone!\n";
      sendAnswer = true;
    }
    else if (msg.text == "/status") {
      status(msg.chat_id);
      sendAnswer = true;
    }
    else if (msg.text == "/chat_id") {
      answer = msg.chat_id;
      sendAnswer = true;
    }
    else if (msg.text.startsWith("/search")) {
      String query = string_strip(msg.text);
      if (query.length() == 0) {
        answer = "Usage /search <name>";
        sendAnswer = true;
      }
      else {
        query.replace(" ", "%20");
        bot.sendMessage(msg.chat_id, "Searching for: " + query, "Markdown");
        Search_image_random(query, msg.chat_id);
      }
    }
    else if (msg.text.startsWith("/music")) {
      String query = string_strip(msg.text);
      if (query.length() == 0) {
        answer = "Usage /music <name>";
        sendAnswer = true;
      }
      else {
        music(query,msg.chat_id);
      }
    }
    else {
      answer = "Say what?";
      sendAnswer = true;
    }

    if (sendAnswer) {
      bot.sendMessage(msg.chat_id, answer, "Markdown");
      sendAnswer = false;
    }
  }
}

void bot_setup()
{
  const String commands = F("["
                            "{\"command\":\"help\",\"description\":\"Get bot usage help\"},"
                            "{\"command\":\"start\",\"description\":\"Message sent when you open a chat with a bot\"},"
                            "{\"command\":\"status\",\"description\":\"Answer device current status\"},"
                            "{\"command\":\"chat_id\",\"description\":\"Show chat id\"},"
                            "{\"command\":\"search\",\"description\":\"Search photos but random\"},"
                            "{\"command\":\"music\",\"description\":\"Search music\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
  //bot.sendMessage("25235518", "Hola amigo!", "Markdown");
}
void setup()
{
  // strarting serial
  Serial.begin(9600);
  Serial.println();
  lcd_display_setup();
  // attempt to start a file system 
  if (!SPIFFS.begin(true)) {
    //replace LCD code
    Serial.println("An error has occured while mounting file system");
    return;
  }
  //replace LCD code
  lcd_show_message("SPIFFS mounted successfully");
  Serial.printf("SPIFFS totalsize=%u used=%u\n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  // replace LCD
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  // replace LCD
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  lcd_show_message("\nWiFi connected. IP address: ");
  lcd_scroll(ScrollCode::DOWN);
  Serial.println(WiFi.localIP());
  
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());

  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");

  time_t now = time(nullptr);

  while (now < 24 * 3600)
  {
      Serial.print(".");
      delay(100);
      now = time(nullptr);
  }

  Serial.println();
  Serial.println(now);

  WiFiClientSecure testClient;
  testClient.setInsecure();

  Serial.println("Testing HTTPS connection to Telegram...");

  if (testClient.connect("api.telegram.org", 443))
  {
      Serial.println("HTTPS connection SUCCESS!");
      testClient.stop();
  }
  else
  {
      Serial.println("HTTPS connection FAILED!");
  }

  secured_client.setInsecure();

  bot_setup();
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}