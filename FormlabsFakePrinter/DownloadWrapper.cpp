#include "DownloadWrapper.h"
#include <iostream>
#include <fstream>

extern "C" {
#include "curl/curl.h"
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    std::ofstream* out = static_cast<std::ofstream*>(stream);
    size_t totalSize = size * nmemb;
    out->write(static_cast<char*>(ptr), totalSize);
    return totalSize;
}

bool DownloadWrapper::download(std::string url, std::string path)
{
    std::ofstream outfile(path, std::ios::binary);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1UL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);

    CURLcode res = curl_easy_perform(curl);
    outfile.close();

    if (res != CURLE_OK)
    {
        std::cerr << "Failed to download image for this layer: " << curl_easy_strerror(res) << std::endl;
        return true;
    }

    return false;
}
