// idk if this is good code or not, first c++ script

#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
using namespace std;

static size_t callback(const char *in, size_t size, size_t num, char *out)
{
	string data(in, (size_t) size *num);
	*((stringstream*) out) << data;
	return size * num;
}

int httpCode(0);
stringstream readBuffer;

void getUrl(string url)
{
	CURL * curl;
	curl = curl_easy_init();

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 11.1; rv:84.0) Gecko/20100101 Firefox/84.0");

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_perform(curl);	// perform curl command
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(curl);
}

int main(void)
{
	string username;
	cout << "[!] Insert GitHub username: "; cin >> username;

	const string url = "https://api.github.com/users/" + username;

	CURL * curl;
	curl = curl_easy_init();

	if (curl)
	{
		getUrl(url);

		if (httpCode == 200)
		{
			cout << "\n[+] Got 200 code response from " << url << endl;

			Json::Value jsonData;
			Json::CharReaderBuilder jsonReader;
			string errs;

			if (Json::parseFromStream(jsonReader, readBuffer, &jsonData, &errs))
			{
				
				const string userID(jsonData["id"].asString());
				const string userName(jsonData["name"].asString());
				const string userEmail(jsonData["email"].asString());
				const string location(jsonData["location"].asString());

				cout << "\n[*] ID: " << userID << endl;
				cout << "[*] Username: " << userName << endl;
				cout << "[*] Location: " << location << endl;
				if (userEmail.empty())
				{
					const string urlCommits = "https://api.github.com/users/" + username + "/events";
					getUrl(urlCommits);
					if (Json::parseFromStream(jsonReader, readBuffer, &jsonData, &errs))
					{
						auto haha = jsonData[0]["payload"]["commits"];

						cout << "[*] Email: Hidden" << endl;
						cout << "\n[?] Checking for email in commits..." << endl;

						if (jsonData[0].isMember("payload"))
							cout << "[*] Email found: \033[3;100;30m" << haha[0]["author"]["email"].asString() << "\033[0m\n";
						else
							cout << "[!] No email found.";

					}
				}
				else
				{
					cout << "[*] Email: " << userEmail << endl;
				}
			}
		} else {
			cout << "\n[!] Error happened when getting response from " << url << endl;
		}
	}
	return 0;
}
