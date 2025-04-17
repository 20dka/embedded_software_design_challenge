#pragma once
#include <string>

extern "C" {
#include "curl/curl.h"
}

class DownloadWrapper
{
	public:
		DownloadWrapper() {
			curl = curl_easy_init();
		}
		~DownloadWrapper() {
			curl_easy_cleanup(curl);
		}

		bool download(std::string url, std::string path);

private:
	CURL* curl;

};

