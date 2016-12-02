const char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
 "<head>"
   "<title>Der Wecker</title>"
   "<style>"
     "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
   "</style>"
 "</head>"
 "<body>"
   "<h1>Der Wecker</h1>"
   "<FORM action=\"/\" method=\"post\">"
    "<P>"
      "Test123<br>"
      "<INPUT type=\"text\" name=\"messagebox\"><br>"
      "<INPUT type=\"submit\" value=\"Absenden\">"
    "</P>"
   "</FORM>"
 "</body>"
"</html>";

// Use data uploaded to the ESP (file into folder data)

String randomstring;

void serverHomepage() {
  if (server.hasArg("messagebox")) {
    randomstring = server.arg("messagebox");
  }
  server.send(200, "text/html", INDEX_HTML);
}
