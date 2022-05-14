/*You will most likely need to go here and allow unsecure apps:
https://www.google.com/settings/security/lesssecureapps
*/

#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

#define FROM_MAIL "<yourmail@gmail.com>"
#define TO_MAIL "<tosend@hotmail.com>"

static const char *payload_text =
    "To: " TO_MAIL "\r\n"
    "From: " FROM_MAIL "\r\n"
    "Subject: SMTP example message\r\n"
    "\r\n" /* empty line to divide headers from body, see RFC5322 */
    "The body of the message starts here.\r\n"
    "\r\n"
    "It could be a lot of lines, could be MIME encoded, whatever.\r\n"
    "Check RFC5322.\r\n";

struct upload_status {
  size_t bytes_read;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb,
                             void *userp) {
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;

  if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
    return 0;
  }

  data = &payload_text[upload_ctx->bytes_read];

  if (data) {
    size_t len = strlen(data);
    if (room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;

    return len;
  }

  return 0;
}

int main() {
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = {0};

  curl = curl_easy_init();
  if (curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, "yourmail@gmail.com");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "yourmailpassword");

    curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");

    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    // curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_MAIL);

    recipients = curl_slist_append(recipients, TO_MAIL);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* Free the list of recipients */
    curl_slist_free_all(recipients);

    /* Always cleanup */
    curl_easy_cleanup(curl);
  }

  return (int)res;
}